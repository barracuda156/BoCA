 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "editor.h"

BoCA::Editor::Editor(const String &title) : Layer(title)
{
	allowTrackChangeByArrowKey.Connect(True);
	allowTrackRemoveByDeleteKey.Connect(True);
}

BoCA::Editor::~Editor()
{
}

Void BoCA::Editor::OnSelectTrack(const Track &track)
{
}

Void BoCA::Editor::OnSelectAlbum(const Track &track)
{
}

Void BoCA::Editor::OnSelectNone()
{
}
