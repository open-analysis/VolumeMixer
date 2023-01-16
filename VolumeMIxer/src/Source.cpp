#include <vector>
#include <iostream>
#include "audio/VolumeControl.h"
#include "audio/InputControl.h"

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

	ic.getBoost(L"Discord", &boost);
	printf("\Boost: %f\n", boost);
	boost = .20f;
	printf("Seting Discord boost: %f\n", boost);
	ic.setBoost(L"Discord", &boost);

	std::cout << "reset boost [Enter]" << std::endl;
	std::cin >> temp;

	boost = 1.0f;
	printf("Seting Discord boost: %f\n", boost);
	ic.setBoost(L"Discord", &boost);

	ic.destroy();
}

void testMainVolumeControl()
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

int main(int argc, CHAR* argv[])
{
	/*printf("Testing Volume Control\n");
	testMainVolumeControl();*/

	std::cout << "\n\nTesting input control" << std::endl;
	testInputControl();

	return 0;
}