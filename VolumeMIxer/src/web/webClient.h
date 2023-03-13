#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
// Change these as needed
#define MIXER_IP_ADDR "192.168.1.142"
#define MIXER_PORT "5000"

class webClient
{
private:
	SOCKET m_connSocket;
public:
	int getQueue();
	int postDevices();
	int delDevices();
	int postPrograms();
	int delPrograms();

private:
	int openConnection();
	int closeConnection();

	int sendMessage(char* message, int messageSize);
	int receiveMessage(char* buffer, int bufSize);
};

