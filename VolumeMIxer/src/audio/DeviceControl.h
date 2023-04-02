#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include <audioclient.h>
#include <audiopolicy.h>
#include <Mmdeviceapi.h>
#include <endpointvolume.h>
#include <functiondiscoverykeys_devpkey.h>

#include "AudioControl.h"
#include "../utils/EndPointData.h"
#include "../utils/PolicyConfig.h"

class DeviceControl : public AudioControl
{
public:
	DeviceControl();
	DeviceControl(role i_eRole, dataFlow i_eDataFlow);

	// Thank you to amate for how to get the endpoints
	//  https://github.com/amate/SetDefaultAudioDevice
	void getEndPointDeviceData(std::vector<EndPointData>& vecEndPoint);
	
	// Thanks to EreTIk & amate for figuring this out
	void setDefaultEndpoint(const std::wstring* i_devName, const role i_role);

	void toggleMute(const std::wstring* i_devName);
	void getMute(const std::wstring* i_devName, BOOL* o_muteState);
	void setMute(const std::wstring* i_devName, const BOOL i_muteState);

	void getVolume(const std::wstring* i_devName, float *o_volumeLevel);
	void setVolume(const std::wstring* i_devName, const float i_volumeLevel);
};

