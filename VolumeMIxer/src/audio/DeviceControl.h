#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

#include "../utils/utils.h"
#include "../utils/EndPointData.h"


class DeviceControl
{
private:
	IMMDeviceEnumerator* m_deviceEnumerator;
	IMMDevice* m_device;
	IAudioSessionManager2* m_audioSessionManager2;
	role m_eRole;
	dataFlow m_eDataFlow;

	utils util;

public:
	DeviceControl();
	DeviceControl(role i_eRole, dataFlow i_eDataFlow);

	void init();
	void destroy();

	// Thank you to amate for how to get the endpoints
	//  https://github.com/amate/SetDefaultAudioDevice
	void GetEndPointDeviceData(std::vector<EndPointData>& vecEndPoint);
	
	void setDefaultEndpoint(const std::wstring* i_devName);
	
	void toggleDeviceMute(const std::wstring* i_devName);
	void getDeviceMute(const std::wstring* i_devName, BOOL* o_muteState);
	void setDeviceMute(const std::wstring* i_devName, const BOOL* i_muteState);

	void getDeviceVolume(const std::wstring* i_devName, float *o_volumeLevel);
	void setDeviceVolume(const std::wstring* i_devName, const float* i_volumeLevel);

};

