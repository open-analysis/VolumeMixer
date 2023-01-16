#pragma once

#include <string>
#include <cstdio>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <vector>

#include <iostream>

#include "../utils/utils.h"

class AudioControl
{
protected:
	IMMDeviceEnumerator* m_deviceEnumerator;
	IMMDevice* m_device;
	IAudioSessionManager2* m_audioSessionManager2;

	utils util;

public:

	AudioControl();

	void init(EDataFlow, ERole);
	void destroy();

	void getDevices();

	void toggleMute(const WCHAR*);
	void setMute(const WCHAR*, const BOOL*);
	void getMute(const WCHAR*, BOOL*);

};