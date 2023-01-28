#pragma once

#include <Windows.h>
#include <string>
#include <Mmdeviceapi.h>

#define DEBUG 0

#if DEBUG
#include <iostream>
#endif 

typedef __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0002 role;
typedef __MIDL___MIDL_itf_mmdeviceapi_0000_0000_0001 dataFlow;

class utils
{

public:
	utils() {}

	void check(HRESULT);

	std::wstring extractName(WCHAR[]);
};

