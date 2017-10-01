 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <time.h>
#include <stdlib.h>
#include <stdint.h>

#include "opus.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderOpus::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && opusdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Opus Audio Encoder %VERSION%</name>				\
		    <version>1.0</version>						\
		    <id>opus-enc</id>							\
		    <type>encoder</type>						\
		    <require>resample-dsp</require>					\
		    <format>								\
		      <name>Opus Audio</name>						\
		      <extension>opus</extension>					\
		      <extension>oga</extension>					\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
		    </format>								\
		  </component>								\
											\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(String(ex_opus_get_version_string()).Replace("libopus ", NIL)));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOggDLL();
	LoadOpusDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
	FreeOpusDLL();
}

namespace BoCA
{
	/* Constants.
	 */
	const Int	 maxPacketSize = 4000;

	/* Opus header definition.
	 */
	struct OpusHeader
	{
		char		 codec_id[8];	  /**< MUST be "OpusHead" */
		uint8_t		 version_id;	  /**< Version number */
		uint8_t		 nb_channels;	  /**< Number of channels */
		uint16_t	 preskip;	  /**< Pre-skip */
		uint32_t	 sample_rate;	  /**< Input sample rate; informational only */
		int16_t		 output_gain;	  /**< Output gain to apply when decoding */
		uint8_t		 channel_mapping; /**< Channel mapping family */

		uint8_t		 nb_streams;	  /**< Stream count */
		uint8_t		 nb_coupled;	  /**< Two-channel stream count */
		uint8_t		 stream_map[255]; /**< Channel mapping */
	};
};

BoCA::EncoderOpus::EncoderOpus()
{
	configLayer	= NIL;

	encoder		= NIL;
	resampler	= NIL;
	resamplerConfig	= NIL;

	frameSize	= 0;
	preSkip		= 0;
	sampleRate	= 48000;

	numPackets	= 0;
	totalSamples	= 0;

	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));
}

BoCA::EncoderOpus::~EncoderOpus()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderOpus::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Config	*config = GetConfiguration();
	const Format	&format = track.GetFormat();
	Info		 info = track.GetInfo();

	if (format.channels > 8)
	{
		errorString = "This encoder does not support more than 8 channels!";
		errorState  = True;

		return False;
	}

	/* Get best sample rate.
	 */
	if	(format.rate <=  8000) sampleRate =  8000;
	else if (format.rate <= 12000) sampleRate = 12000;
	else if (format.rate <= 16000) sampleRate = 16000;
	else if (format.rate <= 24000) sampleRate = 24000;
	else			       sampleRate = 48000;

	/* Create and init resampler component.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	resampler = (AS::DSPComponent *) boca.CreateComponentByID("resample-dsp");

	if (resampler == NIL)
	{
		errorString = "Could not create resampler component!";
		errorState  = True;

		return False;
	}

	resamplerConfig = Config::Copy(config);
	resamplerConfig->SetIntValue("Resample", "Converter", 2);
	resamplerConfig->SetIntValue("Resample", "Samplerate", sampleRate);

	resampler->SetConfiguration(resamplerConfig);
	resampler->SetAudioTrackInfo(track);
	resampler->Activate();

	if (resampler->GetErrorState() == True)
	{
		errorString = resampler->GetErrorString();
		errorState  = resampler->GetErrorState();

		boca.DeleteComponent(resampler);

		return False;
	}

	/* Init Ogg stream.
	 */
	srand(clock());

	ex_ogg_stream_init(&os, rand());

	dataBuffer.Resize(maxPacketSize * Math::Ceil(format.channels / 2.0));

	/* Create Opus header.
	 */
	OpusHeader	 setup;

	strncpy(setup.codec_id, "OpusHead", 8);

	setup.version_id  = 1;
	setup.nb_channels = format.channels;
	setup.sample_rate = format.rate;
	setup.output_gain = 0;

	if (format.channels <= 2) setup.channel_mapping = 0;
	else			  setup.channel_mapping = 1;

	/* Init Opus encoder.
	 */
	int	 error	 = 0;
	int	 streams = 0;
	int	 coupled = 0;

	encoder = ex_opus_multistream_surround_encoder_create(sampleRate, setup.nb_channels, setup.channel_mapping, &streams, &coupled, setup.stream_map, OPUS_APPLICATION_AUDIO, &error);

	setup.nb_streams = streams;
	setup.nb_coupled = coupled;

	/* Set encoder parameters.
	 */
	if (config->GetIntValue("Opus", "Mode", 0)	!= 0) ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE + config->GetIntValue("Opus", "Mode", 0) - 1));
	if (config->GetIntValue("Opus", "Bandwidth", 0) != 0) ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_NARROWBAND + config->GetIntValue("Opus", "Bandwidth", 0) - 1));

	ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_BITRATE( config->GetIntValue("Opus", "Bitrate", 128) * 1000));
	ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_VBR(config->GetIntValue("Opus", "EnableVBR", True)));
	ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_VBR_CONSTRAINT(config->GetIntValue("Opus", "EnableConstrainedVBR", False)));
	ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(config->GetIntValue("Opus", "Complexity", 10)));
	ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(config->GetIntValue("Opus", "PacketLoss", 0)));
	ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_DTX(config->GetIntValue("Opus", "EnableDTX", False)));
	ex_opus_multistream_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(0));

	/* Get number of pre-skip samples.
	 */
	ex_opus_multistream_encoder_ctl(encoder, OPUS_GET_LOOKAHEAD(&preSkip));

	setup.preskip = preSkip * (48000 / sampleRate);

	frameSize     = Math::Round(Float(sampleRate) / (1000000.0 / config->GetIntValue("Opus", "FrameSize", 20000)));
	totalSamples  = preSkip;
	numPackets    = 0;

	/* Adjust endianness of header fields.
	 */
	if (endianness != EndianLittle)
	{
		BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.preskip, sizeof(setup.preskip));
		BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.sample_rate, sizeof(setup.sample_rate));
		BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.output_gain, sizeof(setup.output_gain));
	}

	/* Write header packet.
	 */
	ogg_packet	 header = { (unsigned char *) &setup, 19 + (setup.channel_mapping == 0 ? 0 : 2 + setup.nb_channels), 1, 0, 0, numPackets };

	ex_ogg_stream_packetin(&os, &header);

	/* Write Vorbis comment header
	 */
	{
		Buffer<unsigned char>	 vcBuffer;

		/* Remove ReplayGain information as per Opus comment spec.
		 */
		info.track_gain = NIL;
		info.track_peak = NIL;
		info.album_gain = NIL;
		info.album_peak = NIL;

		/* Render actual Vorbis comment tag.
		 *
		 * An empty tag containing only the vendor string
		 * is rendered if Vorbis comments are disabled.
		 */
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			const char	*opusVersion = ex_opus_get_version_string();

			tagger->SetConfiguration(GetConfiguration());
			tagger->SetVendorString(String(opusVersion).Append("\n"));

			if (config->GetIntValue("Tags", "EnableVorbisComment", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))) tagger->RenderBuffer(vcBuffer, track);
			else																					    tagger->RenderBuffer(vcBuffer, Track());

			boca.DeleteComponent(tagger);
		}

		vcBuffer.Resize(vcBuffer.Size() + 8);

		memmove(vcBuffer + 8, vcBuffer, vcBuffer.Size() - 8);
		memcpy(vcBuffer, "OpusTags", 8);

		ogg_packet	 header_comm = { vcBuffer, vcBuffer.Size(), 0, 0, 0, numPackets++ };

		ex_ogg_stream_packetin(&os, &header_comm);
	}

	WriteOggPackets(True);

	return True;
}

Bool BoCA::EncoderOpus::Deactivate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	Buffer<UnsignedByte>	 data;

	Int	 size = resampler->Flush(data);

	/* Convert samples to 16 bit.
	 */
	const Format	&format	 = track.GetFormat();
	Int		 samples = size / format.channels / (format.bits / 8);
	Int		 offset	 = samplesBuffer.Size();

	samplesBuffer.Resize(samplesBuffer.Size() + samples * format.channels);

	for (Int i = 0; i < samples * format.channels; i++)
	{
		if	(format.bits ==  8				) samplesBuffer[offset + i] =	    (				  data [i] - 128) * 256;
		else if (format.bits == 16				) samplesBuffer[offset + i] = (int)  ((short *) (unsigned char *) data)[i];
		else if (format.bits == 32				) samplesBuffer[offset + i] = (int) (((long *)  (unsigned char *) data)[i]	  / 65536);

		else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[offset + i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
		else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[offset + i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
	}

	/* Output remaining samples to encoder.
	 */
	EncodeFrames(True);

	/* Write any remaining Ogg packets.
	 */
	WriteOggPackets(True);

	ex_opus_multistream_encoder_destroy(encoder);

	ex_ogg_stream_clear(&os);

	/* Fix chapter marks in Vorbis Comments.
	 */
	FixChapterMarks();

	/* Clean up resampler component.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	resampler->Deactivate();

	boca.DeleteComponent(resampler);

	Config::Free(resamplerConfig);

	return True;
}

Int BoCA::EncoderOpus::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	/* Change to Vorbis channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::Vorbis_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::Vorbis_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::Vorbis_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Default_6_1, Channel::Vorbis_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Default_7_1, Channel::Vorbis_7_1);

	/* Resample data.
	 */
	Int	 size = resampler->TransformData(data);

	/* Convert samples to 16 bit.
	 */
	Int	 samples = size / format.channels / (format.bits / 8);
	Int	 offset	 = samplesBuffer.Size();

	samplesBuffer.Resize(samplesBuffer.Size() + samples * format.channels);

	for (Int i = 0; i < samples * format.channels; i++)
	{
		if	(format.bits ==  8				) samplesBuffer[offset + i] =	    (				  data [i] - 128) * 256;
		else if (format.bits == 16				) samplesBuffer[offset + i] = (int)  ((short *) (unsigned char *) data)[i];
		else if (format.bits == 32				) samplesBuffer[offset + i] = (int) (((long *)  (unsigned char *) data)[i]	  / 65536);

		else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[offset + i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
		else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[offset + i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
	}

	/* Output samples to encoder.
	 */
	return EncodeFrames(False);
}

Int BoCA::EncoderOpus::EncodeFrames(Bool flush)
{
	const Format	&format = track.GetFormat();

	/* Pad end of stream with empty samples.
	 */
	Int	 nullSamples = 0;

	if (flush)
	{
		nullSamples = preSkip;

		if ((samplesBuffer.Size() / format.channels + preSkip) % frameSize > 0) nullSamples += frameSize - (samplesBuffer.Size() / format.channels + preSkip) % frameSize;

		samplesBuffer.Resize(samplesBuffer.Size() + nullSamples * format.channels);

		memset(((signed short *) samplesBuffer) + samplesBuffer.Size() - nullSamples * format.channels, 0, sizeof(short) * nullSamples * format.channels);
	}

	/* Encode samples and build Ogg packets.
	 */
	Int	 framesProcessed = 0;

	while (samplesBuffer.Size() - framesProcessed * frameSize * format.channels >= frameSize * format.channels)
	{
		Int	 dataLength = ex_opus_multistream_encode(encoder, samplesBuffer + framesProcessed * frameSize * format.channels, frameSize, dataBuffer, dataBuffer.Size());

		totalSamples += frameSize;

		op.packet     = dataBuffer;
		op.bytes      = dataLength;
		op.b_o_s      = 0;
		op.e_o_s      =  (flush && samplesBuffer.Size() - framesProcessed * frameSize * format.channels <= frameSize * format.channels) ? 1 : 0;
		op.granulepos = ((flush && samplesBuffer.Size() - framesProcessed * frameSize * format.channels <= frameSize * format.channels) ? totalSamples - nullSamples : totalSamples) * (48000 / sampleRate);
		op.packetno   = numPackets++;

		ex_ogg_stream_packetin(&os, &op);

		framesProcessed++;
	}

	memmove((signed short *) samplesBuffer, ((signed short *) samplesBuffer) + framesProcessed * frameSize * format.channels, sizeof(short) * (samplesBuffer.Size() - framesProcessed * frameSize * format.channels));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * frameSize * format.channels);

	return WriteOggPackets(flush);
}

Int BoCA::EncoderOpus::WriteOggPackets(Bool flush)
{
	Int	 bytes = 0;

	do
	{
		int	 result = 0;

		if (flush) result = ex_ogg_stream_flush(&os, &og);
		else	   result = ex_ogg_stream_pageout(&os, &og);

		if (result == 0) break;

		bytes += driver->WriteData(og.header, og.header_len);
		bytes += driver->WriteData(og.body, og.body_len);
	}
	while (true);

	return bytes;
}

Bool BoCA::EncoderOpus::FixChapterMarks()
{
	if (track.tracks.Length() == 0 || !GetConfiguration()->GetIntValue("Tags", "WriteChapters", True)) return True;

	driver->Seek(0);

	/* Skip first Ogg page and read second into buffer.
	 */
	Buffer<UnsignedByte>	 buffer;
	Int			 position;
	ogg_page		 og;

	for (Int i = 0; i < 2; i++)
	{
		driver->Seek(driver->GetPos() + 26);

		Int		 dataSize    = 0;
		UnsignedByte	 segments    = 0;
		UnsignedByte	 segmentSize = 0;

		driver->ReadData(&segments, 1);

		for (Int i = 0; i < segments; i++) { driver->ReadData(&segmentSize, 1); dataSize += segmentSize; }

		buffer.Resize(27 + segments + dataSize);
		position = driver->GetPos() - segments - 27;

		driver->Seek(position);
		driver->ReadData(buffer, buffer.Size());

		og.header     = buffer;
		og.header_len = 27 + segments;
		og.body	      = buffer + og.header_len;
		og.body_len   = dataSize;
	}

	/* Update chapter marks.
	 */
	if (buffer.Size() > 0)
	{
		Int64	 offset = 0;

		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			const Track	&chapterTrack  = track.tracks.GetNth(i);
			const Format	&chapterFormat = chapterTrack.GetFormat();

			for (Int b = 0; b < buffer.Size() - 23; b++)
			{
				if (buffer[b + 0] != 'C' || buffer[b + 1] != 'H' || buffer[b + 2] != 'A' || buffer[b +  3] != 'P' ||
				    buffer[b + 4] != 'T' || buffer[b + 5] != 'E' || buffer[b + 6] != 'R' || buffer[b + 10] != '=') continue;

				String	 id;

				id[0] = buffer[b + 7];
				id[1] = buffer[b + 8];
				id[2] = buffer[b + 9];

				if (id.ToInt() != i + 1) continue;

				String	 value	= String(offset / chapterFormat.rate / 60 / 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 / 60)).Append(":")
						 .Append(offset / chapterFormat.rate / 60 % 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 % 60)).Append(":")
						 .Append(offset / chapterFormat.rate % 60      < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate % 60)).Append(".")
						 .Append(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) < 100 ?
							(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) <  10 ?  "00" : "0") : "").Append(String::FromInt(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate)));

				for (Int p = 0; p < 12; p++) buffer[b + 11 + p] = value[p];

				break;
			}

			if	(chapterTrack.length	   >= 0) offset += chapterTrack.length;
			else if (chapterTrack.approxLength >= 0) offset += chapterTrack.approxLength;
		}

		/* Write page back to file.
		 */
		ex_ogg_page_checksum_set(&og);

		driver->Seek(position);
		driver->WriteData(buffer, buffer.Size());
	}

	driver->Seek(driver->GetSize());

	return True;
}

String BoCA::EncoderOpus::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	switch (config->GetIntValue("Opus", "FileExtension", 0))
	{
		default:
		case  0: return "opus";
		case  1: return "oga";
	}
}

ConfigLayer *BoCA::EncoderOpus::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureOpus();

	return configLayer;
}
