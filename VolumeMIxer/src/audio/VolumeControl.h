#pragma once

#include <string>
#include <cstdio>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <vector>
#include <string>
#include "../utils/utils.h"

class VolumeControl
{
private:
	IMMDeviceEnumerator* m_deviceEnumerator;
	IMMDevice* m_device;
	IAudioSessionManager2* m_audioSessionManager2;

	utils util;

public:
	
	VolumeControl();

	void init();
	void destroy();

	void getAudioStreams(std::vector<std::wstring>&);
	
	void setVolume(const WCHAR*, const float*);
	void getVolume(const WCHAR*, float*);
	void toggleMute(const WCHAR*);
	void setMute(const WCHAR*, const BOOL*);
	void getMute(const WCHAR*, BOOL*);

};