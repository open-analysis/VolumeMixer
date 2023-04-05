#include <iostream>
#include <vector>
#include <set>
#include "web/WebClient.h"
#include "utils/Parser.h"
#include "utils/Utils.h"

#define PROG_INPUT 0
#define DEV_INPUT 0
#define SLEEP_TIME 1000

void webDel(WebClient* i_client, char* i_buffer, const bool i_isDevice)
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

void webPost(WebClient* i_client, char* i_buffer, const bool i_isDevice)
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

void webGet(WebClient* i_client, Parser* i_parser)
{
	char* l_buffer = nullptr;

	l_buffer = i_client->getQueue();

	std::cout << "MainLoop:Buffer: " << l_buffer << std::endl;

	i_parser->setQueue(l_buffer);
}

std::vector<std::vector<std::wstring>> getComputerData(std::vector<std::wstring>* i_names, std::set<std::wstring>* i_set)
{
	unsigned int l_count = 0;

	std::vector<std::vector<std::wstring>> r_results;
	r_results.resize(2);

	// Check what items are in the vector vs set
	for (auto i : *i_names)
	{
		l_count++;
		// If the current item is not found in the set
		//  Add it & send it to the server
		if (!i_set->count(i))
		{
			i_set->insert(i);
			r_results[0].push_back(i);
		}
	}
	// If there's more names in the set than the returned vector
	//  then an item got deleted
	if (l_count < i_set->size())
	{
		for (auto i : *i_set)
		{
			// If the current item is not found in the set
			//  Add it & send it to the server
			auto t_pos = i_set->find(i);
			if (t_pos != i_set->end())
			{
				r_results[1].push_back(i);
				i_set->erase(t_pos);
			}
		}
	}

	i_names->clear();

	return r_results;
}

void sendComptuerDevices(WebClient* i_client, Parser* i_parser, DeviceControl* i_devCntl, std::vector<std::vector<std::wstring>>* i_actions, const bool i_isOutput)
{
	std::string l_buffer;

	std::vector<EndPointData> l_endPointData;
	i_devCntl->getEndPointDeviceData(l_endPointData);


	for (auto t_datum : l_endPointData)
	{
		// POST Data
		for (auto t_name : i_actions[0][0])
		{
			if (t_name == t_datum.name)
			{
				l_buffer = i_parser->device2Json(i_isOutput, t_datum.name, i_devCntl->getImgPath(), t_datum.bDefault);
				i_client->post(l_buffer, L"devices");
				break;
			}
		}
	
		// DELETE Data
		for (auto t_name : i_actions[0][1])
		{
			if (t_name == t_datum.name)
			{
				l_buffer = i_parser->device2Json(i_isOutput, t_datum.name, i_devCntl->getImgPath(), t_datum.bDefault);
				i_client->del(l_buffer, L"devices");
				break;
			}
		}
	}
}

void sendComptuerPrograms(WebClient* i_client, Parser* i_parser, AudioControl* i_audCntl, std::vector<std::vector<std::wstring>>* i_actions)
{
	std::string l_buffer;

	// POST Data
	for (auto t_name : i_actions[0][0])
	{
		l_buffer = i_parser->program2Json(t_name, i_audCntl->getImgPath());
		i_client->post(l_buffer, L"programs");
	}

	// DELETE Data
	for (auto t_name : i_actions[0][1])
	{
		l_buffer = i_parser->program2Json(t_name, i_audCntl->getImgPath());
		i_client->del(l_buffer, L"programs");
	}
}

void runProgram()
{
	WebClient l_client = WebClient();

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
	std::vector<std::vector<std::wstring>> l_actions;

	// Main program loop
	while (1)
	{
		// Get & send computer data to server
		l_devOutputCntl.getStreams(l_currNames);
		l_actions = getComputerData(&l_currNames, &l_devOutputNames);
		sendComptuerDevices(&l_client, &l_parser, &l_devOutputCntl, &l_actions, true);
		l_progOutputCntl.getStreams(l_currNames);
		l_actions = getComputerData(&l_currNames, &l_progOutputNames);
		sendComptuerPrograms(&l_client, &l_parser, &l_progOutputCntl, &l_actions);

#if DEV_INPUT
		l_devInputCntl.getStreams(l_currNames);
		l_actions = getComputerData(&l_client, &l_devInputCntl, &l_devInputNames);
#endif
#if PROG_INPUT
		l_progInputCntl.getStreams(l_currNames);
		l_actions = getComputerData(&l_client, &l_progInputCntl, &l_progInputNames);
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

		l_actions.clear();

		// Wait 1 second to tax either system
		Sleep(SLEEP_TIME);
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