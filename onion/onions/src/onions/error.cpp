#include <fstream>
#include "../../include/onions/error.h"

using namespace std;

namespace onion
{

	void errlog(const char* message)
	{
		ofstream file("log.txt", ios::app);
		file << message;
		file.close();
	}

	void errlog(string message)
	{
		ofstream file("log.txt", ios::app);
		file << message;
		file.close();
	}


	void errabort(int errcode)
	{
		// TODO
	}

}