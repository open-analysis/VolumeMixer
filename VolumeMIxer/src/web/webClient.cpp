#include "webClient.h"

#include <iostream>

int webClient::openConnection()
{
	WSADATA wsaData;
	int l_result;

	// Initialize the socket
	m_connSocket = INVALID_SOCKET;
	
	// Init winsock
	l_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (l_result != 0)
	{
		std::cout << "WSAStartup failed with error: " << l_result << std::endl;
	}

	struct addrinfo *result = NULL,
					*ptr = NULL,
					hints;

	// set address info
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve server address & port
	l_result = getaddrinfo(MIXER_IP_ADDR, MIXER_PORT, &hints, &result);
	if (l_result != 0)
	{
		std::cout << "getaddrinfo failed with error " << l_result << std::endl;
		WSACleanup();
		exit(1);
	}

	// Attempt to connect to address
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Attempt to set up socket with the address
		ptr = result;
		m_connSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (m_connSocket == INVALID_SOCKET)
		{
			std::cout << "socket failed with error " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			WSACleanup();
			exit(1);
		}

		// Connect to the address
		l_result = connect(m_connSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (l_result == SOCKET_ERROR)
		{
			closesocket(m_connSocket);
			m_connSocket = INVALID_SOCKET;
			std::cout << "Server is down, did not connect" << std::endl;
		}
	}

	freeaddrinfo(result);

	if (m_connSocket == INVALID_SOCKET)
	{
		std::cout << "Unable to connect to server! " << std::endl;
		WSACleanup();
		exit(1);
	}

	return l_result;
}

int webClient::closeConnection()
{
	int l_result = shutdown(m_connSocket, SD_SEND);
	closesocket(m_connSocket);
	WSACleanup();

	return l_result;
}

int webClient::sendMessage(char* message, int messageSize)
{
	int l_result = 0;
	l_result = send(m_connSocket, message, messageSize, 0);
	if (l_result == SOCKET_ERROR)
	{
		std::cout << "Send failed: " << WSAGetLastError() << std::endl;
		closesocket(m_connSocket);
		WSACleanup();
	}

	// Shutdown the connection for sending data
	//  Can still receive data on the socket
	l_result = shutdown(m_connSocket, SD_SEND);
	if (l_result == SOCKET_ERROR)
	{
		std::cout << "Shutdown failed: " << WSAGetLastError() << std::endl;
		closesocket(m_connSocket);
		WSACleanup();
	}

	return l_result;
}

int webClient::receiveMessage(char* buffer, int bufSize)
{
	int l_result = 0;
	l_result = recv(m_connSocket, buffer, bufSize, 0);

	if (l_result > 0)
	{
		std::cout << "data received, do something now" << buffer << std::endl;
	}
	else if (l_result < 0)
	{
		std::cout << "recv failed " << WSAGetLastError() << std::endl;
	}

	return l_result;
}
