#pragma once
#include "gui.h"

// Used to edit stuff.
class Editor
{
public:
	/// <summary>Displays the editor.</summary>
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
class ChunkEditor : public Editor
{
private:
	// The frame for the world.
	WorldOrthographicFrame m_WorldFrame;

	// The frame for the UI.
	UIFrame m_UIFrame;

	// The currently activated tool.
	ChunkTool* m_Tool;

public:
	ChunkEditor();

	/// <summary>Displays the chunk editor.</summary>
	void display();
};