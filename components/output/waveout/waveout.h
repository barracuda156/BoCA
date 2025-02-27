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
#include <windows.h>

BoCA_BEGIN_COMPONENT(OutputWaveOut)

namespace BoCA
{
	class OutputWaveOut : public CS::OutputComponent
	{
		private:
			HWAVEOUT			 hWaveOut;
			Array<WAVEHDR *, Void *>	 headers;

			Threads::Thread			*thread;
			Bool				 stop;

			CRITICAL_SECTION		 sync;

			HANDLE				 hEvent;
			char				*buffer;
			UINT				 buf_size, buf_size_used;
			UINT				 data_written, write_ptr;
			UINT				 minblock, maxblock, avgblock;
			DWORD				 last_time;
			DWORD				 p_time;

			UINT				 n_playing;
			Bool				 paused;
			Bool				 needplay;
			Bool				 newpause;

			Void				 WorkerThread();
		public:
			static const String		&GetComponentSpecs();

							 OutputWaveOut();
							~OutputWaveOut();

			Bool				 Activate();
			Bool				 Deactivate();

			Int				 WriteData(Buffer<UnsignedByte> &);

			Int				 CanWrite();

			Int				 SetPause(Bool);
			Bool				 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(OutputWaveOut)

BoCA_END_COMPONENT(OutputWaveOut)
