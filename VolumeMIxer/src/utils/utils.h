#pragma once

#include <Windows.h>
#include <string>
#include <Mmdeviceapi.h>
#include <iostream>
#include <cstdint>
#include <codecvt>
#include <locale>

#define SAFE_RELEASE(x) if (x) { x->Release(); x = 0; }

typedef __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0002 role;
typedef __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0001 dataFlow;

class utils
{

public:
	utils() {}

	void check(HRESULT);

	std::wstring extractName(WCHAR[]);

	std::wstring convertStr2Wstr(std::string i_str);
	std::string convertWstr2Str(std::wstring i_wstr);
};

