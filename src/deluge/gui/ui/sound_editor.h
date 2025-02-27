/*
 * Copyright © 2014-2023 Synthstrom Audible Limited
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
#include "gui/menu_item/menu_item.h"
#include "gui/ui/ui.h"
#include "hid/button.h"
#include "modulation/arpeggiator.h"

#define SHORTCUTS_VERSION_1 0
#define SHORTCUTS_VERSION_3 1
#define NUM_SHORTCUTS_VERSIONS 2

extern void enterEditor();
extern void enterSaveSynthPresetUI();

class Drum;
class Sound;
class Source;
class ParamManagerForTimeline;
class InstrumentClip;
class SideChain;
class Arpeggiator;
class MultisampleRange;
class Clip;
class SampleHolder;
class SampleControls;
class ModControllableAudio;
class ModelStackWithThreeMainThings;
class AudioFileHolder;
class MIDICable;
namespace deluge::gui::menu_item {
enum class RangeEdit : uint8_t;
}

class SoundEditor final : public UI {
public:
	SoundEditor();
	bool opened() override;
	void focusRegained() override;
	void displayOrLanguageChanged() final;
	bool getGreyoutColsAndRows(uint32_t* cols, uint32_t* rows) override;
	Sound* currentSound;
	ModControllableAudio* currentModControllable;
	int8_t currentSourceIndex;
	Source* currentSource;
	ParamManagerForTimeline* currentParamManager;
	SideChain* currentSidechain;
	ArpeggiatorSettings* currentArpSettings;
	::MultiRange* currentMultiRange;
	SampleControls* currentSampleControls;
	VoicePriority* currentPriority;
	int16_t currentMultiRangeIndex;
	MIDICable* currentMIDICable;
	deluge::gui::menu_item::RangeEdit editingRangeEdge;

	ActionResult buttonAction(deluge::hid::Button b, bool on, bool inCardRoutine) override;
	ActionResult padAction(int32_t x, int32_t y, int32_t velocity) override;
	ActionResult verticalEncoderAction(int32_t offset, bool inCardRoutine) override;
	void modEncoderAction(int32_t whichModEncoder, int32_t offset) override;
	void modEncoderButtonAction(uint8_t whichModEncoder, bool on) override;
	ActionResult horizontalEncoderAction(int32_t offset) override;
	void scrollFinished() override;
	bool editingKit();

	ActionResult timerCallback() override;
	void setupShortcutBlink(int32_t x, int32_t y, int32_t frequency);
	bool findPatchedParam(int32_t paramLookingFor, int32_t* xout, int32_t* yout);
	void updateSourceBlinks(MenuItem* currentItem);
	void resetSourceBlinks();

	uint8_t navigationDepth;
	uint8_t patchingParamSelected;
	uint8_t currentParamShorcutX;
	uint8_t currentParamShorcutY;
	uint8_t paramShortcutBlinkFrequency;
	uint32_t shortcutBlinkCounter;

	uint32_t timeLastAttemptedAutomatedParamEdit;

	int8_t numberScrollAmount;
	uint32_t numberEditSize;
	int8_t numberEditPos;

	uint8_t shortcutsVersion;

	MenuItem* menuItemNavigationRecord[16];

	bool shouldGoUpOneLevelOnBegin;

	bool programChangeReceived(MIDICable& cable, uint8_t channel, uint8_t program) { return false; }
	bool midiCCReceived(MIDICable& cable, uint8_t channel, uint8_t ccNumber, uint8_t value);
	bool pitchBendReceived(MIDICable& cable, uint8_t channel, uint8_t data1, uint8_t data2);
	void selectEncoderAction(int8_t offset) override;
	bool canSeeViewUnderneath() override { return true; }
	bool setup(Clip* clip = nullptr, const MenuItem* item = nullptr, int32_t sourceIndex = 0);
	void enterOrUpdateSoundEditor(bool on);
	void blinkShortcut();
	ActionResult potentialShortcutPadAction(int32_t x, int32_t y, bool on);
	bool editingReverbSidechain();
	MenuItem* getCurrentMenuItem();
	bool inSettingsMenu();
	bool setupKitGlobalFXMenu;
	bool exitUI() override {
		exitCompletely();
		return true;
	};
	void exitCompletely();
	void goUpOneLevel();
	void enterSubmenu(MenuItem* newItem);
	bool pcReceivedForMidiLearn(MIDICable& cable, int32_t channel, int32_t program) override;
	bool noteOnReceivedForMidiLearn(MIDICable& cable, int32_t channel, int32_t note, int32_t velocity) override;
	void markInstrumentAsEdited();
	bool editingCVOrMIDIClip();
	bool editingNonAudioDrumRow();
	bool editingMidiDrumRow();
	bool editingGateDrumRow();
	bool isUntransposedNoteWithinRange(int32_t noteCode);
	void setCurrentMultiRange(int32_t i);
	void possibleChangeToCurrentRangeDisplay();
	MenuPermission checkPermissionToBeginSessionForRangeSpecificParam(Sound* sound, int32_t whichThing,
	                                                                  ::MultiRange** previouslySelectedRange);
	void setupExclusiveShortcutBlink(int32_t x, int32_t y);
	void setShortcutsVersion(int32_t newVersion);
	ModelStackWithThreeMainThings* getCurrentModelStack(void* memory);

	void cutSound();
	AudioFileHolder* getCurrentAudioFileHolder();
	void mpeZonesPotentiallyUpdated();

	void renderOLED(deluge::hid::display::oled_canvas::Canvas& canvas) override;

	// ui
	UIType getUIType() override { return UIType::SOUND_EDITOR; }

	bool selectedNoteRow;

	// Note / Note Row Editor
	bool inNoteEditor();
	bool inNoteRowEditor();
	void toggleNoteEditorParamMenu(int32_t on);
	void updatePadLightsFor(MenuItem* item);

private:
	/// Setup shortcut blinking by finding the given menu item in the provided item map
	void setupShortcutsBlinkFromTable(MenuItem const* currentItem,
	                                  MenuItem const* const items[kDisplayWidth][kDisplayHeight]);
	bool beginScreen(MenuItem* oldMenuItem = nullptr);
	uint8_t getActualParamFromScreen(uint8_t screen);
	void setLedStates();
	ActionResult handleAutomationViewPadAction(int32_t x, int32_t y, int32_t velocity);
	bool isEditingAutomationViewParam();
	void handlePotentialParamMenuChange(deluge::hid::Button b, bool on, bool inCardRoutine, MenuItem* previousItem,
	                                    MenuItem* currentItem);
	bool handleClipName();

	uint8_t sourceShortcutBlinkFrequencies[2][kDisplayHeight];
	uint8_t sourceShortcutBlinkColours[2][kDisplayHeight];
};

extern SoundEditor soundEditor;
