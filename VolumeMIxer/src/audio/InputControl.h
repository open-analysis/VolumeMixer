#pragma once

#include "AudioControl.h"

class InputControl : public AudioControl
{
public:

	InputControl();

	void getInputStreams(std::vector<std::wstring>&);

	void setBoost(const std::wstring*, const float);
	void getBoost(const std::wstring*, float*);

	void muteAll();
	void unmuteAll();
};