#include <algorithm>
#include "../../include/onions/frame.h"
#include "../../include/onions/application.h"

using namespace std;


Frame::Frame()
{
	m_Parent = nullptr;
}

Frame::Frame(int x, int y, int width, int height)
{
	m_Parent = nullptr;
	set_bounds(x, y, width, height);
}

const mat2x2i& Frame::get_bounds() const
{
	return m_Bounds;
}

mat2x2i Frame::get_absolute_bounds() const
{
	if (m_Parent)
	{
		mat2x2i pbounds = m_Parent->get_absolute_bounds();
		int px = pbounds.get(0, 0);
		int py = pbounds.get(1, 0);

		return mat2x2i(m_Bounds.get(0, 0) + px, m_Bounds.get(0, 1) + px,
			m_Bounds.get(1, 0) + py, m_Bounds.get(1, 1) + py);
	}
	else
	{
		return m_Bounds;
	}
}

void Frame::set_bounds(int x, int y, int width, int height)
{
	m_Bounds.set(0, 0, x);
	m_Bounds.set(1, 0, y);

	m_Bounds.set(0, 1, x + width);
	m_Bounds.set(1, 1, y + height);
}

int Frame::get_width() const
{
	return m_Bounds.get(0, 1) - m_Bounds.get(0, 0);
}

int Frame::get_height() const
{
	return m_Bounds.get(1, 1) - m_Bounds.get(1, 0);
}

void Frame::set_parent(Frame* parent)
{
	m_Parent = parent;
}



ScrollBar::ScrollBar(Graphic* backgroundGraphic, Graphic* arrowGraphic, Graphic* scrollGraphic, int x, int y, bool horizontal)
{
	m_Background = backgroundGraphic;
	m_Arrow = arrowGraphic;
	m_Scroller = scrollGraphic;

	if (horizontal)
	{
		m_Horizontal = true;
		set_bounds(x, y, m_Background->get_width() + (m_Arrow->get_width() * 2), max(m_Background->get_height(), m_Arrow->get_height()));
	}
	else
	{
		m_Horizontal = false;
		set_bounds(x, y, max(m_Background->get_width(), m_Arrow->get_width()), m_Background->get_height() + (m_Arrow->get_height() * 2));
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
	mat2x2i absbounds = get_absolute_bounds();
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
		mat2x2i absbounds = get_absolute_bounds();
		int dx = event_data.x - absbounds.get(0, 0) - m_Arrow->get_width();
		int dy = event_data.y - absbounds.get(1, 0) - m_Arrow->get_height();

		set_center_of_scroller(dx, dy);
	}

	return EVENT_CONTINUE;
}

void ScrollBar::display() const
{
	// Set up transformation
	mat_push();
	mat_translate(m_Bounds.get(0), m_Bounds.get(1), 0.f);

	if (m_Horizontal)
	{
		// Draw the left arrow
		m_Arrow->display();

		// Draw the background
		mat_translate(m_Arrow->get_width(), 0.f, 0.f);
		m_Background->display();

		// Draw the scroller
		mat_push();
		mat_translate(roundf(m_Value * (m_Background->get_width() - m_Scroller->get_width())), 0.f, -0.1f);
		m_Scroller->display();
		mat_pop();

		// Draw the right arrow
		mat_translate(m_Background->get_width() + m_Arrow->get_width(), 0.f, 0.f);
		mat_scale(-1.f, 1.f, 1.f);
		m_Arrow->display();
	}
	else
	{
		// Draw the bottom arrow
		mat_translate(0.f, m_Arrow->get_height(), 0.f);

		mat_push();
		mat_scale(1.f, -1.f, 1.f);
		m_Arrow->display();
		mat_pop();

		// Draw the background
		m_Background->display();

		// Draw the scroller
		mat_push();
		mat_translate(0.f, roundf(m_Value * (m_Background->get_height() - m_Scroller->get_height() - (0.5f * m_Arrow->get_height()))), -0.1f);
		mat_scale(-1.f, 1.f, 1.f);
		mat_rotatez(1.570796f);
		m_Scroller->display();
		mat_pop();

		// Draw the top arrow
		mat_translate(0.f, m_Background->get_height(), 0.f);
		m_Arrow->display();
	}

	mat_pop();
}



ScrollableFrame::ScrollableFrame(Frame* frame, ScrollBar* horizontal, ScrollBar* vertical, int x, int y, int width, int height)
{
	m_Frame = frame;
	m_HorizontalScrollBar = horizontal;
	m_VerticalScrollBar = vertical;

	m_Frame->set_parent(this);
	if (m_HorizontalScrollBar) m_HorizontalScrollBar->set_parent(this);
	if (m_VerticalScrollBar) m_VerticalScrollBar->set_parent(this);

	set_bounds(x, y, width, height);

	int w = width - m_Frame->get_width();
	int h = height - m_Frame->get_height();

	m_ScrollDistance = vec2i(min(w, 0), h);
}

void ScrollableFrame::display() const
{
	// Set up the translation
	mat_push();
	mat_translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

	// Draw the frame itself
	mat_push();
	int dx = m_HorizontalScrollBar ? m_HorizontalScrollBar->get_value() * m_ScrollDistance.get(0) : 0;
	int dy = m_VerticalScrollBar ? 
		(m_ScrollDistance.get(1) < 0 ? m_VerticalScrollBar->get_value() * m_ScrollDistance.get(1) : m_ScrollDistance.get(1))
		: 0;
	mat_translate(dx, dy, 0.f);
	m_Frame->display();
	mat_pop();

	// Draw the scroll bars
	if (m_HorizontalScrollBar) m_HorizontalScrollBar->display();
	if (m_VerticalScrollBar) m_VerticalScrollBar->display();

	// Clean up the translation
	mat_pop();
}




LayerFrame::LayerFrame()
{
	if (m_Parent)
		set_bounds(0, 0, m_Parent->get_width(), m_Parent->get_height());
	else
		set_bounds(-1, -1, 2, 2);

	insert_top(SolidColorGraphic::generate(rand() % 256, rand() % 256, rand() % 256, 255, 2, 2));
}

LayerFrame::LayerFrame(int x, int y, int width, int height)
{
	set_bounds(x, y, width, height);

	insert_top(SolidColorGraphic::generate(rand() % 256, rand() % 256, rand() % 256, 255, width, height));
}

void LayerFrame::reset()
{
	static Application*& app = get_application_settings();

	if (m_Sequence.empty())
	{
		m_ZScale = 1.f;
	}
	else
	{
		m_ZScale = 1.f / m_Sequence.size();
	}

	m_Transform.set(2, 2, m_ZScale);
	m_Transform.set(2, 3, 1.f - m_ZScale);
}

void LayerFrame::insert_top(Graphic* graphic)
{
	m_Sequence.push_back(graphic);

	if (Frame* frame = dynamic_cast<Frame*>(graphic))
	{
		frame->set_parent(this);
	}

	this->reset();
}

void LayerFrame::display() const
{
	// Set up the transformation
	mat_push();
	mat_custom_transform(m_Transform);

	// Display the layers from back to front
	float dz = -2.f * m_ZScale;
	for (auto iter = m_Sequence.begin(); iter != m_Sequence.end(); ++iter)
	{
		(*iter)->display();
		mat_translate(0.f, 0.f, dz);
	}

	mat_pop();
}


UIFrame::UIFrame()
{
	Application* app = get_application_settings();
	set_bounds(0, 0, app->width, app->height);
	reset();
}

void UIFrame::reset()
{
	LayerFrame::reset();

	Application* app = get_application_settings();
	float sx = 2.f / app->width;
	float sy = 2.f / app->height;

	m_Transform.set(0, 0, sx);
	m_Transform.set(1, 1, sy);
	m_Transform.set(0, 3, (sx * m_Bounds.get(0, 0)) - 1.f);
	m_Transform.set(1, 3, (sy * m_Bounds.get(1, 0)) - 1.f);
}




vec2i WorldOrthographicFrame::Tool::get_tile(int dx, int dy)
{
	return vec2i(dx / TILE_WIDTH, dy / TILE_HEIGHT);
}

Object* WorldOrthographicFrame::Tool::get_object(int dx, int dy)
{
	return nullptr;
}


WorldOrthographicFrame::WorldOrthographicFrame(int x, int y, int width, int height, int tile_margin) : m_TileMargin(tile_margin)
{
	set_bounds(x, y, width, height);
}

void WorldOrthographicFrame::reset()
{
	// Calculate the world space area
	m_Chunk = World::get_chunk();

	int x = roundf(m_Camera.get(0));
	int y = roundf(m_Camera.get(1));

	int width = get_width();
	int height = get_height();
	int halfWidth = (width + 1) / 2;
	int halfHeight = (height + 1) / 2;

	m_DisplayArea.set(0, 0, x - halfWidth);
	m_DisplayArea.set(0, 1, x + (width / 2) - 1);
	m_DisplayArea.set(1, 0, y - halfHeight);
	m_DisplayArea.set(1, 1, y + (height / 2) - 1);
	clamp_display_area();

	// Construct the transformation matrix
	Application* app = get_application_settings();
	float sx = 2.f / app->width; // The x-axis scale of the projection
	float sy = 2.f / app->height; // The y-axis scale of the projection
	float sz = 2.f / (TILE_HEIGHT * m_Chunk->height); // The z-scale of the projection

	int mx = 1 + (width % 2);
	int my = 1 + (height % 2);

	int fx = -(m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1) + mx) / 2;
	int fy = -(m_DisplayArea.get(1, 0) + m_DisplayArea.get(1, 1) + my) / 2;

	float cx = (sx * ((m_Bounds.get(0, 0) + halfWidth) + fx)) - 1.f;
	float cy = (sy * ((m_Bounds.get(1, 0) + halfHeight) + fy)) - 1.f;

	m_Transform = mat4x4f(
		sx, 0.f, 0.f, cx,
		0.f, sy, -sy, cy,
		0.f, sz, sz, sz * fy
	);

	m_Tool = nullptr;
}

void WorldOrthographicFrame::clamp_display_area()
{
	int xmin = TILE_WIDTH * m_TileMargin;
	int xmax = (m_Chunk->width * TILE_WIDTH) - (xmin + 1);
	int xleft = xmin - m_DisplayArea.get(0, 0);
	int xright = xmax - m_DisplayArea.get(0, 1);
	if (xleft > 0)
	{
		m_DisplayArea(0, 1) += xleft;
		m_DisplayArea.set(0, 0, xmin);
	}
	else if (xright < 0)
	{
		m_DisplayArea(0, 0) += xright;
		m_DisplayArea.set(0, 1, xmax);
	}

	int ymin = TILE_HEIGHT * m_TileMargin;
	int ymax = (m_Chunk->height * TILE_HEIGHT) - (ymin + 1);
	int ybottom = ymin - m_DisplayArea.get(1, 0);
	int ytop = ymax - m_DisplayArea.get(1, 1);
	if (ybottom > 0)
	{
		m_DisplayArea(1, 1) += ybottom;
		m_DisplayArea.set(1, 0, ymin);
	}
	else if (ytop < 0)
	{
		m_DisplayArea(1, 0) += ytop;
		m_DisplayArea.set(1, 1, ymax);
	}
}

void WorldOrthographicFrame::set_camera(float x, float y)
{
	m_Camera.set(0, 0, x);
	m_Camera.set(1, 0, y);
	reset();
}

void WorldOrthographicFrame::set_tool(WorldOrthographicFrame::Tool* tool)
{
	m_Tool = tool;
}

void WorldOrthographicFrame::adjust_camera(float dx, float dy)
{
	// Get camera coordinates
	float& cx = m_Camera(0);
	float& cy = m_Camera(1);

	// Store original (integer) camera coordinates
	int former_x = roundf(cx);
	int former_y = roundf(cy);

	// Adjust camera
	cx += dx;
	cy += dy;

	// Calculate absolute change in x-coordinates of camera
	int dcx = (int)roundf(cx) - former_x;
	if (dcx)
	{
		int width = m_Bounds.get(0, 1) - m_Bounds.get(0, 0);

		int xmin = TILE_WIDTH * m_TileMargin;
		int xmax = (TILE_WIDTH * m_Chunk->width) - xmin - 1;

		// Calculate displayed change in x-coordinates of camera
		// (Which may be different from absolute change due to clamping of display)
		if (dcx > 0)
		{
			int former_xmin = former_x - ((width + 1) / 2);

			if (former_xmin < xmin)
				dcx = max(0, dcx - xmin + former_xmin);
			dcx = min(dcx, xmax - m_DisplayArea.get(0, 1));
		}
		else
		{
			int former_xmax = former_x + (width / 2) - 1;

			if (former_xmax > xmax)
				dcx = min(0, dcx - xmax + former_xmax);
			dcx = max(dcx, xmin - m_DisplayArea.get(0, 0));
		}

		if (dcx)
		{
			m_DisplayArea(0, 0) += dcx;
			m_DisplayArea(0, 1) += dcx;
		}
	}

	// Calculate absolute change in y-coordinates of camera
	int dcy = (int)roundf(cy) - former_y;
	if (dcy)
	{
		int height = m_Bounds.get(1, 1) - m_Bounds.get(1, 0);

		int ymin = TILE_HEIGHT * m_TileMargin;
		int ymax = (TILE_HEIGHT * m_Chunk->width) - ymin - 1;

		// Calculate displayed change in y-coordinates of camera
		if (dcy > 0)
		{
			int former_ymin = former_y - ((height + 1) / 2);

			if (former_ymin < ymin)
				dcy = max(0, dcy - ymin + former_ymin);
			dcy = min(dcy, ymax - m_DisplayArea.get(1, 1));
		}
		else
		{
			int former_ymax = former_y + (height / 2) - 1;

			if (former_ymax > ymax)
				dcy = min(0, dcy - ymax + former_ymax);
			dcy = max(dcy, ymin - m_DisplayArea.get(1, 0));
		}

		if (dcy)
		{
			m_DisplayArea(1, 0) += dcy;
			m_DisplayArea(1, 1) += dcy;
		}
	}

	// If there was a change in the integer camera position, clamp the display and adjust the transformation
	if (dcx || dcy)
	{
		// Calculate the change in the translation
		float dtx = -m_Transform.get(0, 0) * dcx;
		float dty = -m_Transform.get(1, 1) * dcy;
		float dtz = -m_Transform.get(2, 2) * dcy;

		// Adjust the transformation
		m_Transform(0, 3) += dtx;
		m_Transform(1, 3) += dty;
		m_Transform(2, 3) += dtz;
	}
}

void WorldOrthographicFrame::display() const
{
	// Set up transformation.
	mat_push();
	mat_custom_transform(m_Transform);

	// Draw chunk
	m_Chunk->display(
		m_DisplayArea.get(0, 0), m_DisplayArea.get(1, 0),
		m_DisplayArea.get(0, 1), m_DisplayArea.get(1, 1)
	);

	mat_pop();
}

int WorldOrthographicFrame::trigger(const MousePressEvent& event_data)
{
	if (m_Tool)
	{
		mat2x2i absbounds = get_absolute_bounds();
		if (event_data.x >= absbounds.get(0, 0) && event_data.x < absbounds.get(0, 1)
			&& event_data.y >= absbounds.get(1, 0) && event_data.y < absbounds.get(1, 1))
		{
			m_Tool->select();
			dragged = this;
			return EVENT_STOP;
		}
	}

	return EVENT_CONTINUE;
}

int WorldOrthographicFrame::trigger(const MouseMoveEvent& event_data)
{
	if (m_Tool)
	{
		mat2x2i absbounds = get_absolute_bounds();
		int dx = event_data.x - absbounds.get(0, 0);
		int dy = event_data.y - absbounds.get(1, 0);
		if (dx >= 0 && event_data.x < absbounds.get(0, 1)
			&& dy >= 0 && dy < absbounds.get(1, 1))
		{
			// Call the tool highlight function
			m_Tool->highlight(dx + m_DisplayArea.get(0, 0), dy + m_DisplayArea.get(1, 0));

			// If mouse button is held down, call the tool select function
			if (dragged == this) m_Tool->select();

			return EVENT_STOP;
		}
	}

	return EVENT_CONTINUE;
}