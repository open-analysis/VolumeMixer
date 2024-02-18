#pragma once

#include <iostream>
#include <vector>

#include "../audio/AudioControl.h"
#include "../audio/DeviceControl.h"
#include "AudioStructs.h"
#include "Utils.h"

class Parser
{
private:
	AudioControl* m_audioCntl;
	DeviceControl* m_devCntl;
	Utils m_util;

	std::vector<std::string> m_queue;

	std::string m_JSON_SECTIONS[10] = {
		"{",				// 0
		"\"type\": ",		// 1
		"\"in\", ",			// 2
		"\"out\", ",		// 3
		"\"name\": ",		// 4
		"\"img\": ",		// 5
		"\"default\": ",	// 6
		"\"volume\": ",		// 7
		"\"mute\": ",		// 8
		"}"					// 9
	};

	char c_lf_char;

public:
	Parser();
	Parser(AudioControl* i_audioCntl, DeviceControl* i_devCntl);

	void parseQueue(const std::string i_direction, const std::string i_type);

	void setQueue(char* i_buffer);
	void flushQueue() { m_queue.clear(); }

	std::string device2Json(const AudioDevice& i_audio_device);
	std::string program2Json(const Audio& i_audio);

	void setAudioCntl(AudioControl* i_audioCntl)
	{
		m_audioCntl = i_audioCntl;
	}

	void setDeviceCntl(DeviceControl* i_devCntl)
	{
		m_devCntl = i_devCntl;
	}

private:
	void parseCmdDevice(std::vector<std::string> i_cmds);
	void parseCmdPrograms(std::vector<std::string> i_cmds);

	void removeCharsFromString(std::string& str, char* charsToRemove);
};