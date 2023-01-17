#pragma once

#include <string>
#include <cstdio>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <vector>
#include <string>

#include "AudioControl.h"
#include "../utils/utils.h"

class VolumeControl : public AudioControl
{
public:
	
	VolumeControl();

	void getAudioStreams(std::vector<std::wstring>&);
	
	void setVolume(const std::wstring*, const float*);
	void getVolume(const std::wstring*, float*);

	void muteAll();
	void unmuteAll();
};