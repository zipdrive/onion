#include <regex>
#include "../include/onions/fileio.h"

using namespace std;

namespace onion
{

	bool StringData::get(String key, Boolean& value) const
	{
		string string_value;
		if (get(key, string_value))
		{
			if (string_value.compare("true") == 0)
			{
				value = true;
				return true;
			}
			else if (string_value.compare("false") == 0)
			{
				value = false;
				return true;
			}
		}
		return false;
	}

	bool StringData::get(String key, Int& value) const
	{
		String string_value;
		if (get(key, string_value))
		{
			regex pattern("(\\d+)");
			if (regex_match(string_value, pattern))
			{
				value = stoi(string_value);
				return true;
			}
		}
		return false;
	}

	bool StringData::get(String key, Float& value) const
	{
		string string_value;
		if (get(key, string_value))
		{
			regex pattern("(\\d*(\\.\\d*)?)");
			if (regex_match(string_value, pattern) && !string_value.empty())
			{
				value = stof(string_value);
				return true;
			}
		}
		return false;
	}

	bool StringData::get(String key, INT_VEC2& value) const
	{
		String str;
		if (get(key, str))
		{
			regex pattern("\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)");

			smatch match;
			if (regex_match(str, match, pattern))
			{
				value(0) = stoi(match[1].str());
				value(1) = stoi(match[2].str());
				return true;
			}
		}
		return false;
	}
	
	bool StringData::get(String key, INT_VEC3& value) const
	{
		String str;
		if (get(key, str))
		{
			regex pattern("\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*\\)");

			smatch match;
			if (regex_match(str, match, pattern))
			{
				value(0) = stoi(match[1].str());
				value(1) = stoi(match[2].str());
				value(2) = stoi(match[3].str());

				return true;
			}
		}
		return false;
	}

	bool StringData::get(String key, FLOAT_VEC3& value) const
	{
		String str;
		if (get(key, str))
		{
			regex pattern("\\(\\s*(\\S+)\\s*,\\s*(\\S+)\\s*,\\s*(\\S+)\\s*\\)");

			smatch match;
			if (regex_match(str, match, pattern))
			{
				value(0) = stof(match[1].str());
				value(1) = stof(match[2].str());
				value(2) = stof(match[3].str());

				return true;
			}
		}
		return false;
	}

	void StringData::set(String key, Boolean value)
	{
		set(key, value ? "true" : "false");
	}

	void StringData::set(String key, Int value)
	{
		set(key, to_string(value));
	}

	void StringData::set(String key, Float value)
	{
		set(key, to_string(value));
	}

	void StringData::set(String key, const INT_VEC2& value)
	{
		set(key, "(" + to_string(value.get(0)) + "," + to_string(value.get(1)) + ")");
	}

	void StringData::set(String key, const INT_VEC3& value)
	{
		set(key, "(" + to_string(value.get(0)) + "," + to_string(value.get(1)) + "," + to_string(value.get(2)) + ")");
	}

	void StringData::set(String key, const FLOAT_VEC3& value)
	{
		set(key, "(" + to_string(value.get(0)) + "," + to_string(value.get(1)) + "," + to_string(value.get(2)) + ")");
	}


	SaveFile::SaveFile(string path) : m_File(path) {}

	SaveFile::~SaveFile()
	{
		m_File.close();
	}

	void SaveFile::save_int_binary(int16_t value)
	{
		if (!m_File.good()) return;

		char buffer[2];
		buffer[0] = value % 0x100;
		buffer[1] = value / 0x100;

		m_File.write(buffer, 2);
	}

	void SaveFile::save_string_binary(string value)
	{
		save_int_binary(value.size());
		m_File.write(value.c_str(), value.size());
	}

	void SaveFile::save_data(String id, const StringData& data)
	{
		if (data.m_Data.size() == 1)
		{
			if (!id.empty())
				m_File << id << "\t";
			auto iter = data.m_Data.begin();
			m_File << iter->first << " = \"" << iter->second << "\"\n\n";
		}
		else
		{
			m_File << "begin " << id << "\n";
			for (auto iter = data.m_Data.begin(); iter != data.m_Data.end(); ++iter)
				m_File << "\t" << iter->first << " = \"" << iter->second << "\"\n";
			m_File << "end\n\n";
		}
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

	int16_t LoadFile::load_int_binary()
	{
		if (!m_File.good()) return 0;

		char buffer[2];
		m_File.read(buffer, 2);

		return buffer[0] | ((int16_t)buffer[1] << 8);
	}

	string LoadFile::load_string_binary()
	{
		int len = load_int_binary();
		if (len == 0 || !m_File.good()) return string();

		char* buffer = new char[len + 1];
		m_File.read(buffer, len);
		buffer[len] = '\0';

		return string(buffer);
	}

	String LoadFile::load_line()
	{
		string line;
		if (getline(m_File, line))
			return line;
		return "";
	}

	String LoadFile::load_data(IntegerData& data)
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

	String LoadFile::load_data(StringData& data)
	{
		string line;
		regex begin_regex("^begin\\s+(.*)$");
		regex enclosed_regex("^((.*\\S)\\s)?\\s*(\\S+)\\s*=\\s*\"(.*)\"\\s*$");
		regex unenclosed_regex("^((.*\\S)\\s)?\\s*(\\S+)\\s*=\\s*(.+)\\s*$");

		if (getline(m_File, line))
		{
			smatch match;
			if (regex_match(line, match, begin_regex))
			{
				string id = match[1].str();
				while (getline(m_File, line))
				{
					if (line.compare("end") == 0)
						break;

					while (regex_match(line, match, enclosed_regex) || regex_match(line, match, unenclosed_regex))
					{
						data.set(match[3].str(), match[4].str());
						line = match[2].str();
					}
				}
				line = id;
			}
			else
			{
				while (regex_match(line, match, enclosed_regex) || regex_match(line, match, unenclosed_regex))
				{
					data.set(match[3].str(), match[4].str());
					line = match[2].str();
				}
			}
		}

		return line;
	}
}

