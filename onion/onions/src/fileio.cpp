#include "../include/onions/fileio.h"

using namespace std;

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