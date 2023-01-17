#include "InputControl.h"

InputControl::InputControl()
{
	AudioControl::init(eCapture, eCommunications);
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

void InputControl::setBoost(const std::wstring* i_name, const float* i_volume)
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

void InputControl::getBoost(const std::wstring* i_name, float* o_level)
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

void InputControl::muteAll()
{
	constexpr BOOL MUTE = true;
	std::vector<std::wstring> inputs;

	getInputStreams(inputs);

	for (auto prog : inputs)
	{
		AudioControl::setMute(&prog, &MUTE);
	}

	inputs.clear();
}

void InputControl::unmuteAll()
{
	constexpr BOOL MUTE = false;
	std::vector<std::wstring> inputs;

	getInputStreams(inputs);

	for (auto prog : inputs)
	{
		AudioControl::setMute(&prog, &MUTE);
	}

	inputs.clear();
}