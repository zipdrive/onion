#pragma once

#include <vector>

#define OBJECT_KEY int


// 
class Object
{
public:
	/// <summary>Display the object.</summary>
	virtual void display() = 0;
};



/*
 *
 * FRAMES
 *
 */

// An object that contains other objects.
class Frame : public Object
{
public:
	/// <summary>Add an object to the frame.</summary>
	/// <param name="object">The object to add.</param>
	virtual void add(Object* object) = 0;

	/// <summary>Remove an object from the frame.</summary>
	/// <param name="object">The object to remove.</param>
	virtual void remove(Object* object) = 0;
};

// A collection of objects, ordered from back to front.
class LayeredFrame : public Frame
{
private:
	// The collection of objects.
	std::vector<Object*> m_Objects;

public:
	void add(Object* object);
	void remove(Object* object);
	void display();
};