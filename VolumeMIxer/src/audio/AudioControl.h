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
	
	/*
		Sample input:
			init(eCapture, eCommunications);
		Sample output
			init(eRender, eConsole);
	*/
	void init(EDataFlow, ERole);
	void destroy();

	void getStreams(std::vector<std::wstring>& o_streams);

	void setVolume(const std::wstring* i_name, const float i_level);
	void getVolume(const std::wstring* i_name, float* o_level);

	void toggleMute(const std::wstring* i_name);
	void setMute(const std::wstring* i_name, const BOOL i_muteState);
	void getMute(const std::wstring* i_name, BOOL* o_muteState);
	void muteAll();
	void unmuteAll();

};