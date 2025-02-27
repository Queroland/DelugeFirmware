/*
 * Copyright (c) 2014-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once
#include "gui/menu_item/formatted_title.h"
#include "gui/menu_item/source/patched_param.h"

namespace deluge::gui::menu_item::envelope {
class Segment : public source::PatchedParam, public FormattedTitle {
public:
	Segment(l10n::String name, l10n::String title_format_str, int32_t newP)
	    : PatchedParam(name, newP), FormattedTitle(title_format_str) {}

	[[nodiscard]] std::string_view getTitle() const override { return FormattedTitle::title(); }
	void getColumnLabel(StringBuf& label) override {
		// For envelope segments the first letter is perfect short name: ADSR is well known.
		label.append(MenuItem::getName().data()[0]);
	}
};
} // namespace deluge::gui::menu_item::envelope
