#include "../include/editor.h"


SpriteSheet* g_GUI = nullptr;

SpriteSheet* get_gui_sprite_sheet()
{
	if (!g_GUI)
	{
		g_GUI = SpriteSheet::generate_empty();
		g_GUI->load_sprite_sheet("editor/ui.png");
	}

	return g_GUI;
}
