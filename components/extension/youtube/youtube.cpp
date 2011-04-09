 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "youtube.h"
#include "config.h"

const String &BoCA::YouTube::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>YouTube Video Downloader</name>	\
	    <version>1.0</version>			\
	    <id>youtube-ext</id>			\
	    <type>extension</type>			\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::YouTube::YouTube()
{
	configLayer  = NIL;
	mainTabLayer = NIL;

	getMainTabLayer.Connect(&YouTube::GetMainTabLayer, this);
}

BoCA::YouTube::~YouTube()
{
	if (configLayer	 != NIL) Object::DeleteObject(configLayer);
	if (mainTabLayer != NIL) Object::DeleteObject(mainTabLayer);
}

ConfigLayer *BoCA::YouTube::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureYouTube();

	return configLayer;
}

Layer *BoCA::YouTube::GetMainTabLayer()
{
	if (mainTabLayer == NIL) mainTabLayer = new LayerYouTube();

	return mainTabLayer;
}
