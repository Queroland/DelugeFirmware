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
#include "definitions_cxx.hpp"
#include "gui/menu_item/arpeggiator/midi_cv/arp_integer.h"
#include "gui/menu_item/value_scaling.h"
#include "gui/ui/sound_editor.h"
#include "model/song/song.h"

namespace deluge::gui::menu_item::arpeggiator::midi_cv {
class BassProbability final : public ArpNonSoundInteger {
public:
	using ArpNonSoundInteger::ArpNonSoundInteger;
	void readCurrentValue() override {
		this->setValue(computeCurrentValueForUnsignedMenuItem(soundEditor.currentArpSettings->bassProbability));
	}
	void writeCurrentValue() override {
		int32_t value = computeFinalValueForUnsignedMenuItem(this->getValue());
		soundEditor.currentArpSettings->bassProbability = value;
	}
	bool isRelevant(ModControllableAudio* modControllable, int32_t whichThing) override {
		return soundEditor.editingCVOrMIDIClip() || soundEditor.editingMidiDrumRow();
	}
};
} // namespace deluge::gui::menu_item::arpeggiator::midi_cv
