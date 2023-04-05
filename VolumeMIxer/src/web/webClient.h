#pragma once

#include <Windows.h>
#include <winhttp.h>
#include <iostream>

#pragma comment(lib, "WinHttp.lib")

#define MIXER_IP_ADDR L"192.168.1.142"
#define MIXER_PORT 5000

class WebClient
{
private:
	HINTERNET m_httpSession;
	HINTERNET m_httpConnect;
	HINTERNET m_httpRequest;

public:
	WebClient() { m_httpSession = NULL; m_httpConnect = NULL; m_httpRequest = NULL; }

	char* getQueue();
	bool post(std::string i_buffer, LPCWSTR i_ext);
	bool del(std::string i_buffer, LPCWSTR i_ext);

private:
	bool request(LPCWSTR i_url, LPCWSTR i_action, LPCWSTR i_extension);
	inline void closeConnection()
	{
		if (m_httpSession) WinHttpCloseHandle(m_httpSession);
		if (m_httpConnect) WinHttpCloseHandle(m_httpConnect);
		if (m_httpRequest) WinHttpCloseHandle(m_httpRequest);
	}

};

