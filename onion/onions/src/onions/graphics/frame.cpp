#include <algorithm>
#include "../../../include/onions/graphics/frame.h"
#include "../../../include/onions/application.h"

using namespace std;

namespace onion
{

	Frame::Frame()
	{
		m_Parent = nullptr;
	}

	Frame::Frame(int x, int y, int z, int width, int height, int depth)
	{
		m_Parent = nullptr;
		set_bounds(x, y, z, width, height, depth);
	}

	const mat3x2i& Frame::get_bounds() const
	{
		return m_Bounds;
	}

	mat3x2i Frame::get_absolute_bounds() const
	{
		if (m_Parent)
		{
			mat3x2i pbounds = m_Parent->get_absolute_bounds();
			int px = pbounds.get(0, 0);
			int py = pbounds.get(1, 0);
			int pz = pbounds.get(2, 0);

			return mat3x2i(m_Bounds.get(0, 0) + px, m_Bounds.get(0, 1) + px,
				m_Bounds.get(1, 0) + py, m_Bounds.get(1, 1) + py,
				m_Bounds.get(2, 0) + pz, m_Bounds.get(2, 1) + pz);
		}
		else
		{
			return m_Bounds;
		}
	}

	void Frame::__set_bounds() {}

	void Frame::set_bounds(int x, int y, int z, int width, int height, int depth)
	{
		m_Bounds.set(0, 0, x);
		m_Bounds.set(1, 0, y);
		m_Bounds.set(2, 0, z);

		m_Bounds.set(0, 1, x + width);
		m_Bounds.set(1, 1, y + height);
		m_Bounds.set(2, 1, z + depth);

		__set_bounds();
	}

	int Frame::get_width() const
	{
		return m_Bounds.get(0, 1) - m_Bounds.get(0, 0);
	}

	int Frame::get_height() const
	{
		return m_Bounds.get(1, 1) - m_Bounds.get(1, 0);
	}

	int Frame::get_depth() const
	{
		return m_Bounds.get(2, 1) - m_Bounds.get(2, 0);
	}

	void Frame::set_parent(Frame* parent)
	{
		m_Parent = parent;
	}

	void Frame::__display() const {}

	void Frame::display() const
	{
		// Set the clipping rectangle to the bounds of the frame
		// TODO
		//Buffer::set_clipping_rectangle(m_Bounds);

		// Set up the transformation
		MatrixStack& p = projection();
		p.push();
		p.translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), m_Bounds.get(2, 0));

		// Display the contents of the frame
		__display();

		// Deconstruct the transformation
		p.pop();

		// Set the clipping rectangle back to bounds of the parent
		if (m_Parent)
		{
			// TODO
			//Buffer::set_clipping_rectangle(m_Parent->get_bounds());
		}
		else
		{
			// TODO
			//Buffer::set_clipping_rectangle(mat2x2f(0.f, 0.f, INFINITY, INFINITY));
		}
	}



	void Button::highlight() {}
	void Button::unhighlight() {}
	void Button::click() {}

	int Button::trigger(const MouseMoveEvent& event_data)
	{
		mat3x2i absbounds = get_absolute_bounds();

		if (event_data.x >= absbounds.get(0, 0) && event_data.x < absbounds.get(0, 1)
			&& event_data.y >= absbounds.get(1, 0) && event_data.y < absbounds.get(1, 1))
		{
			if (!m_Highlighted)
			{
				m_Highlighted = true;
				highlight();
			}
		}
		else if (m_Highlighted)
		{
			m_Highlighted = false;
			unhighlight();
		}

		return EVENT_CONTINUE;
	}

	int Button::trigger(const MousePressEvent& event_data)
	{
		if (m_Highlighted)
		{
			click();
			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	void Button::freeze()
	{
		MouseMoveListener::freeze();
		MousePressListener::freeze();
	}

	void Button::unfreeze()
	{
		MouseMoveListener::unfreeze(m_Bounds.get(2, 0));
		MousePressListener::unfreeze(m_Bounds.get(2, 0));
	}



	TextInput::TextInput()
	{
		m_CursorGraphic = nullptr;
	}

	TextInput::~TextInput()
	{
		if (m_CursorGraphic)
		{
			delete m_CursorGraphic;
		}
	}

	string TextInput::get_input() const
	{
		return m_TextInput;
	}

	int TextInput::trigger(const MousePressEvent& event_data)
	{
		mat3x2i absbounds = get_absolute_bounds();

		if (event_data.x >= absbounds.get(0, 0) && event_data.x < absbounds.get(0, 1)
			&& event_data.y >= absbounds.get(1, 0) && event_data.y < absbounds.get(1, 1))
		{
			// Start responding to keyboard inputs
			if (m_TextFrozen)
			{
				m_TextFrozen = false;
				KeyboardListener::unfreeze(absbounds.get(2, 0));
			}

			// Construct cursor object
			if (!m_CursorGraphic)
			{
				m_CursorGraphic = new SolidColorGraphic(0, 0, 0, 255, 1, get_font()->get_line_height() + 1);
			}

			// Position cursor
			// TODO

			return EVENT_STOP;
		}
		else if (!m_TextFrozen)
		{
			m_TextFrozen = true;
			KeyboardListener::freeze();
		}

		return EVENT_CONTINUE;
	}

	int TextInput::trigger(const UnicodeEvent& event_data)
	{
		if (event_data.character == 0x08 && m_Cursor > 0) // Backspace
		{
			m_TextInput.erase(--m_Cursor, 1);
		}
		else if (event_data.character == 0x7f && m_Cursor < m_TextInput.size() - 1) // Delete
		{
			m_TextInput.erase(m_Cursor, 1);
		}
		else
		{
			m_TextInput.insert(m_Cursor++, 1, event_data.character);
		}

		return EVENT_STOP;
	}

	void TextInput::freeze()
	{
		m_TextFrozen = true;
		MousePressListener::freeze();
		KeyboardListener::freeze();
	}

	void TextInput::unfreeze()
	{
		MousePressListener::unfreeze(m_Bounds.get(2, 0));
	}

	void TextInput::__display() const
	{
		// Set up the transform
		MatrixStack& m = model();
		m.push();
		m.translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

		// Display the text input
		get_font()->display_line(m_TextInput, get_font_palette());

		// Display the cursor
		if (!m_TextFrozen)
		{
			m.translate(get_font()->get_line_width(m_TextInput.substr(0, m_Cursor)), 0.f, 0.f);
			m_CursorGraphic->display();
		}

		// Clean up the transform
		m.pop();
	}



	ScrollBar::ScrollBar(Graphic* backgroundGraphic, Graphic* arrowGraphic, Graphic* scrollGraphic, int x, int y, int z, bool horizontal)
	{
		m_Background = backgroundGraphic;
		m_Arrow = arrowGraphic;
		m_Scroller = scrollGraphic;

		if (horizontal)
		{
			m_Horizontal = true;
			set_bounds(x, y, z, m_Background->get_width() + (m_Arrow->get_width() * 2), max(m_Background->get_height(), m_Arrow->get_height()), 0);
		}
		else
		{
			m_Horizontal = false;
			set_bounds(x, y, z, max(m_Background->get_width(), m_Arrow->get_width()), m_Background->get_height() + (m_Arrow->get_height() * 2), 0);
		}

		m_Value = 0.f;
	}

	void ScrollBar::set_center_of_scroller(int dx, int dy)
	{
		if (m_Horizontal)
		{
			set_value(
				min(
					max(dx - (m_Scroller->get_width() / 2), 0)
					/ (float)(m_Background->get_width() - m_Scroller->get_width()),
					1.f
				)
			);
		}
		else
		{
			set_value(
				min(
					max(dy - (m_Scroller->get_height() / 2), 0)
					/ (float)(m_Background->get_height() - m_Scroller->get_height()),
					1.f
				)
			);
		}
	}

	float ScrollBar::get_value()
	{
		return m_Value;
	}

	void ScrollBar::set_value(float value)
	{
		m_Value = value;
	}

	int ScrollBar::trigger(const MousePressEvent& event_data)
	{
		mat3x2i absbounds = get_absolute_bounds();
		int dx = event_data.x - absbounds.get(0, 0);
		int dy = event_data.y - absbounds.get(1, 0);

		if (dx >= 0 && dx < get_width() && dy >= 0 && dy < get_height())
		{
			if (m_Horizontal)
			{
				if ((dx -= m_Arrow->get_width()) < 0)
				{
					// Click the left arrow
					set_value(0.f);
				}
				else if (dx < m_Background->get_width())
				{
					// Click in the scroll area
					set_center_of_scroller(dx, dy);
					dragged = this;
				}
				else
				{
					// Click the right arrow
					set_value(1.f);
				}
			}
			else
			{
				if ((dy -= m_Arrow->get_height()) < 0)
				{
					// Click the bottom arrow
					set_value(0.f);
				}
				else if (dy < m_Background->get_height())
				{
					// Click in the scroll area
					set_center_of_scroller(dx, dy);
					dragged = this;
				}
				else
				{
					// Click the top arrow
					set_value(1.f);
				}
			}

			return EVENT_STOP;
		}

		return EVENT_CONTINUE;
	}

	int ScrollBar::trigger(const MouseMoveEvent& event_data)
	{
		if (dragged == this)
		{
			mat3x2i absbounds = get_absolute_bounds();
			int dx = event_data.x - absbounds.get(0, 0) - m_Arrow->get_width();
			int dy = event_data.y - absbounds.get(1, 0) - m_Arrow->get_height();

			set_center_of_scroller(dx, dy);
		}

		return EVENT_CONTINUE;
	}

	void ScrollBar::__display() const
	{
		MatrixStack& m = model();
		if (m_Horizontal)
		{
			// Draw the left arrow
			m_Arrow->display();

			// Draw the background
			m.translate(m_Arrow->get_width());
			m_Background->display();

			// Draw the scroller
			m.push();
			m.translate(roundf(m_Value * (m_Background->get_width() - m_Scroller->get_width())), 0.f, -0.1f);
			m_Scroller->display();
			m.pop();

			// Draw the right arrow
			m.translate(m_Background->get_width() + m_Arrow->get_width());
			m.scale(-1.f);
			m_Arrow->display();
		}
		else
		{
			// Draw the bottom arrow
			m.translate(0.f, m_Arrow->get_height());

			m.push();
			m.scale(1.f, -1.f);
			m_Arrow->display();
			m.pop();

			// Draw the background
			m_Background->display();

			// Draw the scroller
			m.push();
			m.translate(0.f, roundf(m_Value * (m_Background->get_height() - m_Scroller->get_height() - (0.5f * m_Arrow->get_height()))), -0.1f);
			m.scale(-1.f);
			m.rotatez(1.570796f);
			m_Scroller->display();
			m.pop();

			// Draw the top arrow
			m.translate(0.f, m_Background->get_height());
			m_Arrow->display();
		}
	}



	ScrollableFrame::ScrollableFrame(Frame* frame, ScrollBar* horizontal, ScrollBar* vertical, int x, int y, int z, int width, int height, int depth)
	{
		m_Frame = frame;
		m_HorizontalScrollBar = horizontal;
		m_VerticalScrollBar = vertical;

		m_Frame->set_parent(this);
		if (m_HorizontalScrollBar) m_HorizontalScrollBar->set_parent(this);
		if (m_VerticalScrollBar) m_VerticalScrollBar->set_parent(this);

		set_bounds(x, y, z, width, height, depth);
	}

	void ScrollableFrame::__update(int frames_passed)
	{
		int w = get_width() - m_Frame->get_width();
		int h = get_height() - m_Frame->get_height();

		m_ScrollDistance = vec2i(min(w, 0), h);
	}

	void ScrollableFrame::__display() const
	{
		// Draw the frame itself
		MatrixStack& m = model();
		m.push();
		int dx = m_HorizontalScrollBar ? m_HorizontalScrollBar->get_value() * m_ScrollDistance.get(0) : 0;
		int dy = m_VerticalScrollBar ?
			(m_ScrollDistance.get(1) < 0 ? m_VerticalScrollBar->get_value() * m_ScrollDistance.get(1) : m_ScrollDistance.get(1))
			: 0;
		m.translate(dx, dy);
		m_Frame->display();
		m.pop();

		// Draw the scroll bars
		if (m_HorizontalScrollBar) m_HorizontalScrollBar->display();
		if (m_VerticalScrollBar) m_VerticalScrollBar->display();
	}

}