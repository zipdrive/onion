#pragma once
#include <onion.h>

// Used to edit stuff.
class Editor
{
public:
	virtual void display() = 0;
};


/*
*
*	Chunk editing
*
*/

// A tool used to edit the chunks.
class ChunkTool
{
public:
	virtual void display() = 0;
};

// Used for editing chunks.
class ChunkEditor : Editor
{
private:
	// The chunk being edited.
	Chunk* m_Chunk;

	// The currently activated tool.
	ChunkTool* m_Tool;

public:
	void display();
};