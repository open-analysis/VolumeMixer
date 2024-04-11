#include "Parser.h"

Parser::Parser()
{
	std::cout << "Parser: Don't use this constructor" << std::endl;
	m_audioCntl = nullptr;
	m_devCntl = nullptr;
	m_audio_programs = nullptr;
	m_audio_devices = nullptr;
	c_lf_char = 10;
	m_util = Utils();
}

Parser::Parser(AudioControl* i_audioCntl, DeviceControl* i_devCntl, std::vector<Audio>* i_audio_programs, std::vector<AudioDevice>* i_audio_devices) :
	m_audioCntl(i_audioCntl), m_devCntl(i_devCntl), m_audio_programs(i_audio_programs), m_audio_devices(i_audio_devices)
{
	m_util = Utils();
	c_lf_char = 10;
}

void Parser::parseQueue()
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

		// Determine type that's being operated on
		if (l_cmds[0] == "device")
		{
			parseCmdDevice(l_cmds);
		}
		else
		{
			parseCmdPrograms(l_cmds);
		}
	}
	
}

void Parser::setQueue(const char* i_buffer)
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
	r_return += ",";

	r_return += m_JSON_SECTIONS[7]; // "volume": 
	r_return += std::to_string(i_audio_device.m_volume);
	r_return += ",";

	r_return += m_JSON_SECTIONS[8]; // "mute": 
	r_return += std::to_string(i_audio_device.m_mute);


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
	r_return += "\",";

	r_return += m_JSON_SECTIONS[7]; // "volume": 
	r_return += std::to_string(i_audio.m_volume);
	std::cout << "Program Volume: " << i_audio.m_volume << std::endl;
	r_return += ",";

	r_return += m_JSON_SECTIONS[8]; // "mute": 
	r_return += std::to_string(i_audio.m_mute);
	std::cout << "Program Mute: " << i_audio.m_mute << std::endl;

	r_return += m_JSON_SECTIONS[9]; // }

	return r_return;
}

// Queue structure type$name$volume$mute$default$output
void Parser::parseCmdDevice(std::vector<std::string> i_cmds)
{
	for (auto l_device : *m_audio_devices)
	{
		if (l_device.m_name == i_cmds[1])
		{
			// Get the name of the device/program
			std::wstring l_name = m_util.convertStr2Wstr(i_cmds[1]);

			// Set volume
			float l_vol = 0;
			l_vol = (float)(stoi(i_cmds[2]) / 100);
			m_devCntl->setVolume(&l_name, l_vol);
			l_device.m_volume = stoi(i_cmds[2]);

			// Set mute
			// Need to clean up the end of the command some
			removeCharsFromString(i_cmds[3], &c_lf_char);

			// Determine mute state
			if (i_cmds[3] == "True")
			{
				m_devCntl->setMute(&l_name, true);
				l_device.m_mute = true;
			}
			else if (i_cmds[3] == "False")
			{
				m_devCntl->setMute(&l_name, false);
				l_device.m_mute = false;
			}

			// Set Default
			// Need to clean up the end of the command some
			removeCharsFromString(i_cmds[4], &c_lf_char);  // Default
			// Only update if this is true
			if (i_cmds[4] == "True")
			{
				removeCharsFromString(i_cmds[5], &c_lf_char);  // Output
				if (i_cmds[5] == "out")
				{
					m_devCntl->setDefaultEndpoint(&l_name, ERole::eConsole);
				}
				else
				{
					m_devCntl->setDefaultEndpoint(&l_name, ERole::eCommunications);
				}
			}

			break;
		}
	}
}

// Queue structure type$name$volume$mute
void Parser::parseCmdPrograms(std::vector<std::string> i_cmds)
{
	for (auto l_program : *m_audio_programs)
	{
		if (l_program.m_name == i_cmds[1])
		{
			// Get the name of the device/program
			std::wstring l_name = m_util.convertStr2Wstr(i_cmds[1]);

			// Set volume
			float l_vol = 0;
			l_vol = (float)stoi(i_cmds[4]) / 100;
			m_audioCntl->setVolume(&l_name, l_vol);
			l_program.m_volume = stoi(i_cmds[4]);

			// Set mute
			// Need to clean up the end of the command some
			removeCharsFromString(i_cmds[4], &c_lf_char);

			// Determine state
			if (i_cmds[3] == "True")
			{
				m_audioCntl->setMute(&l_name, true);
				l_program.m_mute = true;
			}
			// Determine state
			else if (i_cmds[3] == "False")
			{
				m_audioCntl->setMute(&l_name, false);
				l_program.m_mute = false;
			}

			break;
		}
	}

}

void Parser::removeCharsFromString(std::string& str, char* charsToRemove) {
	for (unsigned int i = 0; i < strlen(charsToRemove); ++i) {
		str.erase(remove(str.begin(), str.end(), charsToRemove[i]), str.end());
	}
}