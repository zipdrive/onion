#pragma once

#include <fstream>
#include <string>
#include <unordered_map>


namespace onion
{


	template <typename _Key, typename _Value>
	class _Data
	{
	private:
		// The data loaded from a line in a file.
		std::unordered_map<_Key, _Value> m_Data;

	public:
		/// <summary>Retrieves the value associated with a key.</summary>
		/// <param name="key">The key to retrieve the value of.</param>
		/// <param name="value">Outputs the value associated with the given key.</param>
		/// <returns>True if the key exists, false otherwise.</returns>
		bool get(_Key key, _Value& value) const
		{
			auto iter = m_Data.find(key);
			if (iter != m_Data.end())
			{
				value = iter->second;
				return true;
			}
			return false;
		}

		/// <summary>Sets the value to the key.</summary>
		/// <param name="key">The key to set the value of.</param>
		/// <param name="value">The value to assign to the given key.</param>
		void set(_Key key, const _Value& value)
		{
			auto iter = m_Data.find(key);
			if (iter != m_Data.end())
			{
				m_Data.erase(iter);
				m_Data.emplace_hint(iter, key, value);
			}
			else
			{
				m_Data.emplace(key, value);
			}
		}
	};

	class _StringData : public _Data<std::string, std::string>
	{
	public:
		using _Data<std::string, std::string>::get;

		/// <summary>Retrieves the value associated with a key.</summary>
		/// <param name="key">The key to retrieve the value of.</param>
		/// <param name="value">Outputs the value associated with the given key, converted to integer.</param>
		/// <returns>True if the key exists, false otherwise.</returns>
		bool get(std::string key, int& value) const;

		using _Data<std::string, std::string>::set;

		/// <summary>Sets the value to the key.</summary>
		/// <param name="key">The key to set the value of.</param>
		/// <param name="value">The value to assign to the given key.</param>
		void set(std::string key, const int& value);
	};

	typedef _Data<std::string, int> _IntegerData;


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

		/// <summary>Loads an integer from a dense file.</summary>
		/// <returns>An integer from the file.</returns>
		int16_t load_int();

		/// <summary>Loads a string from a dense file.</summary>
		/// <returns>A string from the file.</returns>
		std::string load_string();

		/// <summary>Loads a line from the file.</summary>
		/// <returns>The next line of the file, as a string.</returns>
		std::string load_line();

		/// <summary>Loads a line of key-value pairs from file, when stored in the format "prefix  key1 = value1  key2 = value2  ..."</summary>
		/// <param name="data">A reference to where the key-value pairs will be stored. Keys cannot include spaces.</param>
		/// <returns>The prefix for the line, which does not correspond to a key or value.</returns>
		std::string load_data(_IntegerData& data);

		/// <summary>Loads a line of key-string pairs from file, when stored in the format "prefix  key1 = "string1"  key2 = "string2"  ..."</summary>
		/// <param name="data">A reference to where the key-string pairs will be stored. Keys cannot include spaces.</param>
		/// <returns>The prefix for the line, which does not correspond to a key or value.</returns>
		std::string load_data(_StringData& data);
	};



}