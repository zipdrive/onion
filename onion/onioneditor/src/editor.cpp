#include <fstream>
#include <regex>

#include "../include/editor.h"

using namespace std;


SpriteSheet* get_gui_sprite_sheet()
{
	static SpriteSheet* gui = SpriteSheet::generate("editor/ui.png");
	return gui;
}

const mat4x4f& get_gui_palette()
{
	static mat4x4f palette = generate_palette_matrix(
		230, 247, 251, 0,
		38, 162, 221, 0
	);

	return palette;
}


Font* get_gui_font()
{
	static Font* guiFont = Font::load_sprite_font("default.png");
	return guiFont;
}

const mat4x4f& get_gui_font_palette()
{
	static mat4x4f palette = generate_palette_matrix(
		0, 0, 0, 255,
		0, 0, 0, 32,
		0, 0, 0, 0,
		0, 0, 0, 0
	);

	return palette;
}


SlicedGraphic* get_gui_pane()
{
	static SlicedGraphic* guiPane = new SlicedGraphic(
		nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr,
		24, 24
	);
	return guiPane;
}




GUITextButton::GUITextButton(string text)
{
	m_Text = text;
	m_Background = SolidColorGraphic::generate(1.f, 0.f, 0.f, 1.f, 1, 1);
}

void GUITextButton::display() const
{
	int w = get_width();
	int h = get_height();

	// Set up the transform
	mat_push();
	mat_translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

	// Draw the background
	SlicedGraphic* pane = get_gui_pane();
	pane->width = w;
	pane->height = h;
	pane->display();

	m_Background->width = w;
	m_Background->height = h;
	m_Background->display();

	// Draw the text
	mat_translate((w - (6 * m_Text.size()) - 1) / 2, (h - 14) / 2, 0.f);
	get_gui_font()->display_line(m_Text, get_gui_font_palette());

	// Clean up
	mat_pop();
}



unordered_map<string, SpriteSheet*> Data::sprite_sheets;
map<CHUNK_KEY, string> Data::chunks;

void Data::save()
{
	// Save the sprite sheets
	SaveFile sprites_file("res/data/sprites.dat");

	for (auto iter = sprite_sheets.begin(); iter != sprite_sheets.end(); ++iter)
	{
		sprites_file.save_string(iter->first);
	}

	// Save the chunks
	SaveFile world_file("res/data/world.dat");

	for (auto iter = chunks.begin(); iter != chunks.end(); ++iter)
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

		sprite_sheets.emplace(path, sheet);
	}

	// Load chunks
	LoadFile world_file("res/data/world.dat");

	while (world_file.good())
	{
		// Load the key and name of the chunk
		CHUNK_KEY key = world_file.load_int();
		string name = world_file.load_string();

		chunks.emplace(key, name);
	}
}