#include "Parser.h"

Parser::Parser()
{
	std::cout << "Don't use this constructor" << std::endl;
	m_volCntl = nullptr;
	m_inCntl = nullptr;
	m_devCntl = nullptr;
}

Parser::Parser(VolumeControl* i_volCntl, InputControl* i_inCntl, DeviceControl* i_devCntl) :
	m_volCntl(i_volCntl), m_inCntl(i_inCntl), m_devCntl(i_devCntl)
{
}

void Parser::parseQueue(char* i_buffer, std::vector<std::wstring>& i_outputs, std::vector<std::wstring>& i_inputs, std::vector<std::wstring>& i_devices)
{
	std::string l_str = (std::string)i_buffer;
	std::vector<std::string> l_cmds;
	size_t l_start = 0;
	size_t l_end = 0;
	char l_delim = ' ';
	std::wstring l_name;

	while ((l_start = l_str.find_first_not_of(l_delim, l_end)) != std::string::npos)
	{
		l_end = l_str.find(l_delim, l_start);
		l_cmds.push_back(l_str.substr(l_start, (l_end - l_start)));
	}

	l_name = convert(l_cmds[1]);

	if (l_cmds[0] == "device")
	{
		if (l_cmds[2] == "mute")
		{
			if (l_cmds[3] == "True")
			{
				m_devCntl->setDeviceMute(&l_name, true);
			}
			else if (l_cmds[3] == "False")
			{
				m_devCntl->setDeviceMute(&l_name, false);
			}
			else if (l_cmds[3] == "toggle")
			{
				m_devCntl->toggleDeviceMute(&l_name);
			}
		}
		else if (l_cmds[2] == "volume")
		{
			float l_vol = 0;
			m_devCntl->getDeviceVolume(&l_name, &l_vol);
			l_vol += (stoi(l_cmds[3]) / 100);
			m_devCntl->setDeviceVolume(&l_name, l_vol);
		}
		else if (l_cmds[2] == "setDefault")
		{
			if (l_cmds[3] == "out")
			{
				m_devCntl->setDefaultEndpoint(&l_name, ERole::eConsole);
			}
			else
			{
				m_devCntl->setDefaultEndpoint(&l_name, ERole::eCommunications);
			}
		}
	}
	else
	{
		if (l_cmds[2] == "mute")
		{
			if (l_cmds[3] == "True")
			{
				m_volCntl->setMute(&l_name, true);
			}
			else if (l_cmds[3] == "False")
			{
				m_volCntl->setMute(&l_name, false);
			}
			else if (l_cmds[3] == "toggle")
			{
				m_volCntl->toggleMute(&l_name);
			}
		}
		else if (l_cmds[2] == "volume")
		{
			float l_vol = 0;
			m_volCntl->getVolume(&l_name, &l_vol);
			l_vol += (stoi(l_cmds[3]) / 100);
			m_volCntl->setVolume(&l_name, l_vol);
		}
	}
}

inline std::wstring Parser::convert(std::string i_str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(i_str);
}