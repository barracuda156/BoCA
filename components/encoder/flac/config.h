 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_FLACCONFIG
#define H_FLACCONFIG

#include <smooth.h>
#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;

namespace BoCA
{
	class ConfigureFLAC : public ConfigLayer
	{
		private:
			TabWidget		*tabwidget;

			Layer			*layer_simple;

			GroupBox		*group_preset;
			Text			*text_preset;
			ComboBox		*combo_preset;

			GroupBox		*group_file_format;
			OptionBox		*option_file_format_flac;
			OptionBox		*option_file_format_ogg;

			GroupBox		*group_stereo;
			CheckBox		*check_mid_side_stereo;
			CheckBox		*check_loose_mid_side;

			Layer			*layer_format;

			GroupBox		*group_format;
			CheckBox		*check_streamable_subset;
			Text			*text_blocksize;
			Slider			*slider_blocksize;
			EditBox			*edit_blocksize;
			Text			*text_blocksize_bytes;

			Layer			*layer_advanced;

			GroupBox		*group_apodization;
			Text			*text_apodization;
			EditBox			*edit_apodization;
			ListBox			*list_apodization;
			Text			*text_apodization_note;
			Text			*text_apodization_explain;

			GroupBox		*group_lpc;
			Text			*text_max_lpc_order;
			Slider			*slider_max_lpc_order;
			Text			*text_max_lpc_order_value;
			CheckBox		*check_exhaustive_model;
			Text			*text_qlp_precision;
			Slider			*slider_qlp_precision;
			Text			*text_qlp_precision_value;
			CheckBox		*check_qlp_precision_search;

			GroupBox		*group_rice;
			Text			*text_min_part_order;
			Slider			*slider_min_part_order;
			Text			*text_min_part_order_value;
			Text			*text_max_part_order;
			Slider			*slider_max_part_order;
			Text			*text_max_part_order_value;

			Int			 preset;
			Int			 file_format;
			Bool			 streamable_subset;
			Bool			 do_mid_side_stereo;
			Bool			 loose_mid_side_stereo;
			Int			 blocksize;
			Int			 max_lpc_order;
			Int			 qlp_coeff_precision;
			Bool			 do_qlp_coeff_prec_search;
			Bool			 do_exhaustive_model_search;
			Int			 min_residual_partition_order;
			Int			 max_residual_partition_order;
		slots:
			Void			 SetPreset();
			Void			 SetLPCOrder();
			Void			 SetQLPSearch();
			Void			 SetQLPPrecision();
			Void			 SetRiceOrder();
			Void			 SetStereoMode();
			Void			 SetStreamableSubset();
			Void			 SetBlockSize();
			Void			 EditBlockSize();
		public:
			static const String	 ConfigID;

						 ConfigureFLAC();
						~ConfigureFLAC();

			Int			 SaveSettings();
	};
};

#endif
