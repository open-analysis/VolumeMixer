#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <codecvt>
#include <locale>

#include "../audio/AudioControl.h"
#include "../audio/DeviceControl.h"

class Parser
{
private:
	AudioControl* m_audioCntl;
	DeviceControl* m_devCntl;

public:
	Parser();
	Parser(AudioControl* i_audioCntl, DeviceControl* i_devCntl);

	void parseQueue(char* i_buffer);

	void setAudioCntl(AudioControl* i_audioCntl)
	{
		m_audioCntl = i_audioCntl;
	}

private:
	inline std::wstring convert(std::string i_str);
	void removeCharsFromString(std::string& str, char* charsToRemove);
};