#pragma once
#include "gui.h"

// Used to edit stuff.
class Editor : public LayerFrame
{
public:
	Editor();
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
	// The frame that displays the chunk.
	WorldOrthographicFrame* m_Frame;

	// The frame that displays the UI.
	UIFrame* m_UI;

	// The currently activated tool.
	ChunkTool* m_Tool;

public:
	ChunkEditor();

	void display();
};