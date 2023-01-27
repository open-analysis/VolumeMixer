#include <string>
#include <vector>
#include <iostream>
#include "audio/VolumeControl.h"
#include "audio/InputControl.h"
#include "audio/AudioControl.h"
#include "audio/EndPointData.h"

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

void testAudioControl()
{
	VolumeControl vc;
	std::vector<EndPointData> endPoints;

	std::cout << "Getting volume endpoints" << std::endl;
	vc.GetEndPointDeviceData(endPoints);
	for (auto elem : endPoints)
	{
		std::wcout << elem.devID << std::endl;
		std::wcout << "\t" << elem.name << std::endl;
		std::wcout << "\t" << elem.bDefault << std::endl;

	}

	endPoints.clear();
	InputControl ic;
	std::cout << "Getting input endpoints" << std::endl;
	ic.GetEndPointDeviceData(endPoints);
	for (auto elem : endPoints)
	{
		std::wcout << elem.devID << std::endl;
		std::wcout << "\t" << elem.name << std::endl;
		std::wcout << "\t" << elem.bDefault << std::endl;

	}

}

int main(int argc, CHAR* argv[])
{
	/*printf("Testing Volume Control\n");
	testVolumeControl();*/

	std::cout << "Testing Audio Control" << std::endl;
	testAudioControl();

	/*std::cout << "\n\nTesting input control" << std::endl;
	testInputControl();*/
	
	printf("\n\n\nDone\n");

	return 0;
}