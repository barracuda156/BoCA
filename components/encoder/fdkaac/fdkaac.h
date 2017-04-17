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

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(EncoderFDKAAC)

namespace BoCA
{
	class EncoderFDKAAC : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			MP4FileHandle		 mp4File;
			HANDLE_AACENCODER	 handle;

			Int			 mp4Track;
			Int			 sampleId;

			Int			 frameSize;

			Int			 totalSamples;
			Int			 delaySamples;

			Buffer<unsigned char>	 outBuffer;
			Buffer<int16_t>		 samplesBuffer;

			Int			 EncodeFrames(Bool);

			Int			 GetSampleRateIndex(Int) const;
		public:
			static const String	&GetComponentSpecs();

						 EncoderFDKAAC();
						~EncoderFDKAAC();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			Bool			 SetOutputFormat(Int);
			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderFDKAAC)

BoCA_END_COMPONENT(EncoderFDKAAC)
