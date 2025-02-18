/*
 * Copyright © 2016-2023 Synthstrom Audible Limited
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

#include "io/midi/learned_midi.h"
#include "definitions_cxx.hpp"
#include "io/midi/midi_device.h"
#include "storage/storage_manager.h"
#include <string.h>

LearnedMIDI::LearnedMIDI() {
	clear();
}
MIDIMatchType LearnedMIDI::checkMatch(MIDICable* newCable, int32_t midiChannel) {
	uint8_t corz = newCable->ports[MIDI_DIRECTION_INPUT_TO_DELUGE].channelToZone(midiChannel);

	if (equalsCable(newCable) && channelOrZone == corz) {
		if (channelOrZone == midiChannel) {
			return MIDIMatchType::CHANNEL;
		}
		bool master = newCable->ports[MIDI_DIRECTION_INPUT_TO_DELUGE].isMasterChannel(midiChannel);
		if (master) {
			return MIDIMatchType::MPE_MASTER;
		}
		else {
			return MIDIMatchType::MPE_MEMBER;
		}
	}
	return MIDIMatchType::NO_MATCH;
}

void LearnedMIDI::clear() {
	cable = nullptr;
	channelOrZone = MIDI_CHANNEL_NONE;
	noteOrCC = 255;
}

char const* getTagNameFromMIDIMessageType(int32_t midiMessageType) {
	switch (midiMessageType) {
	case MIDI_MESSAGE_NOTE:
		return "note";

	case MIDI_MESSAGE_CC:
		return "ccNumber";

	default:
		__builtin_unreachable();
		return NULL;
	}
}

// If you're calling this direcly instead of calling writeToFile(), you'll need to check and possibly write a new tag
// for device - that can't be just an attribute. You should be sure that containsSomething() == true before calling
// this.
void LearnedMIDI::writeAttributesToFile(Serializer& writer, int32_t midiMessageType) {

	if (isForMPEZone()) {
		char const* zoneText = (channelOrZone == MIDI_CHANNEL_MPE_LOWER_ZONE) ? "lower" : "upper";
		writer.writeAttribute("mpeZone", zoneText, false);
	}
	else {
		writer.writeAttribute("channel", channelOrZone, false);
	}

	if (midiMessageType != MIDI_MESSAGE_NONE) {
		char const* messageTypeName = getTagNameFromMIDIMessageType(midiMessageType);

		writer.writeAttribute(messageTypeName, noteOrCC, false);
	}
}

void LearnedMIDI::writeToFile(Serializer& writer, char const* commandName, int32_t midiMessageType) {
	if (!containsSomething()) {
		return;
	}

	writer.writeOpeningTagBeginning(commandName);
	writeAttributesToFile(writer, midiMessageType);

	if (cable) {
		writer.writeOpeningTagEnd();
		cable->writeReferenceToFile(writer);
		writer.writeClosingTag(commandName);
	}
	else {
		writer.closeTag();
	}
}

void LearnedMIDI::readFromFile(Deserializer& reader, int32_t midiMessageType) {

	char const* tagName;
	while (*(tagName = reader.readNextTagOrAttributeName())) {
		if (!strcmp(tagName, "channel")) {
			channelOrZone = reader.readTagOrAttributeValueInt();
		}
		else if (!strcmp(tagName, "mpeZone")) {
			readMPEZone(reader);
		}
		else if (!strcmp(tagName, "device")) {
			cable = MIDIDeviceManager::readDeviceReferenceFromFile(reader);
		}
		else if (midiMessageType != MIDI_MESSAGE_NONE
		         && !strcmp(tagName, getTagNameFromMIDIMessageType(midiMessageType))) {
			noteOrCC = reader.readTagOrAttributeValueInt();
			noteOrCC = std::min<int32_t>(noteOrCC, 127);
		}
		reader.exitTag();
	}
}

void LearnedMIDI::readMPEZone(Deserializer& reader) {
	char const* text = reader.readTagOrAttributeValue();
	if (!strcmp(text, "lower")) {
		channelOrZone = MIDI_CHANNEL_MPE_LOWER_ZONE;
	}
	else if (!strcmp(text, "upper")) {
		channelOrZone = MIDI_CHANNEL_MPE_UPPER_ZONE;
	}
}

bool LearnedMIDI::equalsChannelAllowMPE(MIDICable* newCable, int32_t newChannel) {
	if (channelOrZone == MIDI_CHANNEL_NONE) {
		return false; // 99% of the time, we'll get out here, because input isn't activated/learned.
	}
	if (!equalsCable(newCable)) {
		return false;
	}
	if (!cable) {
		return false; // Could we actually be set to MPE but have no device? Maybe if loaded from weird song file?
	}
	int32_t newCorZ = newCable->ports[MIDI_DIRECTION_INPUT_TO_DELUGE].channelToZone(newChannel);

	return (channelOrZone == newCorZ);
}

bool LearnedMIDI::equalsChannelAllowMPEMasterChannels(MIDICable* newCable, int32_t newChannel) {
	if (channelOrZone == MIDI_CHANNEL_NONE) {
		return false; // 99% of the time, we'll get out here, because input isn't activated/learned.
	}
	if (!equalsCable(newCable)) {
		return false;
	}
	if (channelOrZone < 16) {
		return (channelOrZone == newChannel);
	}
	if (channelOrZone > IS_A_CC) {
		return (channelOrZone == newChannel);
	}

	if (channelOrZone == MIDI_CHANNEL_MPE_LOWER_ZONE) {
		if (newChannel <= newCable->ports[MIDI_DIRECTION_INPUT_TO_DELUGE].mpeLowerZoneLastMemberChannel) {
			return (newChannel == getMasterChannel());
		}
	}
	if (channelOrZone == MIDI_CHANNEL_MPE_UPPER_ZONE) {
		if (newChannel >= newCable->ports[MIDI_DIRECTION_INPUT_TO_DELUGE].mpeUpperZoneLastMemberChannel) {
			return (newChannel == getMasterChannel());
		}
	}
	return false; // should never happen
}
