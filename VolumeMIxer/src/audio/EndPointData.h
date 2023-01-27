#pragma once

struct EndPointData {
	std::wstring devID;
	std::wstring name;
	bool bDefault;

	EndPointData() : bDefault(false) { }
};