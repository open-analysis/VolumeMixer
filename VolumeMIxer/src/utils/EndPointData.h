#pragma once

#include <string>

struct EndPointData {
	std::wstring devID;
	std::wstring name;
	bool bDefault;

	EndPointData() : bDefault(false) { }
};