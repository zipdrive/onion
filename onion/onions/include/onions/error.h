#pragma once
#include <string>

namespace onion
{


	/// <summary>Logs an error message.</summary>
	/// <param name="message">The error message to log.</param>
	void errlog(const char* message);

	/// <summary>Logs an error message.</summary>
	/// <param name="message">The error message to log.</param>
	void errlog(std::string message);



#define ERROR_FAILED_TO_COMPILE_VERTEX_SHADER 0x1
#define ERROR_FAILED_TO_COMPILE_FRAGMENT_SHADER 0x2
#define ERROR_FAILED_TO_LINK_SHADER_PROGRAM 0x3


	/// <summary>Displays an error message and aborts the application.</summary>
	/// <param name="errcode">The error code to display.</param>
	void errabort(int errcode);


}