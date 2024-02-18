#include <iostream>
#include <vector>
#include <set>
#include <Windows.h>
#include "web/WebClient.h"
#include "utils/Parser.h"
#include "utils/Utils.h"
#include "utils/AudioStructs.h"

#define DEBUG 0
#define WEB_CONN 0
#define PROG_INPUT 0
#define DEV_INPUT 0
#define SLEEP_TIME 1000

// Make sure the current list of device names is in the set
//  This is not the most efficient use of system resources. Could be improved by going back to the old method of checking against what's in the list
//  But this is easier, as removing items doesn't have to be dealt with and trying to find a pointer that doesn't exist anymore.
void updateClientDevices(std::vector<std::wstring>& i_names, DeviceControl* i_device_cntl, std::vector<AudioDevice>* o_set)
{
	Utils util = Utils();
	std::vector<EndPointData> l_endPointData;
	float l_vol = 0;
	BOOL l_mute = 0;
	BOOL l_is_default = 0;
	BOOL l_is_output = 1;

	i_device_cntl->getEndPointDeviceData(l_endPointData);
	l_is_output = i_device_cntl->getIsOutput();
	// Clear the set & re-add all of the dat
	o_set->clear();
	// Check what items are in the vector vs set
	for (int i = 0; i < i_names.size(); i++)
	{
		AudioDevice t_device = AudioDevice();
		i_device_cntl->getVolume(&i_names[i], &l_vol);
		i_device_cntl->getMute(&i_names[i], &l_mute);

		for (auto l_datum : l_endPointData)
		{
			if (i_names[i] == l_datum.name)
			{
				l_is_default = l_datum.bDefault;
				break;
			}
		}

		t_device.m_img = "";
		t_device.m_name = util.convertWstr2Str(i_names[i]);
		t_device.m_volume = static_cast<int>(l_vol * 100);
		t_device.m_mute = l_mute;
		t_device.m_is_default = l_is_default;
		t_device.m_is_output = l_is_output;

		o_set->push_back(t_device);
	}

	// clear the names when done to minimize chances of duplicates or cross contamination
	i_names.clear();
}

// Make sure the current list of program names is in the set
void updateClientPrograms(std::vector<std::wstring>& i_names, AudioControl* i_audio_cntl, std::vector<Audio>* o_set)
{
	Utils util = Utils();
	float l_vol = 0;
	BOOL l_mute = 0;

	// Clear the set & re-add all of the dat
	o_set->clear();
	// Check what items are in the vector vs set
	for (int i = 0; i < i_names.size(); i++)
	{
		Audio t_audio = Audio();
		i_audio_cntl->getVolume(&i_names[i], &l_vol);
		i_audio_cntl->getMute(&i_names[i], &l_mute);

		t_audio.m_img = "";
		t_audio.m_name = util.convertWstr2Str(i_names[i]);
		t_audio.m_volume = static_cast<int>(l_vol * 100);
		t_audio.m_mute = l_mute;

		o_set->push_back(t_audio);
	}

	// clear the names when done to minimize chances of duplicates or cross contamination
	i_names.clear();
}

void runProgram()
{
#if WEB_CONN
	WebClient l_client = WebClient();
#endif

	bool l_runProgram = true;

	AudioControl l_progOutputCntl = AudioControl();
	l_progOutputCntl.init(eRender, eConsole);
	DeviceControl l_devOutputCntl = DeviceControl();
	l_devOutputCntl.init(eRender, eConsole);
	std::vector<AudioDevice> l_output_audio_devices;
	std::vector<Audio> l_output_audio_programs;

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
	while (l_runProgram)
	{
#if DEBUG
		std::cout << "\t\tDebuggin time" << std::endl;
#endif

#if WEB_CONN
		l_client.handshake(l_output_audio_devices, l_output_audio_programs);
#endif

		// Get & send computer data to server

		// DEVICES
		l_devOutputCntl.getStreams(l_currNames);
		updateClientDevices(l_currNames, &l_devOutputCntl, &l_output_audio_devices);
#if DEBUG
		std::cout << "\tOutput Devices" << std::endl;
		for (auto l_tmp : l_output_audio_devices)
			std::cout << l_tmp.m_name << std::endl;
		
		std::cout << "\tOutput Device Action" << std::endl;
		for (auto l_tmp1 : l_actions)
			for (auto l_tmp : l_tmp1)
				std::wcout << l_tmp << std::endl;
#endif

		// PROGRAMS
		l_progOutputCntl.getStreams(l_currNames);
		updateClientPrograms(l_currNames, &l_progOutputCntl, &l_output_audio_programs);
#if DEBUG
		std::cout << "\tOutput Programs" << std::endl;
		for (auto l_tmp : l_output_audio_programs)
			std::cout << l_tmp.m_name << std::endl;

		std::cout << "\tOutput Program Action" << std::endl;
		for (auto l_tmp1 : l_actions)
			for (auto l_tmp : l_tmp1)
				std::wcout << l_tmp << std::endl;
#endif

#if DEV_INPUT
		l_devInputCntl.getStreams(l_currNames);
		l_actions = getComputerData(&l_client, &l_devInputCntl, &l_devInputNames);
		l_currNames.clear();
#if DEBUG
		std::cout << "Input Devices" << std::endl;
		for (auto l_tmp : l_devInputNames)
			std::wcout << l_tmp << std::endl;
		for (auto l_tmp1 : l_actions)
			for (auto l_tmp : l_tmp1)
				std::wcout << l_tmp << std::endl;
#endif
#endif
#if PROG_INPUT
		l_progInputCntl.getStreams(l_currNames);
		l_actions = getComputerData(&l_client, &l_progInputCntl, &l_progInputNames);
		l_currNames.clear();
#if DEBUG
		std::cout << "Input Devices" << std::endl;
		for (auto l_tmp : l_progInputNames)
			std::wcout << l_tmp << std::endl;
		for (auto l_tmp1 : l_actions)
			for (auto l_tmp : l_tmp1)
				std::wcout << l_tmp << std::endl;
#endif
#endif

#if WEB_CONN
		// Handshake with the server
		// IE update the queue of actions from the server & update the server with client data
		l_client.handshake(l_output_audio_devices, l_output_audio_programs, l_parser);

		// Parse received data
		l_parser.parseQueue("out", "device");
		l_parser.parseQueue("out", "program");
#endif

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

#if 0
int WinMain(HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR    lpCmdLine,
			int       cmdShow)
{
	runProgram();

	return 0;
}
#else
int main()
{
	runProgram();

	return 0;
}
#endif