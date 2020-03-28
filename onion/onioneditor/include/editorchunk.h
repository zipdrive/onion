#pragma once

#include "editor.h"


class WorldScrollBar;



// A tool used to edit the chunks.
class ChunkTool : public WorldOrthographicFrame::Tool
{
public:
	/// <summary>Displays the toolbar.</summary>
	virtual void display() = 0;

	/// <summary>Freezes input for the tool.</summary>
	virtual void freeze() = 0;

	/// <summary>Unfreezes input for the tool.</summary>
	virtual void unfreeze() = 0;
};



class PaintSelectFrame : public Frame
{
private:
	// The sprite sheet of tiles
	SpriteSheet* m_TileSpriteSheet;

	SPRITE_KEY m_PaintKey;

	int m_NumberTiles;

	int m_NumberTilesPerRow;

	int m_NumberRows;

	int m_Margin;

	Graphic* m_Highlight;

public:
	PaintSelectFrame(int width);

	SPRITE_KEY get_paint_key() const;

	void display() const;

	void select(int dx, int dy);
};

// A tool that paints tiles
class PaintChunkTool : public ChunkTool, public MousePressListener
{
private:
	// The frame for the toolbar
	ScrollableFrame* m_ToolbarFrame;

	// The scroll bar
	ScrollBar* m_VerticalScrollBar;

	// The frame to choose a tile to paint
	PaintSelectFrame* m_ToolbarSelectFrame;

	// The highlighted tile
	vec2i m_Highlight;

public:
	PaintChunkTool();

	/// <summary>Displays the paint toolbar.</summary>
	void display();

	/// <summary>Triggers in response to a mouse button being pressed.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MousePressEvent& event_data);

	/// <summary>Freezes input for the tool.</summary>
	void freeze();

	/// <summary>Unfreezes input for the tool.</summary>
	void unfreeze();

	/// <summary>Triggers in response to something being highlighted.</summary>
	/// <param name="x">The in-world x-coordinate being highlighted.</param>
	/// <param name="y">The in-world y-coordinate being highlighted.</param>
	void highlight(int dx, int dy);

	/// <summary>Triggers in response to the highlighted tile being selected.</summary>
	void select();
};




// Used for editing chunks.
class ChunkEditor : public Editor
{
private:
	// The frame for the world.
	WorldOrthographicFrame* m_WorldFrame;

	// The horizontal scroll bar for the world frame.
	WorldScrollBar* m_HorizontalScrollBar;

	// The vertical scroll bar for the world frame.
	WorldScrollBar* m_VerticalScrollBar;

	// The corner between the scroll bars.
	Graphic* m_ScrollBarCorner;

	// The background for the toolbar
	Graphic* m_ToolbarBackground;

	// The currently activated tool.
	ChunkTool* m_Tool;

public:
	ChunkEditor();

	void set_tool(ChunkTool* tool);

	/// <summary>Displays the chunk editor.</summary>
	void display();

	/// <summary>Freezes input for the editor.</summary>
	void freeze();

	/// <summary>Unfreezes input for the editor.</summary>
	void unfreeze();
};