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
#include "gui/menu_item/sync_level.h"
#include "gui/ui/sound_editor.h"
#include "processing/engines/audio_engine.h"
#include "processing/sound/sound.h"

namespace deluge::gui::menu_item::sidechain {
class Sync final : public SyncLevel {
public:
	using SyncLevel::SyncLevel;

	size_t size() override { return 10; };
	void readCurrentValue() override {
		this->setValue(syncTypeAndLevelToMenuOption(soundEditor.currentSidechain->syncType,
		                                            soundEditor.currentSidechain->syncLevel));
	}
	void writeCurrentValue() override {
		soundEditor.currentSidechain->syncType = syncValueToSyncType(this->getValue());
		soundEditor.currentSidechain->syncLevel = syncValueToSyncLevel(this->getValue());
		AudioEngine::mustUpdateReverbParamsBeforeNextRender = true;
	}
	bool isRelevant(ModControllableAudio* modControllable, int32_t whichThing) override {
		return !soundEditor.editingReverbSidechain() || AudioEngine::reverbSidechainVolume >= 0;
	}
};
} // namespace deluge::gui::menu_item::sidechain
