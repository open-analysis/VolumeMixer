#include <iostream>
#include <vector>
#include <set>
#include "web/webClient.h"
#include "utils/Parser.h"
#include "utils/utils.h"

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

	std::cout << "MainLoop:Buffer: " << l_buffer << std::endl;

	i_parser->setQueue(l_buffer);
}

void sendComputerData(webClient* i_client, std::vector<std::wstring>* i_names, std::set<std::wstring>* i_set, const bool i_isDevice)
{
	utils l_util = utils();
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
			webPost(i_client, (char*)l_util.convertWstr2Str(i).c_str(), i_isDevice);
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
				webDel(i_client, (char*)l_util.convertWstr2Str(*t_pos).c_str(), i_isDevice);
				i_set->erase(t_pos);
			}
		}
	}

	i_names->clear();
}

void runProgram()
{
	webClient l_client;

	AudioControl l_progOutputCntl = AudioControl();
	l_progOutputCntl.init(eRender, eConsole);
	std::set<std::wstring> l_devOutputNames;
	DeviceControl l_devOutputCntl = DeviceControl();
	l_devOutputCntl.init(eRender, eConsole);
	std::set<std::wstring> l_progOutputNames;

	Parser l_parser = Parser(&l_progOutputCntl, &l_devOutputCntl);

#if PROG_INPUT
	AudioControl l_progInputCntl = AudioControl();
	l_progInputCntl.init(eCapture, eCommunications);
	std::set<std::wstring> l_progInputNames;
#endif
#if DEV_INPUT
	DeviceControl l_devInputCntl = DeviceControl();
	l_devInputCntl.init(eCapture, eCommunications);
	std::set<std::wstring> l_devInputNames;
#endif

	std::vector<std::wstring> l_currNames;

	// Main program loop
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
		// Empty the queue
		l_parser.flushQueue();

		// Wait 1 second to tax either system
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