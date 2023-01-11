#include "MainVolumeControl.h"

MainVolumeControl::MainVolumeControl()
{
	m_deviceEnumerator = NULL;
	m_device = NULL;
	m_audioSessionManager2 = NULL;
}


void MainVolumeControl::init()
{
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&m_deviceEnumerator);
		if (SUCCEEDED(hr))
		{
			hr = m_deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_device);
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

void MainVolumeControl::setVolume(const WCHAR* i_name, const float* i_volume)
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

void MainVolumeControl::getVolume(const WCHAR* i_name, float* o_level)
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

void MainVolumeControl::toggleMute(const WCHAR* i_name)
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
		printf("Cycling process names %d times\n", nSessionCount);
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
							printf("\t\tGot process ID %d\n", nPID);
							WCHAR wsImageName[MAX_PATH + 1];
							DWORD nSize = MAX_PATH;
							if (QueryFullProcessImageNameW(hProcess, NULL, wsImageName, &nSize))
							{
								printf("Name: %ls\n", wsImageName);
								//   v gets the substring
								if (wcsstr(wsImageName, i_name) != NULL)
								{
									printf("Found: %ls\n", wsImageName);
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

void MainVolumeControl::getMute(const WCHAR *i_name, BOOL* o_mute)
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

void MainVolumeControl::check(HRESULT hr)
{
	if (hr != S_OK) 
	{
		//printf(errMsg);
		printf("Failed\n");
		printf("Error Code: 0x%08lx\n", hr);
		CoUninitialize();
		exit(-1);
	}
}

void MainVolumeControl::destroy()
{
	// Release the resources
	m_audioSessionManager2->Release();
	m_device->Release();
	m_deviceEnumerator->Release();
	CoUninitialize();
}

void MainVolumeControl::getAudioStreams(std::vector<WCHAR*>* o_streams)
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
								// Need to strip the name from back to front starting to the left of .exe
								o_streams->push_back(wsImageName);
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
