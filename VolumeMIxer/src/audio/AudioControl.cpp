#include "AudioControl.h"

AudioControl::AudioControl()
{
	m_deviceEnumerator = NULL;
	m_device = NULL;
	m_audioSessionManager2 = NULL;
}

void AudioControl::init(EDataFlow dataFlow, ERole role)
{
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_deviceEnumerator);
		if (SUCCEEDED(hr))
		{
			hr = m_deviceEnumerator->GetDefaultAudioEndpoint(dataFlow, role, &m_device);
			std::wcout << "Device: " << m_device << std::endl;
			std::wcout << "Type: " << typeid(m_device).name() << std::endl;
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


void AudioControl::getDevices()
{
	IMMDeviceEnumerator* deviceEnumerator = NULL;
	IMMDeviceCollection* deviceCollection = NULL;

	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		std::cout << "Initialized" << std::endl;
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&deviceEnumerator);
		if (SUCCEEDED(hr))
		{
			std::cout << "Created instance" << std::endl;
			hr = deviceEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, (IMMDeviceCollection**)deviceCollection);
			if (SUCCEEDED(hr))
			{
				if (deviceCollection != NULL)
				{
					std::cout << "Enumerated devices" << std::endl;
					std::wcout << "Devices: " << deviceCollection << std::endl;
					std::wcout << "Type: " << typeid(deviceCollection).name() << std::endl;
				}
			}
			std::cout << "Done enumerating devices" << std::endl;
		}
	}
	deviceCollection->Release();
	deviceEnumerator->Release();
	CoUninitialize();

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
