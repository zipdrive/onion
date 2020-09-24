#include <regex>
#include "../include/onions/fileio.h"

using namespace std;

namespace onion
{

	bool _StringData::get(string key, int& value) const
	{
		string v;
		if (get(key, v))
		{
			value = stoi(v);
			return true;
		}
		return false;
	}

	void _StringData::set(string key, const int& value)
	{
		set(key, to_string(value));
	}


	SaveFile::SaveFile(string path) : m_File(path) {}

	SaveFile::~SaveFile()
	{
		m_File.close();
	}

	void SaveFile::save_int(int16_t value)
	{
		if (!m_File.good()) return;

		char buffer[2];
		buffer[0] = value % 0x100;
		buffer[1] = value / 0x100;

		m_File.write(buffer, 2);
	}

	void SaveFile::save_string(string value)
	{
		save_int(value.size());
		m_File.write(value.c_str(), value.size());
	}


	LoadFile::LoadFile(string path) : m_File(path) {}

	LoadFile::~LoadFile()
	{
		m_File.close();
	}

	bool LoadFile::good()
	{
		return m_File.good();
	}

	int16_t LoadFile::load_int()
	{
		if (!m_File.good()) return 0;

		char buffer[2];
		m_File.read(buffer, 2);

		return buffer[0] | ((int16_t)buffer[1] << 8);
	}

	string LoadFile::load_string()
	{
		int len = load_int();
		if (len == 0 || !m_File.good()) return string();

		char* buffer = new char[len + 1];
		m_File.read(buffer, len);
		buffer[len] = '\0';

		return string(buffer);
	}

	string LoadFile::load_line()
	{
		string line;
		if (getline(m_File, line))
			return line;
		return "";
	}

	string LoadFile::load_data(_IntegerData& data)
	{
		string line;
		regex line_data("((.*\\S)\\s)?\\s*(\\S+)\\s*=\\s*(-?\\d+)");

		if (getline(m_File, line))
		{
			smatch match;
			while (regex_match(line, match, line_data))
			{
				data.set(match[3].str(), stoi(match[4].str()));
				line = match[2].str();
			}
		}

		return line;
	}

	string LoadFile::load_data(_StringData& data)
	{
		string line;
		regex line_data("((.*\\S)\\s)?\\s*(\\S+)\\s*=\\s*\"(.*)\"\\s*$");

		if (getline(m_File, line))
		{
			smatch match;
			while (regex_match(line, match, line_data))
			{
				data.set(match[3].str(), match[4].str());
				line = match[2].str();
			}
		}

		return line;
	}
}

