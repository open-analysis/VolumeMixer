#include "AudioControl.h"

AudioControl::AudioControl()
{
	m_deviceEnumerator = NULL;
	m_device = NULL;
	m_audioSessionManager2 = NULL;
	m_eDataFlow = eRender;
	m_eRole = eConsole;
	m_img = "";
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
	SAFE_RELEASE(m_audioSessionManager2);
	SAFE_RELEASE(m_device);
	SAFE_RELEASE(m_deviceEnumerator);
	CoUninitialize();
}

void AudioControl::getStreams(std::vector<std::wstring>& o_streams)
{
	IAudioSessionEnumerator* audioSessionEnumerator;
	IAudioSessionControl* audioSessionControl;
	IAudioSessionControl2* audioSessionControl2;
	HRESULT hr;

	std::wstring progName;
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
								//printf("\nImage: %ls\n", wsImageName);
								progName = util.extractName(wsImageName);
								//std::wcout << "Prog: " << progName << std::endl;
								o_streams.push_back(progName);
							}
							CloseHandle(hProcess);
						}
					}
					SAFE_RELEASE(audioSessionControl2);
				}
			}
			SAFE_RELEASE(audioSessionControl);
		}
		SAFE_RELEASE(audioSessionEnumerator);
	}
}

void AudioControl::setVolume(const std::wstring* i_name, const float i_level)
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
										simpleAudioVolume->SetMasterVolume(i_level, NULL);
										SAFE_RELEASE(simpleAudioVolume);
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					SAFE_RELEASE(audioSessionControl2);
				}
			}
			SAFE_RELEASE(audioSessionControl);
		}
		SAFE_RELEASE(audioSessionEnumerator);
	}
}

void AudioControl::getVolume(const std::wstring* i_name, float* o_level)
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
										simpleAudioVolume->GetMasterVolume(o_level);
										SAFE_RELEASE(simpleAudioVolume);
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					SAFE_RELEASE(audioSessionControl2);
				}
			}
			SAFE_RELEASE(audioSessionControl);
		}
		SAFE_RELEASE(audioSessionEnumerator);
	}
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
										SAFE_RELEASE(simpleAudioVolume);
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					SAFE_RELEASE(audioSessionControl2);
				}
			}
			SAFE_RELEASE(audioSessionControl);
		}
		SAFE_RELEASE(audioSessionEnumerator);
	}
}

void AudioControl::setMute(const std::wstring* i_name, const BOOL i_muteState)
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
										hr = simpleAudioVolume->SetMute(i_muteState, NULL);
										SAFE_RELEASE(simpleAudioVolume);
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					SAFE_RELEASE(audioSessionControl2);
				}
			}
			SAFE_RELEASE(audioSessionControl);
		}
		SAFE_RELEASE(audioSessionEnumerator);
	}
}

void AudioControl::getMute(const std::wstring* i_name, BOOL* o_muteState)
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
										hr = simpleAudioVolume->GetMute(o_muteState);
										SAFE_RELEASE(simpleAudioVolume);
									}
								}
							}
							CloseHandle(hProcess);
						}
					}
					SAFE_RELEASE(audioSessionControl2);
				}
			}
			SAFE_RELEASE(audioSessionControl);
		}
		SAFE_RELEASE(audioSessionEnumerator);
	}
}

void AudioControl::muteAll()
{
	constexpr BOOL MUTE = true;
	std::vector<std::wstring> inputs;

	getStreams(inputs);

	for (auto prog : inputs)
	{
		AudioControl::setMute(&prog, MUTE);
	}

	inputs.clear();
}

void AudioControl::unmuteAll()
{
	constexpr BOOL MUTE = false;
	std::vector<std::wstring> inputs;

	getStreams(inputs);

	for (auto prog : inputs)
	{
		AudioControl::setMute(&prog, MUTE);
	}

	inputs.clear();
}
