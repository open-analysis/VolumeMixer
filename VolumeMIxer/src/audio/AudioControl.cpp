#include "AudioControl.h"

AudioControl::AudioControl()
{
	m_deviceEnumerator = NULL;
	m_device = NULL;
	m_audioSessionManager2 = NULL;
	m_eDataFlow = eRender;
	m_eRole = eConsole;
}

void AudioControl::init(EDataFlow dataFlow, ERole role)
{
	m_eDataFlow = dataFlow;
	m_eRole = role;

	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_deviceEnumerator);
		if (SUCCEEDED(hr))
		{
			hr = m_deviceEnumerator->GetDefaultAudioEndpoint(dataFlow, role, &m_device);
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

void AudioControl::destroy()
{
	// Release the resources
	m_audioSessionManager2->Release();
	m_device->Release();
	m_deviceEnumerator->Release();
	CoUninitialize();
}

bool AudioControl::GetEndPointDeviceData(std::vector<EndPointData>& vecEndPoint)
{
	constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
	constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

	IMMDeviceEnumerator* spEnumerator;
	if (FAILED(CoCreateInstance(
		CLSID_MMDeviceEnumerator, NULL,
		CLSCTX_ALL, IID_IMMDeviceEnumerator,
		(void**)&spEnumerator)))
		return false;


	IMMDeviceCollection* spCollection;
	if (FAILED(spEnumerator->EnumAudioEndpoints(m_eDataFlow, DEVICE_STATE_ACTIVE, &spCollection)))
		return false;

	std::wstring defaultDevID;
	IMMDevice* spDefaultDevice;
	if (SUCCEEDED(spEnumerator->GetDefaultAudioEndpoint(m_eDataFlow, m_eRole, &spDefaultDevice))) {
		LPWSTR id;
		if (SUCCEEDED(spDefaultDevice->GetId(&id))) {
			defaultDevID = id;
			::CoTaskMemFree(id);
		}
	}

	UINT nCount = 0;
	spCollection->GetCount(&nCount);
	for (UINT i = 0; i < nCount; ++i) {
		IMMDevice* spDevice;
		spCollection->Item(i, &spDevice);
		if (spDevice) {
			EndPointData	data;
			LPWSTR id;
			if (SUCCEEDED(spDevice->GetId(&id))) {
				data.devID = id;
				::CoTaskMemFree(id);
				if (data.devID == defaultDevID)
					data.bDefault = true;
			}

			IPropertyStore* spProperty;
			spDevice->OpenPropertyStore(STGM_READ, &spProperty);
			if (spProperty) {
				PROPVARIANT	varName;
				PropVariantInit(&varName);
				if (SUCCEEDED(spProperty->GetValue(PKEY_Device_FriendlyName, &varName)))
					data.name = varName.pwszVal;
				PropVariantClear(&varName);
			}
			vecEndPoint.push_back(data);
		}
	}

	return true;
}

void AudioControl::toggleMute(const std::wstring* i_name)
{
	IAudioSessionEnumerator* audioSessionEnumerator;
	IAudioSessionControl* audioSessionControl;
	IAudioSessionControl2* audioSessionControl2;
	HRESULT hr;

	hr = m_audioSessionManager2->GetSessionEnumerator(&audioSessionEnumerator);
	if (SUCCEEDED(hr))
	{
		int nSessionCount;
		hr = audioSessionEnumerator->GetCount(&nSessionCount);
		for (int n = 0; n < nSessionCount; n++)
		{
			hr = audioSessionEnumerator->GetSession(n, &audioSessionControl);
			if (SUCCEEDED(hr))
			{
				hr = audioSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&audioSessionControl2);
				if (SUCCEEDED(hr))
				{
					DWORD nPID = 0;
					hr = audioSessionControl2->GetProcessId(&nPID);
					if (SUCCEEDED(hr))
					{
						HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, nPID);
						if (hProcess != NULL)
						{
							WCHAR wsImageName[MAX_PATH + 1];
							DWORD nSize = MAX_PATH;
							if (QueryFullProcessImageNameW(hProcess, NULL, wsImageName, &nSize))
							{
								const WCHAR* TEMP_NAME = i_name->c_str();
								//   v gets the substring
								if (wcsstr(wsImageName, TEMP_NAME) != NULL)
								{
									ISimpleAudioVolume* simpleAudioVolume;
									hr = audioSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume);
									if (SUCCEEDED(hr))
									{
										BOOL muted;
										hr = simpleAudioVolume->GetMute(&muted);
										hr = simpleAudioVolume->SetMute(!muted, NULL);
										simpleAudioVolume->Release();
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					audioSessionControl2->Release();
				}
			}
			audioSessionControl->Release();
		}
		audioSessionEnumerator->Release();
	}
}

void AudioControl::setMute(const std::wstring* i_name, const BOOL* i_mute)
{
	IAudioSessionEnumerator* audioSessionEnumerator;
	IAudioSessionControl* audioSessionControl;
	IAudioSessionControl2* audioSessionControl2;
	HRESULT hr;

	hr = m_audioSessionManager2->GetSessionEnumerator(&audioSessionEnumerator);
	if (SUCCEEDED(hr))
	{
		int nSessionCount;
		hr = audioSessionEnumerator->GetCount(&nSessionCount);
		for (int n = 0; n < nSessionCount; n++)
		{
			hr = audioSessionEnumerator->GetSession(n, &audioSessionControl);
			if (SUCCEEDED(hr))
			{
				hr = audioSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&audioSessionControl2);
				if (SUCCEEDED(hr))
				{
					DWORD nPID = 0;
					hr = audioSessionControl2->GetProcessId(&nPID);
					if (SUCCEEDED(hr))
					{
						HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, nPID);
						if (hProcess != NULL)
						{
							WCHAR wsImageName[MAX_PATH + 1];
							DWORD nSize = MAX_PATH;
							if (QueryFullProcessImageNameW(hProcess, NULL, wsImageName, &nSize))
							{
								const WCHAR* TEMP_NAME = i_name->c_str();
								//   v gets the substring
								if (wcsstr(wsImageName, TEMP_NAME) != NULL)
								{
									ISimpleAudioVolume* simpleAudioVolume;
									hr = audioSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume);
									if (SUCCEEDED(hr))
									{
										hr = simpleAudioVolume->SetMute(*i_mute, NULL);
										simpleAudioVolume->Release();
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					audioSessionControl2->Release();
				}
			}
			audioSessionControl->Release();
		}
		audioSessionEnumerator->Release();
	}
}

void AudioControl::getMute(const std::wstring* i_name, BOOL* o_mute)
{
	IAudioSessionEnumerator* audioSessionEnumerator;
	IAudioSessionControl* audioSessionControl;
	IAudioSessionControl2* audioSessionControl2;
	HRESULT hr;
	hr = m_audioSessionManager2->GetSessionEnumerator(&audioSessionEnumerator);
	if (SUCCEEDED(hr))
	{
		int nSessionCount;
		hr = audioSessionEnumerator->GetCount(&nSessionCount);
		for (int n = 0; n < nSessionCount; n++)
		{
			hr = audioSessionEnumerator->GetSession(n, &audioSessionControl);
			if (SUCCEEDED(hr))
			{
				hr = audioSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&audioSessionControl2);
				if (SUCCEEDED(hr))
				{
					DWORD nPID = 0;
					hr = audioSessionControl2->GetProcessId(&nPID);
					if (SUCCEEDED(hr))
					{
						HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, nPID);
						if (hProcess != NULL)
						{
							WCHAR wsImageName[MAX_PATH + 1];
							DWORD nSize = MAX_PATH;
							if (QueryFullProcessImageNameW(hProcess, NULL, wsImageName, &nSize))
							{
								const WCHAR* TEMP_NAME = i_name->c_str();
								if (wcsstr(wsImageName, TEMP_NAME) != NULL)
								{
									ISimpleAudioVolume* simpleAudioVolume;
									hr = audioSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume);
									if (SUCCEEDED(hr))
									{
										hr = simpleAudioVolume->GetMute(o_mute);
										simpleAudioVolume->Release();
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					audioSessionControl2->Release();
				}
			}
			audioSessionControl->Release();
		}
		audioSessionEnumerator->Release();
	}
}
