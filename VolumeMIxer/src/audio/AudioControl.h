#pragma once

#include <string>
#include <cstdio>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <vector>

#include "../utils/utils.h"

class AudioControl
{
protected:
	IMMDeviceEnumerator* m_deviceEnumerator;
	IMMDevice* m_device;
	IAudioSessionManager2* m_audioSessionManager2;
	role m_eRole;
	dataFlow m_eDataFlow;

	utils util;

public:

	AudioControl();

	void init(EDataFlow, ERole);
	void destroy();

	void toggleMute(const std::wstring*);
	void setMute(const std::wstring*, const BOOL);
	void getMute(const std::wstring*, BOOL*);
	virtual void muteAll() = 0;
	virtual void unmuteAll() = 0;

};