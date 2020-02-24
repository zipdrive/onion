#pragma once

#include <vector>

#define OBJECT_KEY int


// A thing within the game world.
class Object
{
public:
	/// <summary>Display the object.</summary>
	virtual void display() = 0;
};