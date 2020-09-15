#pragma once

#include "font.h"
#include "../event.h"


namespace onion
{


	class Frame : public Graphic
	{
	protected:
		// The parent of the frame.
		Frame* m_Parent;

		// The frame boundaries on the screen
		mat2x2i m_Bounds;

		/// <summary>Adjusts the position and dimensions of the frame.</summary>
		virtual void __set_bounds();

		/// <summary>Displays the contents of the frame.</summary>
		virtual void __display() const;

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

		/// <summary>Displays the frame.</summary>
		virtual void display() const;
	};



	// A frame that 
	class Button : public Frame, public MouseMoveListener, public MousePressListener
	{
	protected:
		// Whether the button is highlighted or not.
		bool m_Highlighted = false;

		/// <summary>A function that triggers when the button is highlighted. Does nothing by default.</summary>
		virtual void highlight();

		/// <summary>A function that triggers when the button is unhighlighted. Does nothing by default.</summary>
		virtual void unhighlight();

		/// <summary>A function that triggers when the button is pressed. Does nothing by default.</summary>
		virtual void click();

	public:
		/// <summary>Triggers when the mouse cursor is moved.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(const MouseMoveEvent& event_data);

		/// <summary>Triggers when a mouse button is pressed.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(const MousePressEvent& event_data);

		/// <summary>Freezes reaction to inputs.</summary>
		virtual void freeze();

		/// <summary>Unfreezes reaction to inputs.</summary>
		virtual void unfreeze();
	};


	// A frame that takes and displays a line of text input
	class TextInput : public Frame, public MousePressListener, public KeyboardListener
	{
	protected:
		// Whether the frame is receiving text inputs or not.
		bool m_TextFrozen = true;

		// The text being inputted.
		std::string m_TextInput;

		// The position of the cursor.
		int m_Cursor = 0;

		// The graphic that displays where the cursor is
		SolidColorGraphic* m_CursorGraphic;

	public:
		/// <summary>Constructs the TextInput object.</summary>
		TextInput();

		/// <summary>Destroys the TextInput object.</summary>
		~TextInput();

		/// <summary>Retrieves the text input.</summary>
		/// <returns>The text inputted by the user.</returns>
		std::string get_input() const;

		/// <summary>Retrieves the font.</summary>
		virtual Font* get_font() const = 0;

		/// <summary>Retrieves the font palette.</summary>
		virtual const Palette* get_font_palette() const = 0;

		/// <summary>Triggers when a mouse button is pressed.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(const MousePressEvent& event_data);

		/// <summary>Triggers when a mouse button is pressed.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(const UnicodeEvent& event_data);

		/// <summary>Freezes reaction to inputs.</summary>
		virtual void freeze();

		/// <summary>Unfreezes reaction to inputs.</summary>
		virtual void unfreeze();

		/// <summary>Displays the text input.</summary>
		virtual void display() const;
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


	class ScrollableFrame : public Frame, public UpdateListener
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

		/// <summary>Updates the frame.</summary>
		/// <param name="frames_passed">The number of frames that have passed since the last update.</param>
		void __update(int frames_passed);

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



}