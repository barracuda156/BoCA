 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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
#include "worker.h"

BoCA_BEGIN_COMPONENT(EncoderFDKAAC)

namespace BoCA
{
	class EncoderFDKAAC : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;
			Config			*config;

			Array<SuperWorker *>	 workers;

			MP4FileHandle		 mp4File;
			MP4TrackId		 mp4Track;

			Int			 nextWorker;

			Int			 frameSize;
			Int			 granuleSize;

			Int			 blockSize;
			Int			 overlap;

			Int64			 totalFrames;
			Int64			 totalSamples;
			Int			 delaySamples;

			Buffer<int16_t>		 samplesBuffer;

			Int			 EncodeFrames(Bool);
			Int			 ProcessPackets(const Buffer<unsigned char> &, const Array<Int> &, Bool);

			static UnsignedInt32	 GetEncoderVersion();

			static Bool		 ConvertArguments(Config *);
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
