#pragma once

#include <cstdio>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <vector>
#include <string>

#include "AudioControl.h"
#include "../utils/utils.h"

class InputControl : public AudioControl
{
public:

	InputControl();

	void getInputStreams(std::vector<std::wstring>&);

	void setBoost(const WCHAR*, const float*);
	void getBoost(const WCHAR*, float*);
};