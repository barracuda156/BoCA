 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_YOUTUBE_LAYER
#define H_YOUTUBE_LAYER

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class Video;
	class VideoSite;
	class VideoList;

	class LayerYouTube : public Layer
	{
		private:
			Array<VideoSite *>	 sites;
			Bool			 missingDecoders;

			Array<Track>		 tracks;

			Text			*text_url;
			EditBox			*edit_url;
			Button			*button_add_url;

			CheckBox		*check_auto_download;
			CheckBox		*check_keep_files;

			Text			*text_downloads;
			VideoList		*list_downloads;

			Text			*text_tracks;
			ListBox			*list_tracks;

			ActiveArea		*area_cover;
			Image			*image_cover;

			Text			*text_site;
			Text			*text_site_value;

			Text			*text_source;
			Hyperlink		*link_source;

			Text			*text_title;
			EditBox			*edit_title;

			Text			*text_description;
			MultiEdit		*edit_description;

			Text			*text_uploader;
			Text			*text_uploader_value;

			Text			*text_date;
			Text			*text_date_value;

			S::System::Timer	*timer_check_clipboard;

			String			 previousClipboardText;

			Void			 LoadVideoSites();
			Void			 FreeVideoSites();

			VideoSite		*GetVideoSiteForURL(const String &);

			String			 GetClipboardText();

			Bool			 StartDownload(const String &);
			Bool			 FinishDownload(Video *);
		slots:
			Void			 OnApplicationModifyTrack(const Track &);
			Void			 OnApplicationRemoveTrack(const Track &);
			Void			 OnApplicationSelectTrack(const Track &);

			Void			 OnApplicationRemoveAllTracks();

			Void			 OnTimerCheckClipboard();

			Void			 OnEditDownloadURL();
			Void			 OnDownloadTrack();
			Void			 OnFinishDownload(Video *);

			Void			 OnSelectTrack();
			Void			 OnSelectNone();

			Void			 OnEditMetadata();

			Void			 OnShowLayer();
			Void			 OnQuit();

			Void			 OnChangeSize(const Size &);
			Void			 OnChangeLanguageSettings();
		public:
						 LayerYouTube();
						~LayerYouTube();
	};
};

#endif
