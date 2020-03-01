#include "editor.h"


SpriteSheet* g_GUI;

SpriteSheet* get_gui_sprite_sheet()
{
	if (!g_GUI)
	{
		g_GUI = SpriteSheet::generate_empty();
		g_GUI->load_sprite_sheet("editor/ui.png");
	}

	return g_GUI;
}




// A scroll bar that controls the x- or y-coordinate of the camera of a world frame.
class WorldScrollBarFrame : public ScrollBarFrame
{
private:
	// A pointer to the world orthographic frame.
	WorldOrthographicFrame* m_WorldFrame;

public:
	/// <summary>Constructs a scroll bar.</summary>
	/// <param name="worldFrame">The world orthographic frame.</param>
	/// <param name="backgroundGraphic">A graphic to display as the background for the scrolling area.</param>
	/// <param name="arrowGraphic">A graphic to display as arrows on either side of the scrolling area.</param>
	/// <param name="scrollGraphic">A graphic to display to show the current value of the scroll bar.</param>
	/// <param name="x">The x-coordinate of the left side.</param>
	/// <param name="y">The y-coordinate of the bottom side.</param>
	/// <param name="horizontal">True if the scroll bar is horizontal, false if vertical.</param>
	WorldScrollBarFrame(WorldOrthographicFrame* worldFrame, Graphic* backgroundGraphic, Graphic* arrowGraphic, Graphic* scrollGraphic, int x, int y, bool horizontal) : ScrollBarFrame(backgroundGraphic, arrowGraphic, scrollGraphic, x, y, horizontal)
	{
		m_WorldFrame = worldFrame;

		m_WorldFrame->set_camera(0.5f * World::get_chunk()->width * TILE_WIDTH, 0.5f * World::get_chunk()->height * TILE_HEIGHT);
		m_Value = 0.5f;
	}

	void set_value(float value)
	{
		float dv = value - m_Value;
		const mat2x2i& bounds = m_WorldFrame->get_bounds();

		if (m_Horizontal)
		{
			int span = (World::get_chunk()->width * TILE_WIDTH) - (bounds.get(0, 1) - bounds.get(0, 0));
			m_WorldFrame->adjust_camera(dv * span, 0.f);
		}
		else
		{
			int span = (World::get_chunk()->height * TILE_HEIGHT) - (bounds.get(1, 1) - bounds.get(1, 0));
			m_WorldFrame->adjust_camera(0.f, dv * span);
		}

		m_Value = value;
	}
};




ChunkEditor::ChunkEditor()
{
	SpriteSheet* gui = get_gui_sprite_sheet();

	Application*& app = get_application_settings();
	int chunkFrameWidth = app->width - 184;
	int chunkFrameHeight = app->height - 48;

	m_Frame = new WorldOrthographicFrame(0, 24, chunkFrameWidth, chunkFrameHeight, 0);

	m_UI = new UIFrame();

	mat4x4f palette = generate_palette_matrix(
		230, 247, 251, 0,
		38, 162, 221, 0
	);

	// Bottom-right corner between scroll bars
	//m_UI->insert_top(generate_solid_color_graphic(230, 247, 251, 255, 24, 24));

	// World horizontal scroll bar
	Graphic* horizontalWSBBG = generate_solid_color_graphic(230, 247, 251, 255, chunkFrameWidth - 48, 24);
	Graphic* horizontalWSBArrow = new StaticSpriteGraphic(gui, Sprite::get_sprite(LEFT_ARROW_SPRITE), palette);
	Graphic* horizontalWSBScroller = new StaticSpriteGraphic(gui, Sprite::get_sprite(SCROLL_BAR), palette);

	ScrollBarFrame* horizontalWSB = new WorldScrollBarFrame(
			m_Frame,
			horizontalWSBBG,
			horizontalWSBArrow,
			horizontalWSBScroller,
			0,
			0,
			true
		);

	m_UI->insert_top(horizontalWSB);
	push_mouse_press_listener(horizontalWSB);

	Graphic* verticalWSBBG = generate_solid_color_graphic(230, 247, 251, 255, 24, chunkFrameHeight - 48);
	Graphic* verticalWSBArrow = new StaticSpriteGraphic(gui, Sprite::get_sprite(UP_ARROW_SPRITE), palette);
	Graphic* verticalWSBScroller = new StaticSpriteGraphic(gui, Sprite::get_sprite(SCROLL_BAR), palette);
	
	ScrollBarFrame* verticalWSB = new WorldScrollBarFrame(
			m_Frame,
			verticalWSBBG,
			verticalWSBArrow,
			verticalWSBScroller,
			chunkFrameWidth,
			24,
			false
		);

	m_UI->insert_top(verticalWSB);
	push_mouse_press_listener(verticalWSB);

	m_Layers = new LayerFrame();
	m_Layers->insert_top(m_Frame);
	m_Layers->insert_top(m_UI);

	m_Tool = nullptr;
}

void ChunkEditor::display()
{
	// Draw world space in space from (0, 24) to (W - 184, H - 24)
	m_Layers->display();

	// Draw horizontal and vertical scroll bars

	// Draw tool sidebar from (W - 184, 24) to (W, H)
	if (m_Tool)
	{
		// Draw tool options
		m_Tool->display();
	}
	else
	{
		// Draw tool selection sidebar
		// TODO
	}
}