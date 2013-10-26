 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "donate.h"
#include "dialog.h"

const String &BoCA::Donate::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Donate</name>				\
	    <version>1.0</version>			\
	    <id>donate-ext</id>				\
	    <type>extension</type>			\
	  </component>					\
							\
	";

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
}

Void smooth::DetachDLL()
{
}

BoCA::Donate::Donate()
{
	static Bool	 checkedDonation = False;

	dialog = NIL;

	if (checkedDonation) return;

	Config	*config = Config::Get();

	Int	 startCount = config->GetIntValue("Donate", "StartCount", 0) + 1;

	if (startCount % 10 == 0 && config->GetIntValue("Donate", "ShowAgain", True))
	{
		dialog = new DonateDialog();

		dialog->ShowDialog();
	}

	config->SetIntValue("Donate", "StartCount", startCount);

	checkedDonation = True;
}

BoCA::Donate::~Donate()
{
	if (dialog != NIL) Object::DeleteObject(dialog);
}
