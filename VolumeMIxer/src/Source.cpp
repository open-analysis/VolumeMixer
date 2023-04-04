#include <iostream>
#include <vector>
#include <set>
#include "web/webClient.h"
#include "utils/Parser.h"

#define RELEASE 1
#define DEBUG_POST 1
#define DEBUG_DEL 0
#define PROG_INPUT 0
#define DEV_INPUT 0

void sendComputerData(webClient* i_client, std::vector<std::wstring>* i_names, std::set<std::wstring>* i_set, const bool i_isDevice)
{
	unsigned int l_count = 0;

	// Check what items are in the vector vs set
	for (auto i : *i_names)
	{
		l_count++;
		// If the current item is not found in the set
		//  Add it & send it to the server
		if (!i_set->count(i))
		{
			i_set->insert(i);
			//webPost(&i_client, i, i_isDevice);
		}
	}
	// If there's more names in the set than the returned vector
	//  then an item got deleted
	if (l_count < i_set->size())
	{
		for (auto i : *i_set)
		{
			l_count++;
			// If the current item is not found in the set
			//  Add it & send it to the server
			auto t_pos = i_set->find(i);
			if (t_pos != i_set->end())
			{
				i_set->erase(t_pos);
				//webDel(&i_client, t_pos, i_isDevice);
			}
		}
	}

	i_names->clear();
}

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

	std::cout << "MainLoop:Buffer: " << l_buffer << std::endl;

	i_parser->setQueue(l_buffer);
}

void runProgram()
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

	std::set<std::wstring> l_devOutputNames;
	std::set<std::wstring> l_progOutputNames;
#if DEV_INPUT
	std::set<std::wstring> l_devInputNames;
#endif
#if PROG_INPUT
	std::set<std::wstring> l_progInputNames;
#endif
	std::vector<std::wstring> l_currNames;

	while (1)
	{
		// Get & send computer data to server
		l_devOutputCntl.getStreams(l_currNames);
		sendComputerData(&l_client, &l_currNames, &l_devOutputNames, true);
		l_progOutputCntl.getStreams(l_currNames);
		sendComputerData(&l_client, &l_currNames, &l_progOutputNames, false);

		for (auto t : l_devOutputNames)
			std::wcout << t << std::endl;
		for (auto t : l_progOutputNames)
			std::wcout << t << std::endl;

#if DEV_INPUT
		l_devInputCntl.getStreams(l_currNames);
		sendComputerData(&l_client, &l_devInputCntl, &l_devInputNames, true);
#endif
#if PROG_INPUT
		l_progInputCntl.getStreams(l_currNames);
		sendComputerData(&l_client, &l_progInputCntl, &l_progInputNames, false);
#endif

		// Get server data
		webGet(&l_client, &l_parser);

#if 0 // Current out of the program for testing. Don't want to accidentally break anything
		// Parse received data
		l_parser.parseQueue("out", "device");
		l_parser.parseQueue("out", "program");

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

#endif 
		// Flush queue
		l_parser.flushQueue();

		Sleep(1000);
	}

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
	runProgram();
	
	return 0;
}