#pragma once

#include <string>
#include <vector>
#include "EndPointData.h"

struct Audio
{
public:
	std::string m_name;
	std::string m_img;
	int m_volume;
	BOOL m_mute;

};

struct AudioDevice : public Audio
{
public: 
	BOOL m_is_default;
	BOOL m_is_output;
};