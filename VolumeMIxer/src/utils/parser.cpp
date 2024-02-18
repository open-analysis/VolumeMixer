#include "Parser.h"

Parser::Parser()
{
	std::cout << "Don't use this constructor" << std::endl;
	m_audioCntl = nullptr;
	m_devCntl = nullptr;
	c_lf_char = 10;
	m_util = Utils();
}

Parser::Parser(AudioControl* i_audioCntl, DeviceControl* i_devCntl) :
	m_audioCntl(i_audioCntl), m_devCntl(i_devCntl)
{
	m_util = Utils();
	c_lf_char = 10;
}

void Parser::parseQueue(const std::string i_direction, const std::string i_type)
{

	if (m_queue[0][0] == c_lf_char)
		return;

	constexpr char c_cmd_delim = '$';

	for (auto t_queue : m_queue)
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

		// Check if the cmds direction & type are correct
		//  The direction is handled outside of this function
		//  ie by setting Audio/DeviceControl 
		if (l_cmds[0] != i_type || l_cmds[2] != i_direction) continue;

		// Determine type that's being operated on
		if (i_type == "device")
		{
			parseCmdDevice(l_cmds);
		}
		else
		{
			parseCmdPrograms(l_cmds);
		}
	}
	
}

void Parser::setQueue(char* i_buffer)
{
	constexpr char c_queue_delim = ',';

	std::string l_str = (std::string)i_buffer;
	char c_remove_chars[] = "[\"],";
	size_t l_queue_start = 0;
	size_t l_queue_end = 0;

	// Break up the queue into each separate command
	while ((l_queue_start = l_str.find_first_not_of(c_queue_delim, l_queue_end)) != std::string::npos)
	{
		l_queue_end = l_str.find(c_queue_delim, l_queue_start);
		m_queue.push_back(l_str.substr(l_queue_start, (l_queue_end - l_queue_start)));
	}

	for (unsigned int i = 0; i < m_queue.size(); i++)
	{
		removeCharsFromString(m_queue[i], c_remove_chars);
	}
}

std::string Parser::device2Json(const AudioDevice& i_audio_device)
{
	std::string r_return = "";

	r_return = m_JSON_SECTIONS[0]; // {

	r_return += m_JSON_SECTIONS[1]; // "type": 
	if (i_audio_device.m_is_output)
	{
		r_return += m_JSON_SECTIONS[3]; // "out",
	}
	else
	{
		r_return += m_JSON_SECTIONS[2]; // "in",
	}

	r_return += m_JSON_SECTIONS[4]; // "name": 
	r_return += "\"";
	r_return += i_audio_device.m_name;
	r_return += "\",";

	r_return += m_JSON_SECTIONS[5]; // "img": 
	r_return += "\"";
	r_return += i_audio_device.m_img; 
	r_return += "\",";

	r_return += m_JSON_SECTIONS[6]; // "default": 
	if (i_audio_device.m_is_default)
	{
		r_return += "true";
	}
	else
	{
		r_return += "false";
	}

	r_return += m_JSON_SECTIONS[7]; // "volume": 
	r_return += "\"";
	r_return += i_audio_device.m_volume; 
	r_return += "\"";

	r_return += m_JSON_SECTIONS[8]; // "mute": 
	r_return += "\"";
	r_return += i_audio_device.m_mute; 
	r_return += "\"";


	r_return += m_JSON_SECTIONS[9]; // }
	
	return r_return;
}

std::string Parser::program2Json(const Audio& i_audio)
{
	std::string r_return = "";

	r_return = m_JSON_SECTIONS[0]; // {
	
	r_return += m_JSON_SECTIONS[4]; // "name": 
	r_return += "\"";
	r_return += i_audio.m_name;
	r_return += "\",";

	r_return += m_JSON_SECTIONS[5]; // "img": 
	r_return += "\"";
	r_return += i_audio.m_img; 
	r_return += "\"";

	r_return += m_JSON_SECTIONS[7]; // "volume": 
	r_return += "\"";
	r_return += i_audio.m_volume; 
	r_return += "\"";

	r_return += m_JSON_SECTIONS[8]; // "mute": 
	r_return += "\"";
	r_return += i_audio.m_mute; 
	r_return += "\"";

	r_return += m_JSON_SECTIONS[9]; // }

	return r_return;
}

void Parser::parseCmdDevice(std::vector<std::string> i_cmds)
{
	// Get the name of the device/program
	std::wstring l_name = m_util.convertStr2Wstr(i_cmds[1]);


	// Need to clean up the end of the command some
	removeCharsFromString(i_cmds[3], &c_lf_char);

	// Determine action
	if (i_cmds[3] == "mute")
	{
		// Need to clean up the end of the command some
		removeCharsFromString(i_cmds[4], &c_lf_char);

		// Determine state
		if (i_cmds[4] == "True")
		{
			m_devCntl->setMute(&l_name, true);
		}
		// Determine state
		else if (i_cmds[4] == "False")
		{
			m_devCntl->setMute(&l_name, false);
		}
		// Determine state
		else if (i_cmds[4] == "toggle")
		{
			m_devCntl->toggleMute(&l_name);
		}
	}
	// Determine action
	else if (i_cmds[3] == "volume")
	{
		float l_vol = 0;
		m_devCntl->getVolume(&l_name, &l_vol);
		l_vol += (float)stoi(i_cmds[4]) / 100;
		m_devCntl->setVolume(&l_name, l_vol);
	}
	// Determine action
	else if (i_cmds[3] == "setDefault")
	{
		// Determine state
		if (i_cmds[2] == "out")
		{
			m_devCntl->setDefaultEndpoint(&l_name, ERole::eConsole);
		}
		else
		{
			m_devCntl->setDefaultEndpoint(&l_name, ERole::eCommunications);
		}
	}
}

void Parser::parseCmdPrograms(std::vector<std::string> i_cmds)
{
	// Get the name of the device/program
	std::wstring l_name = m_util.convertStr2Wstr(i_cmds[1]);

	// Determine action
	if (i_cmds[3] == "mute")
	{
		// Need to clean up the end of the command some
		removeCharsFromString(i_cmds[4], &c_lf_char);

		// Determine state
		if (i_cmds[4] == "True")
		{
			m_audioCntl->setMute(&l_name, true);
		}
		// Determine state
		else if (i_cmds[4] == "False")
		{
			m_audioCntl->setMute(&l_name, false);
		}
		// Determine state
		else if (i_cmds[4] == "toggle")
		{
			m_audioCntl->toggleMute(&l_name);
		}
	}
	// Determine action
	else if (i_cmds[3] == "volume")
	{
		float l_vol = 0;
		m_audioCntl->getVolume(&l_name, &l_vol);
		l_vol += (float)stoi(i_cmds[4]) / 100;
		m_audioCntl->setVolume(&l_name, l_vol);
	}
}

void Parser::removeCharsFromString(std::string& str, char* charsToRemove) {
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
}