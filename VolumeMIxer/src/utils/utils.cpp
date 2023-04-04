#include "utils.h"

void utils::check(HRESULT hr)
{
	if (hr != S_OK)
	{
		//printf(errMsg);
		printf("Failed\n");
		printf("Error Code: 0x%08lx\n", hr);
		CoUninitialize();
		exit(-1);
	}
}

std::wstring utils::extractName(WCHAR i_imageName[])
{
	constexpr u_int LEN_IMAGE_MAX = MAX_PATH + 1;
	std::wstring o_progName;

	u_int endImage = 0;
	u_int startExe = 0;
	u_int startProg = endImage;
	u_int lenImage = 0;
	u_int lenProg = 0;
	u_int check = 0;

	// Start by finding the end of the path
	for (u_int i = 0; i < LEN_IMAGE_MAX; i++)
	{
		if ((check == 0) && (i_imageName[i] == L'.')) check++;
		else if ((check == 1) && (i_imageName[i] == L'e')) check++;
		else if ((check == 2) && (i_imageName[i] == L'x')) check++;
		else if ((check == 3) && (i_imageName[i] == L'e'))
		{
			endImage = i;
			startProg = endImage - 4;
			startExe = endImage - 3;
			break;
		}
		else
		{
			check = 0;
		}
	}

	// Find the starting character location the program name
	//  and gets the length of the name
	while (i_imageName[startProg] != L'\\')
	{
		lenProg++;
		startProg--;
	}

	startProg++;
	lenProg--;

	for (u_int i = startProg; i < startExe; i++)
	{
		o_progName += i_imageName[i];
	}

#if DEBUG
	std::wcout << o_progName << std::endl;
#endif

	return o_progName;

}

std::wstring utils::convertStr2Wstr(std::string i_str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(i_str);
}

std::string utils::convertWstr2Str(std::wstring i_wstr)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(i_wstr);
}