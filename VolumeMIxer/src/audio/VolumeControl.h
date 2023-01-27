#pragma once

#include "AudioControl.h"

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