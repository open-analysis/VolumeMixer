#include "SetVolume.h"
#include "MainVolumeControl.h"

void testSetVolume(int argc, CHAR* argv[])
{
	float newVolume = 0;
	IAudioEndpointVolume* endpointVolume = nullptr;

	// Exit early if only displaying help
	if (argc == 1 || argc > 1 && strcmp(argv[1], "help") == 0) {
		DisplayUsageAndExit();
	}

	// Find devices for manipulating mixer volumes
	InitializeAudioEndpoint(&endpointVolume);

	// Parse command line arguments and perform actions
	if (argc == 2 && strcmp(argv[1], "get") == 0) {
		printf("%.0f\n", GetVolume(endpointVolume) * 100);
	}
	else if (argc == 3 && strcmp(argv[1], "set") == 0) {
		newVolume = strtof(argv[2], nullptr) / 100;

		if (argv[2][0] == '+' || argv[2][0] == '-') {
			newVolume += GetVolume(endpointVolume);
		}

		if (newVolume < 0) newVolume = 0;
		if (newVolume > 1) newVolume = 1;
		SetVolume(endpointVolume, newVolume);
	}
	else if (argc == 2 && strcmp(argv[1], "mute") == 0) {
		BOOL currentValue = GetMute(endpointVolume);
		if (currentValue != TRUE)
			SetMute(endpointVolume, TRUE);
	}
	else if (argc == 2 && strcmp(argv[1], "unmute") == 0) {
		BOOL currentValue = GetMute(endpointVolume);
		if (currentValue != FALSE)
			SetMute(endpointVolume, FALSE);
	}
	else if (argc == 2 && strcmp(argv[1], "togglemute") == 0) {
		BOOL currentValue = GetMute(endpointVolume);
		SetMute(endpointVolume, !currentValue);
	}

	// Cleanup
	DestroyAudioEndpoint(endpointVolume);
}

void testMainVolumeControl()
{
	MainVolumeControl mvc;
	BOOL mute = 1;
	float volume = 0.0f;

	mvc.init();

	mvc.getMute(L"Spotify", &mute);
	printf("\tMute state: %d\n", mute);
	printf("Toggling Spotify\n");
	mvc.toggleMute(L"Spotify");

	/*printf("\nRunning GetMute\n");
	mvc.getMuteSpotify(&mute);
	printf("\tMute state: %d\n", mute);
	printf("\tToggling mute\n");
	mvc.toggleMuteSpotify();
	mvc.getMuteSpotify(&mute);
	printf("\tMute state: %d\n", mute);
	printf("Getting volume\n");
	mvc.getVolumeSpotify(&volume);
	printf("\tVolume level: %f\n", volume);*/

	mvc.destroy();
}

int main(int argc, CHAR* argv[])
{
	//testSetVolume(argc, argv);

	printf("Testing Main Volume Control\n");
	testMainVolumeControl();

	return 0;
}