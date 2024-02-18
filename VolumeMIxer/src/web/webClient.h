#pragma once

#include <Windows.h>
#include <winhttp.h>
#include <iostream>
#include <set>
#include "../utils/Utils.h"
#include "../utils/Parser.h"

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

	void handshake(std::vector<AudioDevice>& i_devices, std::vector<Audio>& i_programs, Parser& i_parser);
	char* getQueue();
	bool post(std::string i_buffer, LPCWSTR i_ext);
	bool del(std::string i_buffer, LPCWSTR i_ext); // DEPRECIATED

private:
	bool request(LPCWSTR i_url, LPCWSTR i_action, LPCWSTR i_extension);
	inline void closeConnection()
	{
		if (m_httpSession) WinHttpCloseHandle(m_httpSession);
		if (m_httpConnect) WinHttpCloseHandle(m_httpConnect);
		if (m_httpRequest) WinHttpCloseHandle(m_httpRequest);
	}

	bool readData(const LPCWSTR i_url, const LPCWSTR i_extension, LPSTR& io_buffer);
	bool sendData(const LPCWSTR i_url, const LPCWSTR i_action, const LPCWSTR i_extension, const std::string i_buffer);
};

