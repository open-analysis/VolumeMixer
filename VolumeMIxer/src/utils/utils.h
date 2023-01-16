#pragma once

#include <Windows.h>
#include <string>

#define DEBUG 0

#if DEBUG
#include <iostream>
#endif 

class utils
{

public:
	utils() {}

	void check(HRESULT);

	std::wstring extractName(WCHAR[]);
};

