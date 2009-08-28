 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_TAGEDIT_LAYER_TAG_BASIC
#define H_TAGEDIT_LAYER_TAG_BASIC

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

#include "editor.h"

namespace BoCA
{
	class LayerTagBasic : public Editor
	{
		private:
			GroupBox			*group_info;

			Text				*text_artist;
			EditBox				*edit_artist;

			Text				*text_title;
			EditBox				*edit_title;

			Text				*text_album;
			EditBox				*edit_album;

			Text				*text_genre;
			EditBox				*edit_genre;
			ListBox				*list_genre;

			Text				*text_year;
			EditBox				*edit_year;

			Text				*text_track;
			EditBox				*edit_track;
			Text				*text_ntracks;
			EditBox				*edit_ntracks;

			Text				*text_disc;
			EditBox				*edit_disc;
			Text				*text_ndiscs;
			EditBox				*edit_ndiscs;

			Text				*text_comment;
			MultiEdit			*edit_comment;

			GroupBox			*group_cover;

			Text				*text_covers;
			ImageBox			*image_covers;

			Button				*button_cover_add;
			Button				*button_cover_remove;

			Text				*text_cover_type;
			ComboBox			*combo_cover_type;

			Text				*text_cover_desc;
			MultiEdit			*edit_cover_desc;

			Track				 track;

			Void				 LoadCoverImages();
			Void				 FreeCoverImages();
		slots:
			Void				 OnChangeSize(const Size &);

			Void				 AddCover();
			Void				 RemoveCover();

			Void				 SelectCover(ListEntry *);
			Void				 DisplayCover();

			Void				 OnSelectTrack(const Track &);
			Void				 OnSelectNone();

			Void				 OnModifyTrack();
		public:
							 LayerTagBasic();
							~LayerTagBasic();
	};
};

#endif
