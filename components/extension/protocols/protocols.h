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

#include <boca.h>
#include "layer.h"

BoCA_BEGIN_COMPONENT(Protocols)

namespace BoCA
{
	class Protocols : public CS::ExtensionComponent
	{
		private:
			LayerProtocols		*mainTabLayer;
		public:
			static const String	&GetComponentSpecs();

						 Protocols();
						~Protocols();
		callbacks:
			Layer			*GetMainTabLayer();
	};
};

BoCA_DEFINE_EXTENSION_COMPONENT(Protocols)

BoCA_END_COMPONENT(Protocols)
