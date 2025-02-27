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

#include <boca/component/taggercomponent.h>

BoCA::CS::TaggerComponent::TaggerComponent()
{
}

BoCA::CS::TaggerComponent::~TaggerComponent()
{
}

Void BoCA::CS::TaggerComponent::SetVendorString(const String &vendor)
{
	vendorString = vendor;
}

Error BoCA::CS::TaggerComponent::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	return Error();
}

Error BoCA::CS::TaggerComponent::ParseStreamInfo(const String &file, Track &track)
{
	return Error();
}

Error BoCA::CS::TaggerComponent::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	return Error();
}

Error BoCA::CS::TaggerComponent::RenderStreamInfo(const String &file, const Track &track)
{
	return Error();
}

Error BoCA::CS::TaggerComponent::UpdateStreamInfo(const String &file, const Track &track)
{
	errorState  = True;
	errorString = "Not implemented";

	return Error();
}
