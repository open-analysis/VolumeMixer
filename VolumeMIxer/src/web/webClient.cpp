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

void WebClient::handshake(std::vector<AudioDevice>& i_devices, std::vector<Audio>& i_programs, Parser& i_parser)
{
	bool l_results = true;
	//LPSTR l_buffer = NULL;
	std::string l_buffer = "";
	Utils l_util = Utils();
	Parser l_parser = Parser();

	// Get the queue from the server
	l_buffer = getQueue();
	// Set the queue in the parser
	i_parser.setQueue(l_buffer.c_str());

	l_buffer = "";

	// Send the devices to the server
	for (auto l_device : i_devices)
	{
		l_buffer = i_parser.device2Json(l_device);
		post(l_buffer, L"devices");
	}

	// Send the programs to the server
	for (auto l_program : i_programs)
	{
		l_buffer = i_parser.program2Json(l_program);
		post(l_buffer, L"programs");
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
