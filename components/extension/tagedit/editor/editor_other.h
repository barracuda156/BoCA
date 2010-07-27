 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_TAGEDIT_LAYER_TAG_OTHER
#define H_TAGEDIT_LAYER_TAG_OTHER

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

#include "editor.h"

namespace BoCA
{
	class LayerTagOther : public Editor
	{
		private:
			GroupBox			*group_original;

			Text				*text_oartist;
			EditBox				*edit_oartist;

			Text				*text_oalbum;
			EditBox				*edit_oalbum;

			Text				*text_otextwriter;
			EditBox				*edit_otextwriter;

			Text				*text_oyear;
			EditBox				*edit_oyear;

			GroupBox			*group_web;

			Text				*text_wartist;
			EditBox				*edit_wartist;

			Text				*text_wpublisher;
			EditBox				*edit_wpublisher;

			Text				*text_wradio;
			EditBox				*edit_wradio;

			Text				*text_wsource;
			EditBox				*edit_wsource;

			Text				*text_wcopyright;
			EditBox				*edit_wcopyright;

			Text				*text_wcommercial;
			EditBox				*edit_wcommercial;

			Track				 track;
		slots:
			Void				 OnChangeSize(const Size &);

			Void				 OnSelectTrack(const Track &);
			Void				 OnSelectAlbum(const Track &);
			Void				 OnSelectNone();

			Void				 OnModifyTrack();
		public:
							 LayerTagOther();
							~LayerTagOther();
	};
};

#endif
