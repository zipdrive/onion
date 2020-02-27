#include "editor.h"

void ChunkEditor::display()
{
	static Application*& app = get_application_settings();
	int xpartition = app->width - 184;
	int ypartition = app->height - 24;

	// Draw world space in space from (0, 0) to (xpartition, ypartition)

	// Draw horizontal and vertical scroll bars

	
	if (m_Tool)
	{
		// Draw tool options
		m_Tool->display();
	}
	else
	{
		// Draw tool options sidebar from (xpartition + 24, 0) to (app->width, app->height)
		// TODO
	}
}