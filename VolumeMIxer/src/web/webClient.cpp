#include "WebClient.h"

char* WebClient::getQueue()
{
	bool l_results = true;
	LPSTR l_buffer = NULL;
	
	l_results = readData(MIXER_IP_ADDR, L"queue", l_buffer);

	if (!l_results)
	{
		l_buffer = NULL;
	}

	return l_buffer;
}

void WebClient::handshake(const std::set<std::wstring> i_devNames, const std::set<std::wstring> i_progNames)
{
	bool l_results = true;
	LPSTR l_buffer = NULL;
	LPCWSTR l_ext = L"devices";
	Utils l_util = Utils();

	l_results = readData(MIXER_IP_ADDR, l_ext, l_buffer);

	if (l_buffer != NULL)
	{
		std::cout << "Debuggin handshake" << std::endl;
		std::cout << l_buffer << std::endl;
		return;
		for (int i = 0; i < sizeof(*l_buffer) / sizeof(l_buffer[0]); i++)
		{
			std::wcout << l_buffer[i] << std::endl;
			for (auto l_devName : i_devNames)
			{
				if (!strcmp(l_buffer, l_util.convertWstr2Str(l_devName).c_str()))
				{
					// figure out which data is wrong & update the appropriate system
				}
			}
		}
	}

}

bool WebClient::post(std::string i_buffer, LPCWSTR i_ext)
{
	bool l_results = true;

	l_results = sendData(MIXER_IP_ADDR, L"POST", i_ext, i_buffer);

	return l_results;
}

bool WebClient::del(std::string i_buffer, LPCWSTR i_ext)
{
	bool l_results = true;

	l_results = sendData(MIXER_IP_ADDR, L"DELETE", i_ext, i_buffer);

	return l_results;
}

bool WebClient::request(LPCWSTR i_url, LPCWSTR i_action, LPCWSTR i_extension)
{
	m_httpSession = WinHttpOpen(L"Volume Mixer Program",
								WINHTTP_ACCESS_TYPE_NO_PROXY,
								WINHTTP_NO_PROXY_NAME,
								WINHTTP_NO_PROXY_BYPASS,
								0);
	if (!m_httpSession)
	{
		std::cout << "Failed to establish HTTP Session (WinHttpOpen): " << GetLastError() << std::endl;
		return 0;
	}

	m_httpConnect = WinHttpConnect(m_httpSession, 
								   MIXER_IP_ADDR, 
								   MIXER_PORT, 
								   0);
	if (!m_httpSession)
	{
		std::cout << "Failed to establish HTTP connection (WinHttpConnect): " << GetLastError() << std::endl;
		return 0;
	}


	m_httpRequest = WinHttpOpenRequest(m_httpConnect, 
									   i_action, 
									   i_extension,
									   NULL,
									   WINHTTP_NO_REFERER, 
									   WINHTTP_DEFAULT_ACCEPT_TYPES, 
									   0);
	if (!m_httpRequest)
	{
		std::cout << "Failed to establish HTTP Request (WinHttpOpenRequest): " << GetLastError() << std::endl;
		return 0;
	}
	
	return 1;
}

bool WebClient::readData(LPCWSTR i_url, LPCWSTR i_extension, LPSTR& io_buffer)
{
	LPCWSTR c_action = L"GET";

	bool l_results = true;
	DWORD l_bytesAvailable = 0;
	DWORD l_bytesRead = 0;

	if (request(MIXER_IP_ADDR, c_action, i_extension))
	{

		// Send the GET request
		if (!WinHttpSendRequest(m_httpRequest,
								WINHTTP_NO_ADDITIONAL_HEADERS,
								0,
								WINHTTP_NO_REQUEST_DATA,
								0,
								0,
								0))
		{
			std::cout << "Failed to send HTTP Request (WinHttpSendRequest): " << GetLastError() << std::endl;
			l_results = false;
			goto end_request;
		}

		// Receive the request response
		if (!WinHttpReceiveResponse(m_httpRequest, NULL))
		{
			std::cout << "Failed to receive/end HTTP Request (WinHttpReceiveRequest): " << GetLastError() << std::endl;
			l_results = false;
			goto end_request;
		}

		// Verify that there's data to receive
		if (!WinHttpQueryDataAvailable(m_httpRequest, &l_bytesAvailable))
		{
			std::cout << "Failed to check if return data is available (WinHttpQueryDataAvailable): " << GetLastError() << std::endl;
			l_results = false;
			goto end_request;
		}

		// Start reading in the data
		if (l_bytesAvailable > 0)
		{
			// Prepare the buffer
			io_buffer = new char[l_bytesAvailable + 1];
			if (!io_buffer)
			{
				std::cout << "Failed to create new buffer (Likely ran out of stack memory). " << std::endl;
				l_results = false;
				goto end_request;
			}
			else
			{
				ZeroMemory(io_buffer, l_bytesAvailable + 1);

				if (!WinHttpReadData(m_httpRequest, (LPVOID)io_buffer, l_bytesAvailable, &l_bytesRead))
				{
					std::cout << "Failed to read HTTP data (WinHttpReadData): " << GetLastError() << std::endl;
					l_results = false;
					goto end_request;
				}
			}
		}
	}

end_request:
	closeConnection();
	return l_results;
}

bool WebClient::sendData(const LPCWSTR i_url, const LPCWSTR i_action, const LPCWSTR i_extension, const std::string i_buffer)
{
	bool l_results = true;
	DWORD l_bytesWritten = 0;

	if (request(MIXER_IP_ADDR, i_action, i_extension))
	{
		// Send the POST request
		if (!WinHttpSendRequest(m_httpRequest,
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0,
			WINHTTP_NO_REQUEST_DATA,
			0,
			(DWORD)i_buffer.length(),
			0))
		{
			std::cout << "Failed to send HTTP Request (WinHttpSendRequest): " << GetLastError() << std::endl;
			l_results = false;
			goto end_request;
		}

		// Write data to the server.
		if (!WinHttpWriteData(m_httpRequest,
			i_buffer.c_str(),
			(DWORD)i_buffer.length(),
			&l_bytesWritten))
		{
			std::cout << "Failed to write HTTP data (WinHttpWriteData): " << GetLastError() << std::endl;
			l_results = false;
			goto end_request;
		}

		// End the request.
		if (!WinHttpReceiveResponse(m_httpRequest, NULL))
		{
			std::cout << "Failed to end HTTP request (WinHttpReceiveRequest): " << GetLastError() << std::endl;
			l_results = false;
			goto end_request;
		}
	}

end_request:
	closeConnection();
	return l_results;
}
