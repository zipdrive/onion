#pragma once

#include "graphics.h"
#include "event.h"
#include "world.h"


class Frame : public Graphic
{
protected:
	// The parent of the frame.
	Frame* m_Parent;

	// The frame boundaries on the screen
	mat2x2i m_Bounds;

public:
	/// <summary>Creates an empty frame.</summary>
	Frame();

	/// <summary>Creates an empty frame with the given boundaries.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	Frame(int x, int y, int width, int height);

	/// <summary>Retrieves the boundaries of the frame on the screen.</summary>
	/// <returns>A const reference to the boundaries. The first column represents minimum values, and the second column represents maximum values.</returns>
	const mat2x2i& get_bounds() const;

	/// <summary>Retrieves the absolute boundaries of the frame.</summary>
	/// <returns>The absolute boundaries of the frame.</returns>
	mat2x2i get_absolute_bounds() const;

	/// <summary>Sets the boundaries of the frame on the screen.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	virtual void set_bounds(int x, int y, int width, int height);

	/// <summary>Retrieves the width of the frame.</summary>
	/// <returns>The width of the frame.</returns>
	int get_width() const;

	/// <summary>Retrieves the height of the frame.</summary>
	/// <returns>The height of the frame.</returns>
	int get_height() const;

	/// <summary>Sets the parent of the frame.</summary>
	/// <param name="parent">The parent of the frame.</param>
	void set_parent(Frame* parent);
};



// A frame that allows a user to scroll between values.
class ScrollBar : public Frame, public MouseDraggableListener
{
private:
	// The background of the scroll area.
	Graphic* m_Background;

	// The left- or top-facing arrow.
	Graphic* m_Arrow;

	// The horizontal scroll object.
	Graphic* m_Scroller;

protected:
	// The value of the scroll bar.
	float m_Value;

	// Whether the scroll bar scrolls horizontally or vertically.
	bool m_Horizontal;

	/// <summary>Sets the center of the scroller.</summary>
	/// <param name="dx">The distance from the left side to the x-coordinate.</param>
	/// <param name="dy">The distance from the bottom side to the y-coordinate.</param>
	void set_center_of_scroller(int dx, int dy);

public:
	/// <summary>Constructs a scroll bar.</summary>
	/// <param name="background_graphic">A graphic to display as the background for the scrolling area.</param>
	/// <param name="arrow_graphic">A graphic to display as arrows on either side of the scrolling area.</param>
	/// <param name="scroll_graphic">A graphic to display to show the current value of the scroll bar.</param>
	/// <param name="x">The x-coordinate of the left side.</param>
	/// <param name="y">The y-coordinate of the bottom side.</param>
	/// <param name="horizontal">True if the scroll bar is horizontal, false if vertical.</param>
	ScrollBar(Graphic* background_graphic, Graphic* arrow_graphic, Graphic* scroll_graphic, int x, int y, bool horizontal);

	/// <summary>Retrieves the current value of the scroll bar.</summary>
	/// <returns>The current value of the scroll bar.</returns>
	float get_value();

	/// <summary>Sets the value of the scroll bar.</summary>
	/// <param name="value">The value for the scroll bar.</param>
	virtual void set_value(float value);

	/// <summary>Triggers in response to a mouse button being pressed.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MousePressEvent& event_data);

	/// <summary>Triggers in response to the mouse being moved.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MouseMoveEvent& event_data);

	/// <summary>Displays the contents of the frame.</summary>
	void display() const;
};


class ScrollableFrame : public Frame
{
protected:
	// The frame being scrolled
	Frame* m_Frame;

	// The horizontal scroll bar
	ScrollBar* m_HorizontalScrollBar;

	// The vertical scroll bar
	ScrollBar* m_VerticalScrollBar;

	// The distance available to scroll
	vec2i m_ScrollDistance;

public:
	/// <summary></summary>
	/// <param name="frame">The frame to be scrolled.</param>
	/// <param name="horizontal">The horizontal scroll bar. NULL if it is not horizontally scrolled.</param>
	/// <param name="vertical">The vertical scroll bar. NULL if it is not vertically scrolled.</param>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	ScrollableFrame(Frame* frame, ScrollBar* horizontal, ScrollBar* vertical, int x, int y, int width, int height);

	/// <summary>Displays the frame.</summary>
	void display() const;
};


class ListFrame : public Frame
{
public:
	
};



class LayerFrame : public Frame
{
protected:
	// A sequence of things to be displayed.
	std::vector<Graphic*> m_Sequence;

	// The transformation to apply before displaying.
	mat4x4f m_Transform;

	// The z-scale of the transformation.
	float m_ZScale;

	/// <summary>Resets the transformation data.</summary>
	virtual void reset();

public:
	/// <summary>Constructs an empty frame that fills the screen.</summary>
	LayerFrame();

	/// <summary>Constructs an empty frame with the given dimensions.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	LayerFrame(int x, int y, int width, int height);

	/// <summary>Adds a graphic to be displayed over existing graphics.</summary>
	/// <param name="graphic">The graphic to be displayed.</param>
	void insert_top(Graphic* graphic);

	/// <summary>Displays the contents of the frame.</summary>
	void display() const;
};

// A frame of layers that applies an orthographic projection before displaying.
class UIFrame : public LayerFrame
{
protected:
	/// <summary>Resets the transformation matrix.</summary>
	void reset();

public:
	/// <summary>Creates an empty UI frame that takes up the whole screen.</summary>
	UIFrame();
};




// A frame that displays the world orthographically.
class WorldOrthographicFrame : public Frame, public MouseDraggableListener
{
public:
	class Tool
	{
	protected:
		/// <summary>Determines which tile is being highlighted.</summary>
		/// <param name="x">The in-world x-coordinate being highlighted.</param>
		/// <param name="y">The in-world y-coordinate being highlighted.</param>
		/// <returns>The coordinates of the tile being highlighted.</returns>
		vec2i get_tile(int dx, int dy);

		/// <summary>Determines which object is being highlighted.</summary>
		/// <param name="x">The in-world x-coordinate being highlighted.</param>
		/// <param name="y">The in-world y-coordinate being highlighted.</param>
		/// <returns>The object being highlighted, or NULL if no objects are being highlighted.</returns>
		Object* get_object(int dx, int dy);

	public:
		/// <summary>Triggers in response to something being highlighted.</summary>
		/// <param name="x">The in-world x-coordinate being highlighted.</param>
		/// <param name="y">The in-world y-coordinate being highlighted.</param>
		virtual void highlight(int dx, int dy) = 0;

		/// <summary>Triggers in response to the highlighted tile being selected.</summary>
		virtual void select() = 0;
	};

private:
	/// <summary>
	/// A transform that does the following:
	/// Centers the view at the center of the frame.
	/// Orthographically projects (pixel-perfect).
	/// Centers the camera in world space.
	/// Rotates by -45 degrees around the x-axis, and scales up by sqrt(2).
	/// </summary>
	mat4x4f m_Transform;

	// The position of the camera
	vec2f m_Camera;

	// The boundaries of the area in world space being displayed.
	mat2x2i m_DisplayArea;

	// The number of tiles around the edges that should not be drawn.
	const int m_TileMargin;

	// The current chunk
	Chunk* m_Chunk;

	// The world interaction tool
	Tool* m_Tool;

	/// <summary>Resets the transformation.</summary>
	void reset();

	/// <summary>Clamps the display area to the boundaries of the chunk.</summary>
	void clamp_display_area();

public:
	/// <summary>Creates an orthographic world frame.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	/// <param name="tile_margin">The number of tiles around the edges that should not be drawn.</param>
	WorldOrthographicFrame(int x, int y, int width, int height, int tile_margin = 1);

	/// <summary>Sets the position of the camera.</summary>
	/// <param name="x">The x-coordinate of the camera.</param>
	/// <param name="y">The y-coordinate of the camera.</param>
	void set_camera(float x, float y);

	/// <summary>Adjusts the position of the camera.</summary>
	/// <param name="dx">The x-axis adjustment.</param>
	/// <param name="dy">The y-axis adjustment.</param>
	void adjust_camera(float dx, float dy);

	/// <summary>Displays the contents of the frame.</summary>
	void display() const;

	/// <summary>Sets the current tool.</summary>
	/// <param name="tool">The tool to use.</param>
	void set_tool(Tool* tool);

	/// <summary>Triggers in response to a mouse button being pressed.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MousePressEvent& event_data);

	/// <summary>Triggers in response to the mouse being moved.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MouseMoveEvent& event_data);
};