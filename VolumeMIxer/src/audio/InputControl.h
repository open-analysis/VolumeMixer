#pragma once

#include <string>
#include <cstdio>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <vector>

#include "../utils/utils.h"

class InputControl
{
private:
	IMMDeviceEnumerator* m_deviceEnumerator;
	IMMDevice* m_device;
	IAudioSessionManager2* m_audioSessionManager2;

	utils util;

public:

	InputControl();

	void init();
	void destroy();

	void getInputStreams(std::vector<std::wstring>&);

	void setBoost(const WCHAR*, const float*);
	void getBoost(const WCHAR*, float*);
	void toggleMute(const WCHAR*);
	void setMute(const WCHAR*, const BOOL*);
	void getMute(const WCHAR*, BOOL*);

};