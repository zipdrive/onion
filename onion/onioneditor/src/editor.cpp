#include <fstream>
#include <regex>

#include "../include/editor.h"

using namespace std;


SpriteSheet* g_GUI = nullptr;

SpriteSheet* get_gui_sprite_sheet()
{
	if (!g_GUI)
	{
		g_GUI = SpriteSheet::generate("editor/ui.png");
	}

	return g_GUI;
}


Font* g_GUIFont = nullptr;

Font* get_gui_font()
{
	if (!g_GUIFont)
	{
		g_GUIFont = Font::load_sprite_font("default.png");
	}

	return g_GUIFont;
}



unordered_map<string, SpriteSheet*> Data::m_SpriteSheets;
map<CHUNK_KEY, string> Data::m_Chunks;

void Data::save()
{
	// Save the sprite sheets
	SaveFile sprites_file("res/data/sprites.dat");

	for (auto iter = m_SpriteSheets.begin(); iter != m_SpriteSheets.end(); ++iter)
	{
		sprites_file.save_string(iter->first);
	}

	// Save the chunks
	SaveFile world_file("res/data/world.dat");

	for (auto iter = m_Chunks.begin(); iter != m_Chunks.end(); ++iter)
	{
		world_file.save_int(iter->first);
		world_file.save_string(iter->second);
	}
}

void Data::load()
{
	// Load sprites
	LoadFile sprites_file("res/data/sprites.dat");

	while (sprites_file.good())
	{
		// Load the path
		string path = sprites_file.load_string();

		// Load sprite sheet from path
		SpriteSheet* sheet = SpriteSheet::generate_empty();
		sheet->load_sprite_sheet(path.c_str());

		m_SpriteSheets.emplace(path, sheet);
	}

	// Load chunks
	LoadFile world_file("res/data/world.dat");

	while (world_file.good())
	{
		// Load the key and name of the chunk
		CHUNK_KEY key = world_file.load_int();
		string name = world_file.load_string();

		m_Chunks.emplace(key, name);
	}
}