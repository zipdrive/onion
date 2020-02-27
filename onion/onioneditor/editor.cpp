#include "editor.h"

ChunkEditor::ChunkEditor()
{
	Application*& app = get_application_settings();
	int chunkFrameWidth = app->width - 184;
	int chunkFrameHeight = app->height - 48;

	m_Frame = new WorldOrthographicFrame(0, 24, chunkFrameWidth, chunkFrameHeight, 1);

	m_UI = new UIFrame(0, 0, app->width, app->height);
	m_UI->insert_top(generate_solid_color_graphic(1.f, 1.f, 1.f, 1.f, 100, 100));

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