#include <iostream>
#include "web/webClient.h"

void testWeb()
{

	webClient client;

	char* l_buffer = nullptr;

	char l_buf_dev[15][75] = { "{\"type\": \"in\", \"name\" : \"microphone\", \"img\" : \"\", \"default\" : true}",
							   "{\"type\": \"out\", \"name\" : \"speakers\", \"img\" : \"\", \"default\" : false}",
							   "{\"type\": \"in\", \"name\" : \"test input\", \"img\" : \"\", \"default\" : false}",
							   "{\"type\": \"out\", \"name\" : \"headphones\", \"img\" : \"\", \"default\" : true}" };

	char l_buf_progs[15][50] = {"{\"name\":\"Spotify\", \"img\": \"\"}",
							 "{\"name\":\"Discord\", \"img\": \"\"}",
							 "{\"name\":\"Firefox\", \"img\": \"\"}",
							 "{\"name\":\"Civ6\", \"img\": \"\"}",
							 "{\"name\":\"Prog0\", \"img\": \"\"}",
							 "{\"name\":\"Prog1\", \"img\": \"\"}",
							 "{\"name\":\"Prog2\", \"img\": \"\"}",
							 "{\"name\":\"Prog3\", \"img\": \"\"}",
							 "{\"name\":\"Prog4\", \"img\": \"\"}",
							 "{\"name\":\"Prog5\", \"img\": \"\"}" };

	for (auto dev : l_buf_dev)
	{
		if (dev[0] == '{')
		{
			std::cout << dev << std::endl;
			client.postDevices(dev);
		}
	}
	
	for (auto prog : l_buf_progs)
	{
		if (prog[0] == '{')
		{
			std::cout << prog << std::endl;
			client.postPrograms(prog);
		}
	}

	std::cout << "[Enter]" << std::endl;
	std::cin.get();

	l_buffer = client.getQueue();

	std::cout << "MainLoop:Buffer: " << l_buffer << std::endl;

	std::cout << "[Enter]" << std::endl;
	std::cin.get();

	client.deletePrograms(l_buf_progs[0]);

	std::cout << "[Enter]" << std::endl;
	std::cin.get();

	client.deleteDevices(l_buf_dev[0]);

}

int main(int argc, CHAR* argv[])
{
	testWeb();

	printf("\nDone\n");

	return 0;
}