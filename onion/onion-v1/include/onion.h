#pragma once
#include <vector>

#ifdef ONION_EXPORTS
#define ONION_API __declspec(dllexport)
#else
#define ONION_API __declspec(dllimport)
#endif

#define ONION_BLOCK 0
#define ONION_PASS 1



/***

VISUALS

***/


// A thing that gets drawn to the screen.
ONION_API class ONIONvisual
{
public:
	/*
	Draws the thing to the buffer.
	*/
	virtual void draw() = 0;
};

ONION_API class ONIONtex2D;

// Loads the image, and creates a ONIONtex2D instance that displays that image.
ONION_API ONIONtex2D* onionLoadImage(const char* path);

ONION_API ONIONvisual* onionCreateVisual(ONIONtex2D* tex2D, double left, double top, double right, double bottom);
ONION_API ONIONvisual* onionCreateVisual(ONIONtex2D* tex2D, int left, int top, int right, int bottom);



/***

OBJECTS

***/


ONION_API struct ONIONvec2
{
	int x;
	int y;
};

ONION_API struct ONIONvec3 : public ONIONvec2
{
	int z;
};


// A thing within the game.
ONION_API class ONIONobject
{
protected:
	ONIONvisual* m_Visual;

public:
	/*
	Draws the object to the buffer.
	*/
	void draw();

	/*
	Updates the state of the object in response to an event.
	Returns ONION_PASS if the event should be passed to the next object in line, and ONION_BLOCK otherwise.
	*/
	virtual int update();
};

// An object that consists of layers of other objects.
ONION_API class ONIONlayerObject : ONIONobject
{
protected:
	std::vector
};



/***

CRITICAL FUNCTIONS

***/


// Initializes the Onion library. Call this function before using anything from the Onion library.
ONION_API int onionInit(const char* title, ONIONobject* gameObject);

// Runs the main loop of the Onion library.
ONION_API void onionMain();