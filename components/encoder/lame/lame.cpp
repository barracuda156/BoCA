 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

#include "lame.h"
#include "config.h"

const String &BoCA::EncoderLAME::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (lamedll != NIL)
	{
		componentSpecs = "								\
												\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>					\
		  <component>									\
		    <name>LAME MP3 Encoder %VERSION%</name>					\
		    <version>1.0</version>							\
		    <id>lame-enc</id>								\
		    <type>encoder</type>							\
		    <format>									\
		      <name>MPEG 1 Audio Layer 3</name>						\
		      <extension>mp3</extension>						\
		      <tag id=\"id3v1-tag\" mode=\"append\">ID3v1</tag>				\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>			\
		    </format>									\
		    <input bits=\"16\" channels=\"1-2\"						\
			   rate=\"8000,11025,12000,16000,22050,24000,32000,44100,48000\"/>	\
		    <input float=\"true\" channels=\"1-2\"					\
			   rate=\"8000,11025,12000,16000,22050,24000,32000,44100,48000\"/>	\
		    <parameters>								\
		      <selection name=\"Mode\" argument=\"-m %VALUE\" default=\"VBR\">		\
			<option alias=\"Constant Bitrate\">CBR</option>				\
			<option alias=\"Variable Bitrate\">VBR</option>				\
			<option alias=\"Average Bitrate\">ABR</option>				\
		      </selection>								\
		      <range name=\"CBR/ABR bitrate\" argument=\"-b %VALUE\" default=\"192\">	\
			<min alias=\"min\">8</min>						\
			<max alias=\"max\">320</max>						\
		      </range>									\
		      <range name=\"VBR quality\" argument=\"-q %VALUE\" default=\"5\">		\
			<min alias=\"best\">0</min>						\
			<max alias=\"worst\">9</max>						\
		      </range>									\
		    </parameters>								\
		  </component>									\
												\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_get_lame_short_version()));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadLAMEDLL();
}

Void smooth::DetachDLL()
{
	FreeLAMEDLL();
}

BoCA::EncoderLAME::EncoderLAME()
{
	configLayer  = NIL;
	config	     = NIL;

	dataOffset   = 0;
	frameSize    = 0;

	blockSize    = 128;
	overlap	     = 4;

	nextWorker   = 0;

	totalSamples = 0;

	repacker     = NIL;
}

BoCA::EncoderLAME::~EncoderLAME()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderLAME::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info	= track.GetInfo();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	Int	 preset		  = config->GetIntValue(ConfigureLAME::ConfigID, "Preset", 2);
	Int	 vbrMode	  = config->GetIntValue(ConfigureLAME::ConfigID, "VBRMode", 4);
	Bool	 setBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "SetBitrate", 1);
	Int	 bitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "Bitrate", 192);
	Int	 ratio		  = config->GetIntValue(ConfigureLAME::ConfigID, "Ratio", 1100);
	Int	 abrBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "ABRBitrate", 192);
	Int	 vbrQuality	  = config->GetIntValue(ConfigureLAME::ConfigID, "VBRQuality", 50);
	Bool	 setMinVBRBitrate = config->GetIntValue(ConfigureLAME::ConfigID, "SetMinVBRBitrate", 0);
	Bool	 setMaxVBRBitrate = config->GetIntValue(ConfigureLAME::ConfigID, "SetMaxVBRBitrate", 0);
	Int	 minVBRBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "MinVBRBitrate", 128);
	Int	 maxVBRBitrate	  = config->GetIntValue(ConfigureLAME::ConfigID, "MaxVBRBitrate", 256);
	Bool	 strictISO	  = config->GetIntValue(ConfigureLAME::ConfigID, "StrictISO", 0);
	Int	 stereoMode	  = config->GetIntValue(ConfigureLAME::ConfigID, "StereoMode", 0);

	/* Create and configure LAME encoder.
	 */
	lame_t	 context = ex_lame_init();

	ex_lame_set_in_samplerate(context, format.rate);
	ex_lame_set_num_channels(context, format.channels);

	switch (preset)
	{
		case 0:
			ex_lame_set_strict_ISO(context, strictISO);

			/* Set bitrate.
			 */
			if (vbrMode == vbr_off)
			{
				if (setBitrate) ex_lame_set_brate(context, bitrate);
				else		ex_lame_set_compression_ratio(context, ratio / 100.0);
			}

			/* Set Stereo mode.
			 */
			if	(stereoMode == 1) ex_lame_set_mode(context, MONO);
			else if (stereoMode == 2) ex_lame_set_mode(context, STEREO);
			else if (stereoMode == 3) ex_lame_set_mode(context, JOINT_STEREO);
			else			  ex_lame_set_mode(context, NOT_SET);

			/* Set VBR mode.
			 */
			switch (vbrMode)
			{
				default:
				case vbr_off:
					break;
				case vbr_abr:
					ex_lame_set_VBR(context, vbr_abr);
					ex_lame_set_VBR_mean_bitrate_kbps(context, abrBitrate);
					break;
				case vbr_rh:
					ex_lame_set_VBR(context, vbr_rh);
					ex_lame_set_VBR_quality(context, vbrQuality / 10.0);
					break;
				case vbr_mtrh:
					ex_lame_set_VBR(context, vbr_mtrh);
					ex_lame_set_VBR_quality(context, vbrQuality / 10.0);
					break;
			}

			if (vbrMode != vbr_off && setMinVBRBitrate) ex_lame_set_VBR_min_bitrate_kbps(context, minVBRBitrate);
			if (vbrMode != vbr_off && setMaxVBRBitrate) ex_lame_set_VBR_max_bitrate_kbps(context, maxVBRBitrate);

			break;
		case 1:
			ex_lame_set_preset(context, MEDIUM_FAST);
			break;
		case 2:
			ex_lame_set_preset(context, STANDARD_FAST);
			break;
		case 3:
			ex_lame_set_preset(context, EXTREME_FAST);
			break;
		case 4:
			ex_lame_set_preset(context, abrBitrate);
			break;
	}

	if (ex_lame_init_params(context) < 0)
	{
		errorString = "Bad LAME encoder settings!\n\nPlease check your encoder settings in the\nconfiguration dialog.";
		errorState  = True;

		return False;
	}

	Int	 outSamplerate = ex_lame_get_out_samplerate(context);

	frameSize  = ex_lame_get_framesize(context);

	ex_lame_close(context);

	dataOffset = 0;

	/* Write ID3v2 tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableID3v2", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(config);
			tagger->RenderBuffer(id3Buffer, track);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);

			dataOffset = id3Buffer.Size();
		}
	}

	/* Get number of threads to use.
	 */
	Bool	 enableParallel	 = config->GetIntValue("Resources", "EnableParallelConversions", True);
	Bool	 enableSuperFast = config->GetIntValue("Resources", "EnableSuperFastMode", True) && format.rate == outSamplerate;
	Int	 numberOfThreads = enableParallel && enableSuperFast ? config->GetIntValue("Resources", "NumberOfConversionThreads", 0) : 1;

	if (enableParallel && enableSuperFast && numberOfThreads <= 1) numberOfThreads = CPU().GetNumCores() + (CPU().GetNumLogicalCPUs() - CPU().GetNumCores()) / 2;

	/* Disable overlap if we use only one thread.
	 */
	if (numberOfThreads == 1) overlap = 0;
	else			  overlap = 4 * 1152 / frameSize;

	/* Start up worker threads.
	 */
	for (Int i = 0; i < numberOfThreads; i++) workers.Add(new SuperWorker(config, format, overlap));

	foreach (SuperWorker *worker, workers) worker->Start();

	/* Create repacker instance.
	 */
	repacker = new SuperRepacker(driver);

	repacker->EnableRateControl(setMinVBRBitrate ? minVBRBitrate * 1000 :	8000,
				    setMaxVBRBitrate ? maxVBRBitrate * 1000 : 320000);

	return True;
}

Bool BoCA::EncoderLAME::Deactivate()
{
	const Info	&info = track.GetInfo();

	/* Output remaining samples to encoder.
	 */
	EncodeFrames(True);

	/* Write Xing or Info header.
	 */
	Buffer<UnsignedByte>	 buffer;

	workers.GetFirst()->GetInfoTag(buffer);

	if (workers.Length() > 1) repacker->UpdateInfoTag(buffer, totalSamples);

	driver->Seek(dataOffset);
	driver->WriteData(buffer, buffer.Size());

	/* Delete repacker instance.
	 */
	delete repacker;

	/* Tear down worker threads.
	 */
	foreach (SuperWorker *worker, workers) worker->Quit();
	foreach (SuperWorker *worker, workers) worker->Wait();
	foreach (SuperWorker *worker, workers) delete worker;

	workers.RemoveAll();

	/* Write ID3v1 tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableID3v1", False) && info.HasBasicInfo())
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(config);
			tagger->RenderBuffer(id3Buffer, track);

			driver->Seek(driver->GetSize());
			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True) && config->GetIntValue("Tags", "EnableID3v2", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(config);
			tagger->RenderBuffer(id3Buffer, track);

			driver->Seek(0);
			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderLAME::WriteData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Copy data to samples buffer.
	 */
	Int	 size = data.Size();

	samplesBuffer.Resize(samplesBuffer.Size() + size);

	memcpy(samplesBuffer + samplesBuffer.Size() - size, data, size);

	/* Output samples to encoder.
	 */
	totalSamples += size / format.channels / (format.bits / 8);

	return EncodeFrames(False);
}

Int BoCA::EncoderLAME::EncodeFrames(Bool flush)
{
	const Format	&format = track.GetFormat();

	/* Pass samples to workers.
	 */
	Int	 framesToProcess = blockSize;
	Int	 framesProcessed = 0;
	Int	 dataLength	 = 0;

	Int	 samplesPerFrame = frameSize * format.channels;
	Int	 samplesInBuffer = samplesBuffer.Size() / (format.bits / 8);

	if (flush) framesToProcess = Math::Floor(samplesInBuffer / samplesPerFrame);

	while (samplesInBuffer - framesProcessed * samplesPerFrame >= samplesPerFrame * framesToProcess)
	{
		SuperWorker	*workerToUse = workers.GetNth(nextWorker % workers.Length());

		while (!workerToUse->IsReady()) S::System::System::Sleep(1);

		workerToUse->Lock();

		/* See if the worker has some packets for us.
		 */
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessResults(workerToUse, nextWorker == workers.Length());

		/* Pass new frames to worker.
		 */
		workerToUse->Encode(samplesBuffer, framesProcessed * samplesPerFrame, flush ? samplesInBuffer : samplesPerFrame * framesToProcess, flush);
		workerToUse->Release();

		framesProcessed += framesToProcess - overlap;

		nextWorker++;

		if (flush) break;
	}

	Int	 bytesProcessed = framesProcessed * samplesPerFrame * (format.bits / 8);

	memmove((UnsignedByte *) samplesBuffer, (UnsignedByte *) samplesBuffer + bytesProcessed, samplesBuffer.Size() - bytesProcessed);

	samplesBuffer.Resize(samplesBuffer.Size() - bytesProcessed);

	if (!flush) return dataLength;

	/* Wait for workers to finish and process packets.
	 */
	for (Int i = 0; i < workers.Length(); i++)
	{
		SuperWorker	*workerToUse = workers.GetNth(nextWorker % workers.Length());

		while (!workerToUse->IsReady()) S::System::System::Sleep(1);

		workerToUse->Lock();

		/* See if the worker has some packets for us.
		 */
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessResults(workerToUse, nextWorker == workers.Length());

		workerToUse->Release();

		nextWorker++;
	}

	/* Flush repacker.
	 */
	repacker->Flush();

	return dataLength;
}

Int BoCA::EncoderLAME::ProcessResults(SuperWorker *worker, Bool first)
{
	Int	 processed  = 0;
	Bool	 complete   = False;

	if (workers.Length() == 1) return ProcessPackets(worker->GetPackets(), worker->GetPacketSizes(), first, processed, complete);

	Int	 dataLength = 0;

	for (Int round = 0; !complete; round++)
	{
		dataLength += ProcessPackets(worker->GetPackets(), worker->GetPacketSizes(), first, processed, complete);

		/* Reduce overlap if not finished after 4 rounds.
		 */
		if (round & 4 && overlap > 0) { round = 0; overlap--; processed++; }

		/* Re-encode remaining frames if a frame didn't fit.
		 */
		if (!complete) worker->ReEncode(processed, round);
	}

	overlap = 4 * 1152 / frameSize;

	return dataLength;
}

Int BoCA::EncoderLAME::ProcessPackets(const Buffer<unsigned char> &data, const Array<Int> &chunkSizes, Bool first, Int &processed, Bool &complete)
{
	if (workers.Length() == 1) return driver->WriteData(data, data.Size());

	Buffer<UnsignedByte>	 packets;
	Array<Int>		 packetSizes;

	repacker->UnpackFrames(data, packets, packetSizes);

	Int	 offset	    = 0;
	Int	 dataLength = 0;

	if (!first) for (Int i = 0; i < overlap; i++) offset += packetSizes.GetNth(i);

	processed = 0;
	complete  = False;

	for (Int i = 0; i < packetSizes.Length(); i++)
	{
		if (i <	overlap && !first)	continue;
		if (packetSizes.GetNth(i) == 0) continue;

		if (!repacker->WriteFrame(packets + offset, packetSizes.GetNth(i))) return dataLength;

		processed++;

		offset	   += packetSizes.GetNth(i);
		dataLength += packetSizes.GetNth(i);
	}

	complete = True;

	return dataLength;
}

Bool BoCA::EncoderLAME::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "lame-enc";

	/* Get command line settings.
	 */
	Int	 bitrate = 192;
	Int	 quality = 5;
	String	 mode	 = "VBR";

	if (config->GetIntValue(encoderID, "Set CBR/ABR bitrate", False)) bitrate = config->GetIntValue(encoderID, "CBR/ABR bitrate", bitrate);
	if (config->GetIntValue(encoderID, "Set VBR quality", False))	  quality = config->GetIntValue(encoderID, "VBR quality", quality);
	if (config->GetIntValue(encoderID, "Set Mode", False))		  mode	  = config->GetStringValue(encoderID, "Mode", mode).ToUpper();

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureLAME::ConfigID, "Preset", 0);
	config->SetIntValue(ConfigureLAME::ConfigID, "SetBitrate", True);

	config->SetIntValue(ConfigureLAME::ConfigID, "Bitrate", Math::Max(0, Math::Min(320, bitrate)));
	config->SetIntValue(ConfigureLAME::ConfigID, "ABRBitrate", Math::Max(0, Math::Min(320, bitrate)));
	config->SetIntValue(ConfigureLAME::ConfigID, "VBRQuality", Math::Max(0, Math::Min(9, quality)) * 10);

	Int	 vbrMode = vbr_default;

	if	(mode == "VBR") vbrMode = vbr_mtrh;
	else if (mode == "ABR") vbrMode = vbr_abr;
	else if (mode == "CBR") vbrMode = vbr_off;

	config->SetIntValue(ConfigureLAME::ConfigID, "VBRMode", vbrMode);

	return True;
}

ConfigLayer *BoCA::EncoderLAME::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureLAME();

	return configLayer;
}
