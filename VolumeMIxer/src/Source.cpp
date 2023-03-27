#include <iostream>
#include "web/webClient.h"

void testWeb()
{

	webClient client;

	//client.getQueue();
	char l_buf[] = "{\"type\":\"in\", \"name\":\"Cool microphone\", \"img\": \"\", \"default\": true}";
	client.postDevices(l_buf);
}

int main(int argc, CHAR* argv[])
{
	testWeb();

	printf("\nDone\n");

	return 0;
}