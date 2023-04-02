#include <iostream>
#include <vector>
#include "web/webClient.h"
#include "utils/Parser.h"

#define DEBUG_POST 1
#define DEBUG_DEL 0
#define PROG_INPUT 0
#define DEV_INPUT 0

void webDel(webClient* i_client, char* i_buffer, const bool i_isDevice)
{
	LPCWSTR l_ext = nullptr;
	if (i_isDevice)
	{
		l_ext = L"devices";
	}
	else
	{
		l_ext = L"programs";
	}
	i_client->del(i_buffer, l_ext);
}

void webPost(webClient* i_client, char* i_buffer, const bool i_isDevice)
{
	LPCWSTR l_ext = nullptr;
	if (i_isDevice)
	{
		l_ext = L"devices";
	}
	else
	{
		l_ext = L"programs";
	}
	i_client->post(i_buffer, l_ext);
}

void webGet(webClient* i_client, Parser* i_parser)
{
	char* l_buffer = nullptr;

	l_buffer = i_client->getQueue();

	//std::cout << "MainLoop:Buffer: " << l_buffer << std::endl;

	i_parser->setQueue(l_buffer);
}

void run()
{
	webClient l_client;

	AudioControl l_progOutputCntl = AudioControl();
	l_progOutputCntl.init(eRender, eConsole);
	DeviceControl l_devOutputCntl = DeviceControl();
	l_devOutputCntl.init(eRender, eConsole);

	Parser l_parser = Parser(&l_progOutputCntl, &l_devOutputCntl);

#if PROG_INPUT
	AudioControl l_progInputCntl = AudioControl();
	l_progInputCntl.init(eCapture, eCommunications);
#endif
#if DEV_INPUT
	DeviceControl l_devInputCntl = DeviceControl();
	l_devInputCntl.init(eCapture, eCommunications);
#endif

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

#if DEBUG_POST
	for (auto dev : l_buf_dev)
	{
		if (dev[0] == '{')
		{
			std::cout << dev << std::endl;
			webPost(&l_client, dev, 1);
		}
	}
	
	for (auto prog : l_buf_progs)
	{
		if (prog[0] == '{')
		{
			std::cout << prog << std::endl;
			webPost(&l_client, prog, 0);
		}
	}

	std::cout << "[Enter]" << std::endl;
	std::cin.get();
#endif

	webGet(&l_client, &l_parser);

	/*l_parser.parseQueue("out", "device");
	l_parser.parseQueue("out", "program");*/


#if PROG_INPUT
	l_parser.setAudioCntl(&l_progInputCntl);
	l_parser.parseQueue("in", "program");
	l_parser.setAudioCntl(&l_progOutputCntl);
#endif
#if DEV_INPUT
	l_parser.setAudioCntl(&l_devInputCntl);
	l_parser.parseQueue("in", "device");
	l_parser.setAudioCntl(&l_devOutputCntl);
#endif
	
	l_parser.flushQueue();

	std::cout << "[Enter]" << std::endl;
	std::cin.get();

#if DEBUG_DEL
	webDel(&l_client, l_buf_dev[0], 1);
	webDel(&l_client, l_buf_progs[0], 0);
#endif

	l_progOutputCntl.destroy();
	l_devOutputCntl.destroy();

#if PROG_INPUT
	l_progInputCntl.destroy();
#endif
#if DEV_INPUT
	l_devInputCntl.destroy();
#endif
}

int main(int argc, CHAR* argv[])
{
	run();

	return 0;
}