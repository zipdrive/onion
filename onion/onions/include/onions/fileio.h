#pragma once

#include <fstream>
#include <string>

class SaveFile
{
private:
	// The file
	std::ofstream m_File;

public:
	/// <summary>Opens the file.</summary>
	/// <param name="path">The path to the file.</summary>
	SaveFile(std::string path);

	/// <summary>Closes down the file.</summary>
	~SaveFile();

	/// <summary>Saves an integer to file.</summary>
	/// <param name="value">An integer to save.</returns>
	void save_int(int16_t value);

	/// <summary>Saves a string to file.</summary>
	/// <returns>A string to save.</returns>
	void save_string(std::string value);
};

class LoadFile
{
private:
	// The file
	std::ifstream m_File;

public:
	/// <summary>Loads the file.</summary>
	/// <param name="path">The path to the file.</summary>
	LoadFile(std::string path);

	/// <summary>Closes down the file.</summary>
	~LoadFile();

	/// <summary>Returns whether the file is still good for loading or not.</summary>
	/// <returns>True if the file is good, false otherwise.</returns>
	bool good();

	/// <summary>Loads an integer from file.</summary>
	/// <returns>An integer from the file.</returns>
	int16_t load_int();

	/// <summary>Loads a string from file.</summary>
	/// <returns>A string from the file.</returns>
	std::string load_string();
};