#include "InputControl.h"

InputControl::InputControl()
{
	m_deviceEnumerator = NULL;
	m_device = NULL;
	m_audioSessionManager2 = NULL;
}


void InputControl::init()
{
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_deviceEnumerator);
		if (SUCCEEDED(hr))
		{
			hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eCommunications, &m_device);
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

void InputControl::destroy()
{
	// Release the resources
	m_audioSessionManager2->Release();
	m_device->Release();
	m_deviceEnumerator->Release();
	CoUninitialize();
}

void InputControl::getInputStreams(std::vector<std::wstring>& o_streams)
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
					audioSessionControl2->Release();
				}
			}
			audioSessionControl->Release();
		}
		audioSessionEnumerator->Release();
	}
}

void InputControl::setBoost(const WCHAR* i_name, const float* i_volume)
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
								//   v gets the substring
								if (wcsstr(wsImageName, i_name) != NULL)
								{
									ISimpleAudioVolume* simpleAudioVolume;
									hr = audioSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume);
									if (SUCCEEDED(hr))
									{
										simpleAudioVolume->SetMasterVolume(*i_volume, NULL);
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

void InputControl::getBoost(const WCHAR* i_name, float* o_level)
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
								//   v gets the substring
								if (wcsstr(wsImageName, i_name) != NULL)
								{
									ISimpleAudioVolume* simpleAudioVolume;
									hr = audioSessionControl2->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&simpleAudioVolume);
									if (SUCCEEDED(hr))
									{
										simpleAudioVolume->GetMasterVolume(o_level);
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

void InputControl::toggleMute(const WCHAR* i_name)
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
								//   v gets the substring
								if (wcsstr(wsImageName, i_name) != NULL)
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

void InputControl::setMute(const WCHAR* i_name, const BOOL* i_mute)
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
								//   v gets the substring
								if (wcsstr(wsImageName, i_name) != NULL)
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

void InputControl::getMute(const WCHAR* i_name, BOOL* o_mute)
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
								if (wcsstr(wsImageName, i_name) != NULL)
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
