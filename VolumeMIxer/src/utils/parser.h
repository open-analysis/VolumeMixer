#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <codecvt>
#include <locale>

#include "../audio/DeviceControl.h"
#include "../audio/InputControl.h"
#include "../audio/VolumeControl.h"

class Parser
{
private:
	VolumeControl* m_volCntl;
	InputControl* m_inCntl;
	DeviceControl* m_devCntl;

public:
	Parser();
	Parser(VolumeControl* i_volCntl, InputControl* i_inCntl, DeviceControl* i_devCntl);

	void parseQueue(char* i_buffer, std::vector<std::wstring>& i_outputs, std::vector<std::wstring>& i_inputs, std::vector<std::wstring>& i_devices);

private:
	inline std::wstring convert(std::string i_str);
	void removeCharsFromString(std::string& str, char* charsToRemove);
};