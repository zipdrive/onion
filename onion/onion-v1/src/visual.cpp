#include "..\include\onion.h"
#include <GLFW\glfw3.h>

/***

CLASSES

***/

class ONIONtex2D
{
protected:
	GLFWimage* m_Texture;

public:
	ONIONtex2D(const char* path);
};

class ONIONtex2Dvisual : public ONIONvisual
{

};



/***

CLASS METHODS

***/

ONIONtex2D::ONIONtex2D(const char* path)
{
	// Load image from path
		// TODO
}



/***

EXPORTED METHODS

***/

ONIONtex2D* onionLoadImage(const char* path)
{
	return new ONIONtex2D(path);
}