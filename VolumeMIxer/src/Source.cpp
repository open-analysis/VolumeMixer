#include <string>
#include <vector>
#include <iostream>
#include "audio/VolumeControl.h"
#include "audio/InputControl.h"
#include "audio/AudioControl.h"
#include "audio/DeviceControl.h"
#include "utils/utils.h"
#include "utils/EndPointData.h"
#include "web/webClient.h"

void testInputControl()
{
	InputControl ic;
	BOOL mute = 1;
	float boost = 0.0f;
	std::vector<std::wstring> names;
	int temp;

	//ic.init();

	ic.getInputStreams(names);
	for (int i = 0; i < names.size(); i++)
	{
		std::cout << "I: " << i << " |  ";
		std::wcout << names.at(i) << std::endl;
		printf("\n");
	}

	//ic.getMute(L"Discord", &mute);
	//printf("\tMute mic: %d\n", mute);
	//printf("Toggling Discord\n");
	//ic.toggleMute(L"Discord");

	std::wstring disc = L"Discord";

	ic.getBoost(&disc, &boost);
	printf("Boost: %f\n", boost);
	boost = .50f;
	printf("Seting Discord boost: %f\n", boost);
	ic.setBoost(&disc, &boost);

	std::cout << "reset boost [Enter]" << std::endl;
	std::cin >> temp;

	boost = 1.0f;
	printf("Seting Discord boost: %f\n", boost);
	ic.setBoost(&disc, &boost);

	ic.destroy();
}

void testVolumeControl()
{
	VolumeControl vc;
	BOOL mute = 1;
	float volume = 0.0f;
	std::vector<std::wstring> names;

	/*vc.getMute(L"Spotify", &mute);
	printf("\tMute state: %d\n", mute);
	printf("Toggling Spotify\n");
	vc.toggleMute(L"Spotify");*/

	vc.getAudioStreams(names);
	for (int i = 0; i < names.size(); i++)
	{
		std::cout << "I: " << i << " |  ";
		std::wcout << names.at(i) << std::endl;
		printf("\n");
	}

	vc.destroy();
}

void testDeviceControl()
{
	DeviceControl dc = DeviceControl(eConsole, eRender);
	std::vector<EndPointData> endPoints;

	dc.init();

	std::cout << "Getting volume endpoints" << std::endl;
	dc.getEndPointDeviceData(endPoints);
	for (auto elem : endPoints)
	{
		std::wcout << elem.devID << std::endl;
		std::wcout << "\t" << elem.name << std::endl;
		std::wcout << "\t" << elem.bDefault << std::endl;

	}

	endPoints.clear();

	BOOL mute;
	std::wstring headphones = L"Speakers (USB Audio DAC   )";
	std::wstring speakers = L"Speakers (Realtek High Definition Audio)";
	dc.getDeviceMute(&headphones, &mute);
	std::cout << "Mute state: " << mute << std::endl;
	mute = !mute;
	dc.setDeviceMute(&headphones, &mute);
	std::cout << "Muting..." << std::endl;
	dc.getDeviceMute(&headphones, &mute);
	std::cout << "Mute state: " << mute << std::endl;

	//std::wstring* currOutput = &speakers;
	std::wstring* currOutput = &headphones;
	role audioRole = eConsole;

	std::cout << "Setting default output to ";
	std::wcout << *currOutput << std::endl;
	dc.setDefaultEndpoint(currOutput, &audioRole);

	dc.destroy();
}

void testWeb()
{
	std::cout << "Starting web client..." << std::endl;

	webClient client;
}

int main(int argc, CHAR* argv[])
{
	/*printf("Testing Volume Control\n");
	testVolumeControl();*/

	/*std::cout << "Testing Audio Control" << std::endl;
	testDeviceControl();*/

	/*std::cout << "\n\nTesting input control" << std::endl;
	testInputControl();*/
	
	std::cout << "Testing web client" << std::endl;
	testWeb();

	printf("\n\n\nDone\n");

	return 0;
}