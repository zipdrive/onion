#include "../include/dialog.h"
#include "../include/editorchunk.h"

// A scroll bar that controls the x- or y-coordinate of the camera of a world frame.
class WorldScrollBar : public ScrollBar
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
	WorldScrollBar(WorldOrthographicFrame* worldFrame, Graphic* backgroundGraphic, Graphic* arrowGraphic, Graphic* scrollGraphic, int x, int y, bool horizontal) : ScrollBar(backgroundGraphic, arrowGraphic, scrollGraphic, x, y, horizontal)
	{
		m_WorldFrame = worldFrame;

		//m_WorldFrame->set_camera(0.5f * World::get_chunk()->width * TILE_WIDTH, 0.5f * World::get_chunk()->height * TILE_HEIGHT);
		m_Value = 0.5f;
	}

	/// <summary>Sets the value for the scroll bar.</summary>
	/// <param name="value">The value for the scroll bar.</param>
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




PaintSelectFrame::PaintSelectFrame(int width)
{
	int height = TILE_HEIGHT - 2;

	m_TileSpriteSheet = Chunk::get_tile_sprite_sheet();
	m_NumberTiles = (m_TileSpriteSheet->width / TILE_WIDTH) * (m_TileSpriteSheet->height / TILE_HEIGHT);
	m_NumberTilesPerRow = (width / (TILE_WIDTH + 2)) - 1;
	m_Margin = (width - (m_NumberTilesPerRow * TILE_WIDTH)) / 2;
	m_NumberRows = (m_NumberTiles + m_NumberTilesPerRow - 1) / m_NumberTilesPerRow;

	height += m_NumberRows * (TILE_HEIGHT + 2);

	set_bounds(0, 0, width, height);

	m_Highlight = SolidColorGraphic::generate(1.f, 0.f, 0.f, 1.f, TILE_WIDTH + 4, TILE_HEIGHT + 4);
	m_PaintKey = 0;
}

SPRITE_KEY PaintSelectFrame::get_paint_key() const
{
	return m_PaintKey;
}

void PaintSelectFrame::display() const
{
	const mat4x4f palette = mat4x4f();
	const float dx = TILE_WIDTH + 2;
	const float dy = TILE_HEIGHT + 2;

	mat_push();
	mat_translate(m_Margin, TILE_HEIGHT / 2, 0.5f);

	for (int r = m_NumberRows - 1; r >= 0; --r)
	{
		mat_push();
		for (int c = 0; c < m_NumberTilesPerRow; ++c)
		{
			int index = (r * m_NumberTilesPerRow) + c;
			if (index < m_NumberTiles)
			{
				SPRITE_KEY key = 6 * index;

				if (key == m_PaintKey)
				{
					mat_push();
					mat_translate(-2.f, -2.f, 0.1f);
					m_Highlight->display();
					mat_pop();
				}

				m_TileSpriteSheet->display(key, palette);
			}

			mat_translate(dx, 0.f, 0.f);
		}

		mat_pop();
		mat_translate(0.f, dy, 0.f);
	}

	mat_pop();
}

void PaintSelectFrame::select(int dx, int dy)
{
	int horizontalIndex = (dx - m_Margin + 1) / (TILE_WIDTH + 2);
	int verticalIndex = m_NumberRows - 1 - ((dy - (TILE_HEIGHT / 2) + 1) / (TILE_HEIGHT + 2));

	if (horizontalIndex >= 0 && horizontalIndex < m_NumberTilesPerRow
		&& verticalIndex >= 0 && verticalIndex < m_NumberRows)
	{
		m_PaintKey = ((verticalIndex * m_NumberTilesPerRow) + horizontalIndex) * 6;
	}
}


PaintChunkTool::PaintChunkTool()
{
	Application*& app = get_application_settings();
	SpriteSheet* gui = get_gui_sprite_sheet();

	// Construct the palette for the UI
	mat4x4f palette = generate_palette_matrix(
		230, 247, 251, 0,
		38, 162, 221, 0
	);

	// Vertical scroll bar
	Graphic* verticalWSBBG = SolidColorGraphic::generate(230, 247, 251, 255, 24, app->height - 72);
	Graphic* verticalWSBArrow = new StaticSpriteGraphic(gui, Sprite::get_sprite(UP_ARROW_SPRITE), palette);
	Graphic* verticalWSBScroller = new StaticSpriteGraphic(gui, Sprite::get_sprite(SCROLL_BAR), palette);

	m_VerticalScrollBar = new ScrollBar(
		verticalWSBBG,
		verticalWSBArrow,
		verticalWSBScroller,
		136,
		0,
		false
	);
	m_VerticalScrollBar->set_value(1.f);

	// Paint select frame
	m_ToolbarSelectFrame = new PaintSelectFrame(136);

	// Scrollable frame
	m_ToolbarFrame = new ScrollableFrame(m_ToolbarSelectFrame, nullptr, m_VerticalScrollBar, app->width - 160, 0, 136, app->height - 24);
}

void PaintChunkTool::display()
{
	m_ToolbarFrame->display();
}

int PaintChunkTool::trigger(const MousePressEvent& event_data)
{
	mat2x2i bounds = m_ToolbarFrame->get_bounds();
	int dx = event_data.x - bounds.get(0, 0);
	int dy = event_data.y - bounds.get(1, 0);

	if (dx >= 0 && dx < m_ToolbarFrame->get_width()
		&& dy >= 0 && dy < m_ToolbarFrame->get_height())
	{
		if (m_ToolbarFrame->get_height() < m_ToolbarSelectFrame->get_height())
		{
			dy += (m_ToolbarSelectFrame->get_height() - m_ToolbarFrame->get_height()) * m_VerticalScrollBar->get_value();
		}
		else
		{
			dy += m_ToolbarFrame->get_height() - m_ToolbarSelectFrame->get_height();
		}

		m_ToolbarSelectFrame->select(dx, dy);

		return EVENT_STOP;
	}

	return EVENT_CONTINUE;
}

void PaintChunkTool::freeze()
{
	MousePressListener::freeze();
	m_VerticalScrollBar->freeze();
}

void PaintChunkTool::unfreeze()
{
	MousePressListener::unfreeze();
	m_VerticalScrollBar->unfreeze();
}

void PaintChunkTool::highlight(int dx, int dy)
{
	m_Highlight = get_tile(dx, dy);
}

void PaintChunkTool::select()
{
	World::get_chunk()->set_tile(m_Highlight.get(0), m_Highlight.get(1), m_ToolbarSelectFrame->get_paint_key());
}




ChunkEditor::ChunkEditor()
{
	SpriteSheet* gui = get_gui_sprite_sheet();

	Application*& app = get_application_settings();
	int chunkFrameWidth = app->width - 184;
	int chunkFrameHeight = app->height - 48;

	m_WorldFrame = new WorldOrthographicFrame(0, 24, chunkFrameWidth, chunkFrameHeight, 0);


	// Construct the palette for the UI
	mat4x4f palette = generate_palette_matrix(
		230, 247, 251, 0,
		38, 162, 221, 0
	);

	// World vertical scroll bar
	Graphic* verticalWSBBG = SolidColorGraphic::generate(230, 247, 251, 255, 24, chunkFrameHeight - 48);
	Graphic* verticalWSBArrow = new StaticSpriteGraphic(gui, Sprite::get_sprite(UP_ARROW_SPRITE), palette);
	Graphic* verticalWSBScroller = new StaticSpriteGraphic(gui, Sprite::get_sprite(SCROLL_BAR), palette);

	m_VerticalScrollBar = new WorldScrollBar(
		m_WorldFrame,
		verticalWSBBG,
		verticalWSBArrow,
		verticalWSBScroller,
		chunkFrameWidth,
		24,
		false
	);

	// World horizontal scroll bar
	Graphic* horizontalWSBBG = SolidColorGraphic::generate(230, 247, 251, 255, chunkFrameWidth - 48, 24);
	Graphic* horizontalWSBArrow = new StaticSpriteGraphic(gui, Sprite::get_sprite(LEFT_ARROW_SPRITE), palette);
	Graphic* horizontalWSBScroller = new StaticSpriteGraphic(gui, Sprite::get_sprite(SCROLL_BAR), palette);

	m_HorizontalScrollBar = new WorldScrollBar(
		m_WorldFrame,
		horizontalWSBBG,
		horizontalWSBArrow,
		horizontalWSBScroller,
		0,
		0,
		true
	);

	// Corner between scroll bars
	m_ScrollBarCorner = SolidColorGraphic::generate(219, 243, 250, 255, 24, 24);

	// Background for the toolbars
	m_ToolbarBackground = SolidColorGraphic::generate(219, 243, 250, 255, 184, chunkFrameHeight + 24);



	/*
	// Toolbar
	LayerFrame* toolFrame = new LayerFrame(chunkFrameWidth, 0, 184, m_Bounds.get(1, 1) - m_Bounds.get(1, 0));
	uiFrame->insert_top(toolFrame);

	// Background
	toolFrame->insert_top(generate_solid_color_graphic(230, 247, 251, 255, 184, m_Bounds.get(1, 1) - m_Bounds.get(1, 0)));
	*/

	m_Tool = nullptr;
	set_tool(new PaintChunkTool());
}

void ChunkEditor::set_tool(ChunkTool* tool)
{
	if (m_Tool) m_Tool->freeze();

	m_Tool = tool;
	m_WorldFrame->set_tool(m_Tool);
	m_Tool->unfreeze();
}

void ChunkEditor::display()
{
	// Draw world space in space from (0, 24) to (W - 184, H - 24)
	mat_push();
	mat_identity();
	mat_translate(0.f, 0.f, 0.5f);
	mat_scale(1.f, 1.f, 0.5f);
	m_WorldFrame->display();
	mat_pop();

	// Draw scroll bars
	m_HorizontalScrollBar->display();
	m_VerticalScrollBar->display();

	// Draw corner between scroll bars
	mat_push();
	mat_translate(m_HorizontalScrollBar->get_width(), 0.f, 0.f);
	m_ScrollBarCorner->display();

	// Draw tool sidebar from (W - 184, 24) to (W, H)
	mat_translate(24.f, 0.f, 1.f);
	m_ToolbarBackground->display();

	if (m_Tool)
	{
		// Draw tool options
		mat_pop();
		m_Tool->display();
	}
	else
	{
		// Draw tool selection sidebar
		mat_translate(0.f, 0.f, -1.f);
		// TODO
		mat_pop();
	}
}

void ChunkEditor::freeze()
{
	// Freezes the scroll bars
	m_HorizontalScrollBar->freeze();
	m_VerticalScrollBar->freeze();

	// Freezes the world frame
	m_WorldFrame->freeze();

	// Freezes the toolbar
	if (m_Tool)
		m_Tool->freeze();
}

void ChunkEditor::unfreeze()
{
	// Unfreezes the scroll bars
	m_HorizontalScrollBar->unfreeze();
	m_VerticalScrollBar->unfreeze();

	// Unfreezes the world frame
	m_WorldFrame->unfreeze();

	// Unfreezes the toolbar
	if (m_Tool)
		m_Tool->unfreeze();
}



ChunksEditor::ChunksListFrame::ChunksListFrame()
{
	set_bounds(0, 0, get_width(), Data::chunks.empty() ? 28 : (20 * Data::chunks.size()) + 8);
}

int ChunksEditor::ChunksListFrame::get_height() const
{
	return Data::chunks.empty() ? 28 : (20 * Data::chunks.size()) + 8;
}

void ChunksEditor::ChunksListFrame::display() const
{
	Font* gui = get_gui_font();
	const mat4x4f& palette = get_gui_font_palette();

	if (Data::chunks.empty())
	{
		// Display that no chunks have been initialized
		mat_push();
		mat_translate(12.f, 8.f, 0.f);
		gui->display_line("NO CHUNKS HAVE BEEN INITIALIZED.", palette);
		mat_pop();
	}
	else
	{
		// Display a list of all chunks
		mat_push();
		mat_translate(12.f, -12.f, 0.f);
		for (auto iter = Data::chunks.rbegin(); iter != Data::chunks.rend(); ++iter)
		{
			mat_translate(0.f, 20.f, 0.f);

			if (iter->first == m_Highlighted)
			{
				// TODO
			}

			get_gui_font()->display_line(iter->second, palette);
		}
		mat_pop();
	}
}



class NewChunkDialog : public TextInputDialog
{
public:
	NewChunkDialog() : TextInputDialog("ENTER THE NAME OF THE AREA:") {}

	void confirm()
	{
		CHUNK_KEY key = 1;
		while (true)
		{
			auto iter = Data::chunks.find(key);
			if (iter == Data::chunks.end())
			{
				Data::chunks.emplace(key, m_TextInput.get_input());
				break;
			}

			++key;
		}

		TextInputDialog::confirm();
	}
};

void ChunksEditor::NewChunkButton::click()
{
	push_dialog(new NewChunkDialog());
}

ChunksEditor::NewChunkButton::NewChunkButton() : GUITextButton("NEW") {}



ChunksEditor::ChunksEditor()
{
	Application* app = get_application_settings();
	int h = app->height - 64;

	SpriteSheet* gui = get_gui_sprite_sheet();

	// Construct the palette for the UI
	m_Background = SolidColorGraphic::generate(230, 247, 251, 255, app->width - 24, h);
	m_LowBackground = SolidColorGraphic::generate(230, 247, 251, 255, app->width, 40);

	// World vertical scroll bar
	Graphic* verticalWSBBG = SolidColorGraphic::generate(230, 247, 251, 255, 24, h - 48);
	Graphic* verticalWSBArrow = new StaticSpriteGraphic(gui, Sprite::get_sprite(UP_ARROW_SPRITE), get_gui_palette());
	Graphic* verticalWSBScroller = new StaticSpriteGraphic(gui, Sprite::get_sprite(SCROLL_BAR), get_gui_palette());

	m_ScrollBar = new ScrollBar(
		verticalWSBBG,
		verticalWSBArrow,
		verticalWSBScroller,
		app->width - 24,
		0,
		false
	);

	m_ChunksScroller = new ScrollableFrame(&m_ChunksList, nullptr, m_ScrollBar, 0, 40, app->width, h);

	m_NewChunkButton.editor = this;
	m_NewChunkButton.set_bounds(8, 8, 72, 24);
}

void ChunksEditor::display()
{
	if (m_CurrentlyEditingChunk)
	{
		m_ChunkEditor.display();
	}
	else
	{
		mat_push();
		mat_translate(0.f, 40.f, 0.f);
		m_Background->display();
		mat_pop();

		m_ChunksScroller->display();

		m_LowBackground->display();
		m_NewChunkButton.display();
	}
}

void ChunksEditor::freeze()
{
	m_ScrollBar->freeze();
	m_ChunksScroller->freeze();

	m_NewChunkButton.freeze();
}

void ChunksEditor::unfreeze()
{
	m_ScrollBar->unfreeze();
	m_ChunksScroller->unfreeze();

	m_NewChunkButton.unfreeze();
}