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
	constexpr char c_queue_delim = ',';
	constexpr char c_cmd_delim = ' ';

	std::string l_str = (std::string)i_buffer;
	std::vector<std::string> l_queue;
	char c_remove_chars[] = "[\"],";
	size_t l_queue_start = 0;
	size_t l_queue_end = 0;
	std::wstring l_name;

	// Break up the queue into each separate command
	while ((l_queue_start = l_str.find_first_not_of(c_queue_delim, l_queue_end)) != std::string::npos)
	{
		l_queue_end = l_str.find(c_queue_delim, l_queue_start);
		l_queue.push_back(l_str.substr(l_queue_start, (l_queue_end - l_queue_start)));
	}
	for (auto t_queue : l_queue)
	{
		removeCharsFromString(t_queue, c_remove_chars);
	}

	for (auto t_queue : l_queue)
	{
		std::vector<std::string> l_cmds;
		size_t l_cmd_start = 0;
		size_t l_cmd_end = 0;

		// Break the command into it's constituent parts
		while ((l_cmd_start = t_queue.find_first_not_of(c_cmd_delim, l_cmd_end)) != std::string::npos)
		{
			l_cmd_end = t_queue.find(c_cmd_delim, l_cmd_start);
			l_cmds.push_back(t_queue.substr(l_cmd_start, (l_cmd_end - l_cmd_start)));
		}

		// Get the name of the device/program
		l_name = convert(l_cmds[1]);

		// Determine type that's being operated on
		if (l_cmds[0] == "device")
		{
			// Determine action
			if (l_cmds[2] == "mute")
			{
				// Determine state
				if (l_cmds[3] == "True")
				{
					m_devCntl->setDeviceMute(&l_name, true);
				}
				// Determine state
				else if (l_cmds[3] == "False")
				{
					m_devCntl->setDeviceMute(&l_name, false);
				}
				// Determine state
				else if (l_cmds[3] == "toggle")
				{
					m_devCntl->toggleDeviceMute(&l_name);
				}
			}
			// Determine action
			else if (l_cmds[2] == "volume")
			{
				float l_vol = 0;
				m_devCntl->getDeviceVolume(&l_name, &l_vol);
				l_vol += (stoi(l_cmds[3]) / 100);
				m_devCntl->setDeviceVolume(&l_name, l_vol);
			}
			// Determine action
			else if (l_cmds[2] == "setDefault")
			{
				// Determine state
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
			// Determine action
			if (l_cmds[2] == "mute")
			{
				// Determine state
				if (l_cmds[3] == "True")
				{
					m_volCntl->setMute(&l_name, true);
				}
				// Determine state
				else if (l_cmds[3] == "False")
				{
					m_volCntl->setMute(&l_name, false);
				}
				// Determine state
				else if (l_cmds[3] == "toggle")
				{
					m_volCntl->toggleMute(&l_name);
				}
			}
			// Determine action
			else if (l_cmds[2] == "volume")
			{
				float l_vol = 0;
				m_volCntl->getVolume(&l_name, &l_vol);
				l_vol += (stoi(l_cmds[3]) / 100);
				m_volCntl->setVolume(&l_name, l_vol);
			}
		}
	}
	
}

inline std::wstring Parser::convert(std::string i_str)
{
	return std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(i_str);
}

void Parser::removeCharsFromString(std::string& str, char* charsToRemove) {
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
}