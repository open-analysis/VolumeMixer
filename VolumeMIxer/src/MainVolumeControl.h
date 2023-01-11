#pragma once

#include <cstdio>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <vector>

class MainVolumeControl
{
private:
	IMMDeviceEnumerator* m_deviceEnumerator;
	IMMDevice* m_device;
	IAudioSessionManager2* m_audioSessionManager2;

public:
	
	MainVolumeControl();

	void init();
	void destroy();

	void getAudioStreams(std::vector<WCHAR*>*);
	
	void setVolume(const WCHAR*, const float*);
	void getVolume(const WCHAR*, float*);
	void toggleMute(const WCHAR*);
	void getMute(const WCHAR*, BOOL*);

	void check(HRESULT);
};