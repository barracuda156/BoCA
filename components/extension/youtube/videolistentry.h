 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_YOUTUBE_VIDEOLISTENTRY
#define H_YOUTUBE_VIDEOLISTENTRY

#include <smooth.h>
#include <boca.h>

#include "video.h"

using namespace smooth;

using namespace BoCA;

namespace BoCA
{
	class VideoListEntry : public ListEntry
	{
		private:
			Bool		 downloadFinished;

			PopupMenu	*contextMenu;
		protected:
			Video		*video;

			Image		*image;
			Progressbar	*progress;

			Image		*CreateImageFromVideo();
		public:
					 VideoListEntry(Video *);
			virtual		~VideoListEntry();

			virtual Int	 Paint(Int);

			virtual Bool	 IsTypeCompatible(Short) const;
		accessors:
			Video		*GetVideo() { return video; }
		slots:
			Void		 OnFinishDownload();

			Void		 OnChangeSize(const Size &);

			PopupMenu	*GetContextMenu();
	};
};

#endif
