#include "DeviceControl.h"

DeviceControl::DeviceControl()
{
	m_deviceEnumerator = NULL;
	m_device = NULL;
	m_audioSessionManager2 = NULL;
	m_eRole = eConsole;
	m_eDataFlow = eRender;
}

DeviceControl::DeviceControl(role i_eRole, dataFlow i_eDataFlow)
{
	DeviceControl();
	m_eRole = i_eRole;
	m_eDataFlow = i_eDataFlow;
}

void DeviceControl::init()
{
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_deviceEnumerator);
		if (SUCCEEDED(hr))
		{
			hr = m_deviceEnumerator->GetDefaultAudioEndpoint(m_eDataFlow, m_eRole, &m_device);
			if (SUCCEEDED(hr))
			{
				hr = m_device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (VOID**)&m_audioSessionManager2);
				if (SUCCEEDED(hr))
				{
					return;
				}
			}
		}
	}
}

void DeviceControl::destroy()
{
	// Release the resources
	SAFE_RELEASE(m_audioSessionManager2);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_deviceEnumerator);
	CoUninitialize();
}

void DeviceControl::getEndPointDeviceData(std::vector<EndPointData>& vecEndPoint)
{
	constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	HRESULT hr;

	IMMDeviceEnumerator* spEnumerator;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&spEnumerator);
	if (FAILED(hr))
		return;


	IMMDeviceCollection* spCollection;
	hr = spEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &spCollection);
	if (FAILED(hr))
		return;

	std::wstring defaultDevID;
	IMMDevice* spDefaultDevice;
	hr = spEnumerator->GetDefaultAudioEndpoint(m_eDataFlow, m_eRole, &spDefaultDevice);
	if (SUCCEEDED(hr))
	{
		LPWSTR id;
		hr = spDefaultDevice->GetId(&id);
		if (SUCCEEDED(hr))
		{
			defaultDevID = id;
			::CoTaskMemFree(id);
		}
	}

	UINT nCount = 0;
	spCollection->GetCount(&nCount);
	for (UINT i = 0; i < nCount; ++i)
	{
		IMMDevice* spDevice;
		hr = spCollection->Item(i, &spDevice);
		if (SUCCEEDED(hr))
		{
			if (spDevice)
			{
				EndPointData data;
				LPWSTR id;
				hr = spDevice->GetId(&id);
				if (SUCCEEDED(hr))
				{
					data.devID = id;
					::CoTaskMemFree(id);
					if (data.devID == defaultDevID)
						data.bDefault = true;
				}

				IPropertyStore* spProperty;
				spDevice->OpenPropertyStore(STGM_READ, &spProperty);
				if (spProperty)
				{
					PROPVARIANT	varName;
					PropVariantInit(&varName);
					hr = spProperty->GetValue(PKEY_Device_FriendlyName, &varName);
					if (SUCCEEDED(hr))
						data.name = varName.pwszVal;
					PropVariantClear(&varName);
				}
				vecEndPoint.push_back(data);
			}

		}
	}

	return;
}

void DeviceControl::setDefaultEndpoint(const std::wstring* i_devName, const role i_role)
{
	std::vector<EndPointData> spEndPoints;
	IPolicyConfig* spPolicyConfig = nullptr;
	LPCWSTR devId = NULL;

	HRESULT hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig), (LPVOID*)&spPolicyConfig);
	if (spPolicyConfig == nullptr) {
		hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig10), (LPVOID*)&spPolicyConfig);
	}
	if (spPolicyConfig == nullptr) {
		hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig7), (LPVOID*)&spPolicyConfig);
	}
	if (spPolicyConfig == nullptr) {
		hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID*)&spPolicyConfig);
	}
	if (spPolicyConfig == nullptr) {
		hr = CoCreateInstance(__uuidof(CPolicyConfigClient), NULL, CLSCTX_ALL, __uuidof(IPolicyConfig10_1), (LPVOID*)&spPolicyConfig);
	}

	getEndPointDeviceData(spEndPoints);

	const WCHAR* i_devName_c = i_devName->c_str();
	for (auto& elem : spEndPoints)
	{
		const WCHAR* elem_name_c = elem.name.c_str();

		if (!wcscmp(elem_name_c, i_devName_c))
		{
			std::wcout << L"Found endpoint " << elem_name_c << std::endl;
			devId = elem.devID.c_str();
			std::wcout << L"DevID: " << devId << std::endl;
			std::wcout << L"Elem ID: " << elem.devID << std::endl;
			break;
		}
	}
	std::wcout << L"DevID: " << devId << std::endl;

	if (spPolicyConfig != NULL) {
		if (devId != NULL)
		{
			std::cout << "Setting default endpoint" << std::endl;
			hr = spPolicyConfig->SetDefaultEndpoint(devId, i_role);
			SAFE_RELEASE(spPolicyConfig);
		}
	}

}

void DeviceControl::toggleDeviceMute(const std::wstring* i_devName)
{
	BOOL spMuteState;
	getDeviceMute(i_devName, &spMuteState);
	spMuteState = !spMuteState;
	setDeviceMute(i_devName, spMuteState);
}

void DeviceControl::getDeviceMute(const std::wstring* i_devName, BOOL* o_muteState)
{
	constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	HRESULT hr;

	IMMDeviceEnumerator* spEnumerator;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&spEnumerator);
	if (FAILED(hr))
		return;


	IMMDeviceCollection* spCollection;
	hr = spEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &spCollection);
	if (FAILED(hr))
		return;

	UINT nCount = 0;
	spCollection->GetCount(&nCount);
	for (UINT i = 0; i < nCount; ++i)
	{
		IMMDevice* spDevice;
		hr = spCollection->Item(i, &spDevice);
		if (SUCCEEDED(hr))
		{
			if (spDevice)
			{
				std::wstring l_devName;

				IPropertyStore* spProperty;
				spDevice->OpenPropertyStore(STGM_READ, &spProperty);
				if (spProperty)
				{
					PROPVARIANT	varName;
					PropVariantInit(&varName);
					hr = spProperty->GetValue(PKEY_Device_FriendlyName, &varName);
					if (SUCCEEDED(hr))
					{
						l_devName = varName.pwszVal;

						const WCHAR* ch_l_devName = l_devName.c_str();
						const WCHAR* ch_i_devName = i_devName->c_str();
						if (wcsstr(ch_l_devName, ch_i_devName) != NULL)
						{
							IAudioEndpointVolume* spAudioEndpoint;
							hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID*)&spAudioEndpoint);
							if (SUCCEEDED(hr))
							{
								hr = spAudioEndpoint->GetMute(o_muteState);
								if (FAILED(hr)) {
									printf("Failed to get mute state\n");
									SAFE_RELEASE(spAudioEndpoint);
									PropVariantClear(&varName);
									SAFE_RELEASE(spProperty);
									SAFE_RELEASE(spDevice);
									SAFE_RELEASE(spCollection);
									SAFE_RELEASE(spEnumerator);
									return;
								}
							}
							SAFE_RELEASE(spAudioEndpoint);

						}
					}
					PropVariantClear(&varName);
				}
				SAFE_RELEASE(spProperty);
			}

		}
		SAFE_RELEASE(spDevice);
	}

	SAFE_RELEASE(spCollection);
	SAFE_RELEASE(spEnumerator);
}

void DeviceControl::setDeviceMute(const std::wstring* i_devName, const BOOL i_muteState)
{
	constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	HRESULT hr;

	IMMDeviceEnumerator* spEnumerator;
	hr = CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&spEnumerator);
	if (FAILED(hr))
		return;


	IMMDeviceCollection* spCollection;
	hr = spEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &spCollection);
	if (FAILED(hr))
		return;

	UINT nCount = 0;
	spCollection->GetCount(&nCount);
	for (UINT i = 0; i < nCount; ++i)
	{
		IMMDevice* spDevice;
		hr = spCollection->Item(i, &spDevice);
		if (SUCCEEDED(hr))
		{
			if (spDevice)
			{
				std::wstring l_devName;

				IPropertyStore* spProperty;
				spDevice->OpenPropertyStore(STGM_READ, &spProperty);
				if (spProperty)
				{
					PROPVARIANT	varName;
					PropVariantInit(&varName);
					hr = spProperty->GetValue(PKEY_Device_FriendlyName, &varName);
					if (SUCCEEDED(hr))
					{
						l_devName = varName.pwszVal;

						const WCHAR* ch_l_devName = l_devName.c_str();
						const WCHAR* ch_i_devName = i_devName->c_str();
						if (wcsstr(ch_l_devName, ch_i_devName) != NULL)
						{
							IAudioEndpointVolume* spAudioEndpoint;
							hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID*)&spAudioEndpoint);
							if (SUCCEEDED(hr))
							{
								hr = spAudioEndpoint->SetMute(i_muteState, NULL);
								if (FAILED(hr)) {
									printf("Failed to set mute state\n");
									SAFE_RELEASE(spAudioEndpoint);
									PropVariantClear(&varName);
									SAFE_RELEASE(spProperty);
									SAFE_RELEASE(spDevice);
									SAFE_RELEASE(spCollection);
									SAFE_RELEASE(spEnumerator);
									return;
								}
							}
							SAFE_RELEASE(spAudioEndpoint);

						}
					}
					PropVariantClear(&varName);
				}
				SAFE_RELEASE(spProperty);
			}

		}
		SAFE_RELEASE(spDevice);
	}

	SAFE_RELEASE(spCollection);
	SAFE_RELEASE(spEnumerator);
}

void DeviceControl::getDeviceVolume(const std::wstring* i_devName, float* o_volumeLevel)
{
	{
		constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

		HRESULT hr;

		IMMDeviceEnumerator* spEnumerator;
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&spEnumerator);
		if (FAILED(hr))
			return;


		IMMDeviceCollection* spCollection;
		hr = spEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &spCollection);
		if (FAILED(hr))
			return;

		UINT nCount = 0;
		spCollection->GetCount(&nCount);
		for (UINT i = 0; i < nCount; ++i)
		{
			IMMDevice* spDevice;
			hr = spCollection->Item(i, &spDevice);
			if (SUCCEEDED(hr))
			{
				if (spDevice)
				{
					std::wstring l_devName;

					IPropertyStore* spProperty;
					spDevice->OpenPropertyStore(STGM_READ, &spProperty);
					if (spProperty)
					{
						PROPVARIANT	varName;
						PropVariantInit(&varName);
						hr = spProperty->GetValue(PKEY_Device_FriendlyName, &varName);
						if (SUCCEEDED(hr))
						{
							l_devName = varName.pwszVal;

							const WCHAR* ch_l_devName = l_devName.c_str();
							const WCHAR* ch_i_devName = i_devName->c_str();
							if (wcsstr(ch_l_devName, ch_i_devName) != NULL)
							{
								IAudioEndpointVolume* spAudioEndpoint;
								hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID*)&spAudioEndpoint);
								if (SUCCEEDED(hr))
								{
									hr = spAudioEndpoint->GetMasterVolumeLevelScalar(o_volumeLevel);
									if (FAILED(hr)) {
										printf("Failed to set mute state\n");
										SAFE_RELEASE(spAudioEndpoint);
										PropVariantClear(&varName);
										SAFE_RELEASE(spProperty);
										SAFE_RELEASE(spDevice);
										SAFE_RELEASE(spCollection);
										SAFE_RELEASE(spEnumerator);
										return;
									}
								}
								SAFE_RELEASE(spAudioEndpoint);
							}
						}
						PropVariantClear(&varName);
					}
					SAFE_RELEASE(spProperty);
				}

			}
			SAFE_RELEASE(spDevice);
		}

		SAFE_RELEASE(spCollection);
		SAFE_RELEASE(spEnumerator);
	}
}

void DeviceControl::setDeviceVolume(const std::wstring* i_devName, const float i_volumeLevel)
{
	{
		constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
		constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

		HRESULT hr;

		IMMDeviceEnumerator* spEnumerator;
		hr = CoCreateInstance(
			CLSID_MMDeviceEnumerator, NULL,
			CLSCTX_ALL, IID_IMMDeviceEnumerator,
			(void**)&spEnumerator);
		if (FAILED(hr))
			return;


		IMMDeviceCollection* spCollection;
		hr = spEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &spCollection);
		if (FAILED(hr))
			return;

		UINT nCount = 0;
		spCollection->GetCount(&nCount);
		for (UINT i = 0; i < nCount; ++i)
		{
			IMMDevice* spDevice;
			hr = spCollection->Item(i, &spDevice);
			if (SUCCEEDED(hr))
			{
				if (spDevice)
				{
					std::wstring l_devName;

					IPropertyStore* spProperty;
					spDevice->OpenPropertyStore(STGM_READ, &spProperty);
					if (spProperty)
					{
						PROPVARIANT	varName;
						PropVariantInit(&varName);
						hr = spProperty->GetValue(PKEY_Device_FriendlyName, &varName);
						if (SUCCEEDED(hr))
						{
							l_devName = varName.pwszVal;

							const WCHAR* ch_l_devName = l_devName.c_str();
							const WCHAR* ch_i_devName = i_devName->c_str();
							if (wcsstr(ch_l_devName, ch_i_devName) != NULL)
							{
								IAudioEndpointVolume* spAudioEndpoint;
								hr = spDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID*)&spAudioEndpoint);
								if (SUCCEEDED(hr))
								{
									hr = spAudioEndpoint->SetMasterVolumeLevelScalar(i_volumeLevel, NULL);
									if (FAILED(hr)) {
										printf("Failed to set mute state\n");
										SAFE_RELEASE(spAudioEndpoint);
										PropVariantClear(&varName);
										SAFE_RELEASE(spProperty);
										SAFE_RELEASE(spDevice);
										SAFE_RELEASE(spCollection);
										SAFE_RELEASE(spEnumerator);
										return;
									}
								}
								SAFE_RELEASE(spAudioEndpoint);

							}
						}
						PropVariantClear(&varName);
					}
					SAFE_RELEASE(spProperty);
				}

			}
			SAFE_RELEASE(spDevice);
		}

		SAFE_RELEASE(spCollection);
		SAFE_RELEASE(spEnumerator);
	}
}
