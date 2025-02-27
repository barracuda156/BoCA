 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifdef __WIN32__
#	include <windows.h>
#	include <mmreg.h>
#else
#	define WAVE_FORMAT_PCM	      0x0001
#	define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#include "wave.h"

using namespace smooth::IO;

const String &BoCA::EncoderWave::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>Windows Wave File Output</name>			\
	    <version>1.0</version>					\
	    <id>wave-enc</id>						\
	    <type>encoder</type>					\
	    <format>							\
	      <name>Microsoft Wave Files</name>				\
	      <extension>wav</extension>				\
	      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
	      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
	      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>		\
	    </format>							\
	    <input bits=\"8\" signed=\"false\"/>			\
	    <input bits=\"16-32\"/>					\
	  </component>							\
									\
	";

	return componentSpecs;
}

BoCA::EncoderWave::EncoderWave()
{
}

BoCA::EncoderWave::~EncoderWave()
{
}

Bool BoCA::EncoderWave::Activate()
{
	Buffer<unsigned char>	 buffer(44);
	OutStream		*out = new OutStream(STREAM_BUFFER, buffer, 44);

	const Format	&format = track.GetFormat();

	out->OutputString("RIFF");
	out->OutputNumber(track.length * format.channels * (format.bits / 8) + 36, 4);
	out->OutputString("WAVE");

	out->OutputString("fmt ");
	out->OutputNumber(16, 4);
	out->OutputNumber(WAVE_FORMAT_PCM, 2);
	out->OutputNumber(format.channels, 2);
	out->OutputNumber(format.rate, 4);
	out->OutputNumber(format.rate * format.channels * (format.bits / 8), 4);
	out->OutputNumber(format.channels * (format.bits / 8), 2);
	out->OutputNumber(format.bits, 2);

	out->OutputString("data");
	out->OutputNumber(track.length * format.channels * (format.bits / 8), 4);

	delete out;

	driver->WriteData(buffer, 44);

	return True;
}

Bool BoCA::EncoderWave::Deactivate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Config	*config = GetConfiguration();
	const Info	&info = track.GetInfo();

	/* Write data size to header.
	 */
	UnsignedInt	 dataSize = driver->GetSize() - 44;

	driver->Seek(40);

	if (endianness == EndianLittle)	for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &dataSize) + i, 1);
	else				for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &dataSize) + i, 1);

	driver->Seek(driver->GetSize());

	/* Write RIFF tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableRIFFINFOTag", True) && info.HasBasicInfo())
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 tagBuffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(tagBuffer, track);

			driver->WriteData(tagBuffer, tagBuffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Write CART tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableRIFFCartTag", True) && info.HasBasicInfo())
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("cart-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 tagBuffer;

			tagger->SetConfiguration(GetConfiguration());
			tagger->RenderBuffer(tagBuffer, track);

			driver->WriteData(tagBuffer, tagBuffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

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

			driver->WriteData((unsigned char *) "id3 ", 4);

			Int	 size = id3Buffer.Size();

			if (endianness == EndianLittle) for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &size) + i, 1);
			else				for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &size) + i, 1);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Write file size to header.
	 */
	UnsignedInt	 fileSize = driver->GetSize() - 8;

	driver->Seek(4);

	if (endianness == EndianLittle)	for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
	else				for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);

	driver->Seek(driver->GetSize());

	return True;
}

Int BoCA::EncoderWave::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(data, track.GetFormat().bits / 8);

	return driver->WriteData(data, data.Size());
}
