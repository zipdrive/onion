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


ChunkEditor::ChunkEditor()
{
	SpriteSheet* gui = get_gui_sprite_sheet();

	Application*& app = get_application_settings();
	int chunkFrameWidth = app->width - 184;
	int chunkFrameHeight = app->height - 48;

	m_Frame = new WorldOrthographicFrame(0, 24, chunkFrameWidth, chunkFrameHeight, 1);

	m_UI = new UIFrame(0, 0, app->width, app->height);
	m_UI->insert_top(
		new StaticSpriteGraphic(
			gui, 
			Sprite::get_sprite(SCROLL_BAR), 
			mat4x4f()
		)
	);

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