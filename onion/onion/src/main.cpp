#include <onion.h>
#include "../include/charactercreator.h"
#include "../include/test.h"

#include <iostream>

using namespace onion;

/*
#define CONTROL_DEBUG 0

class KeyboardControlTester : public KeyboardListener
{
public:
	int trigger(const KeyEvent& event_data)
	{
		if (event_data.pressed)
		{
			if (event_data.control == CONTROL_DEBUG)
			{
				std::cout << "Debug key pressed.\n";
			}
			else
			{
				std::cout << "Unknown key pressed.\n";
			}
		}
		return EVENT_CONTINUE;
	}
} g_KeyboardTester;

void test_key_main()
{
	register_keyboard_control(CONTROL_DEBUG);
	assign_key(CONTROL_DEBUG);

	g_KeyboardTester.unfreeze(0);
}


class UpdateTester : public UpdateListener
{
protected:
	void update(int frames_passed) {}

} g_UpdateTester;

void test_update_main()
{
	g_UpdateTester.unfreeze(0);
}
*/


mat4x4f g_Transform;


SimplePixelSpriteSheet* g_TestAlphaSpriteSheet;
Palette* g_TestAlphaPalette;

void test_alpha_display()
{
	Transform::model.push();
	Transform::model.translate(100.f, 100.f);
	g_TestAlphaSpriteSheet->display(0, g_TestAlphaPalette->get_red_palette_matrix(), 0);

	Transform::model.translate(-64.f, 64.f, -0.01f);
	g_TestAlphaSpriteSheet->display(12, g_TestAlphaPalette->get_red_palette_matrix(), 0);
	Transform::model.pop();
}

void test_alpha_main()
{
	// Load sprite sheet
	g_TestAlphaSpriteSheet = new SimplePixelSpriteSheet();
	g_TestAlphaSpriteSheet->load("test/alpha.png", 128, 128);

	// Load palette
	g_TestAlphaPalette = new SinglePalette(
		vec4i(255, 0, 0, 0), vec4i(0, 255, 0, 0), vec4i(0, 0, 255, 0)
	);

	// Call the main function
	onion::main(test_alpha_display);

	// Clean up
	delete g_TestAlphaSpriteSheet;
	g_TestAlphaSpriteSheet = nullptr;
}


SimplePixelSpriteSheet* g_TestSpriteSheet;
Sprite* g_TestSprite;
Palette* g_TestSpritePalette;

void test_sprite_display()
{
	g_TestSpriteSheet->display(g_TestSprite, g_TestSpritePalette);
}

void test_sprite_main()
{
	// Load sprites
	g_TestSpriteSheet = new SimplePixelSpriteSheet("test/texmap.png");
	g_TestSprite = g_TestSpriteSheet->get_sprite("test");

	// Load palette
	g_TestSpritePalette = new SinglePalette(
		vec4i(255, 0, 0, 255), vec4i(0, 255, 0, 255), vec4i(0, 0, 255, 255)
	);

	// Call the main function
	onion::main(test_sprite_display);

	// Clean up
	delete g_TestSpriteSheet;
	g_TestSpriteSheet = nullptr;
}



ShadedTexturePixelSpriteSheet* g_TestTexmapSpriteSheet;
Sprite* g_TestTexmapSprite;
Texture* g_TestTexmapTexture;
Palette* g_TestTexmapPalette;

void test_texmap_display()
{
	g_TestTexmapSpriteSheet->display(g_TestTexmapSprite, false, g_TestTexmapTexture, g_TestTexmapPalette);
}

void test_texmap_main()
{
	// Load sprites
	g_TestTexmapSpriteSheet = new ShadedTexturePixelSpriteSheet("test/texmap.png");
	g_TestTexmapSprite = g_TestTexmapSpriteSheet->get_sprite("test");
	g_TestTexmapTexture = g_TestTexmapSpriteSheet->get_texture("test");

	// Load palette
	g_TestTexmapPalette = new MultiplePalette(
		vec4i(255, 0, 0, 255), vec4i(255, 128, 128, 255), vec4i(128, 0, 0, 255),
		vec4i(0, 255, 0, 255), vec4i(128, 255, 128, 255), vec4i(0, 128, 0, 255),
		vec4i(0, 0, 255, 255), vec4i(128, 128, 255, 255), vec4i(0, 0, 128, 255)
	);

	// Call the main function
	onion::main(test_texmap_display);

	// Clean up
	delete g_TestTexmapSpriteSheet;
	g_TestTexmapSpriteSheet = nullptr;
}



world::HuneGraphic* g_TestHune = nullptr;

void test_hune_display()
{
	//g_TestHune->display();
	// TODO
}

void test_hune_main()
{
	// Load graphic
	g_TestHune = new world::HuneGraphic();

	g_TestHune->set_body_type("lean");
	g_TestHune->set_head_shape("thin");
	g_TestHune->set_snout_shape("nose long");
	g_TestHune->set_upper_head_feature("small");

	g_TestHune->set_textures("head striped", "monochrome", "monochrome", "monochrome");

	g_TestHune->set_primary_color(vec4i(244, 151, 31, 255), vec4i(255, 255, 220, 255), vec4i(0, 0, 25, 255));
	g_TestHune->set_secondary_color(vec4i(243, 236, 226, 255), vec4i(255, 255, 220, 255), vec4i(0, 0, 25, 255));
	g_TestHune->set_tertiary_color(vec4i(142, 108, 17, 255), vec4i(255, 255, 220, 255), vec4i(0, 0, 20, 255));
	//g_TestHune->set_primary_color(vec4i(133, 76, 25, 255), vec4i(244, 243, 234, 255), vec4i(25, 2, 25, 255));

	g_TestHune->direction = vec2i(0, -1);

	// Call the main function
	onion::main(test_hune_display);

	// Clean up
	delete g_TestHune;
	g_TestHune = nullptr;
}


Font* g_TestFont;
Palette* g_TestFontPalette;

void test_font_display()
{
	g_TestFont->display_line("djskfdjsldfkb", g_TestFontPalette);
}

void test_font_main()
{
	// Create the font
	g_TestFont = new SpriteFont("outline11.png");

	// Create the palette
	g_TestFontPalette = new SinglePalette(vec4i(255, 0, 0, 0), vec4i(0, 255, 0, 0), vec4i(0, 0, 255, 0));

	// Call the main function
	onion::main(test_font_display);

	// Clean up
	delete g_TestFont;
	delete g_TestFontPalette;
}



void convert_hune()
{
	LoadFile loader("res/img/sprites/hune.meta.txt");
	SaveFile saver("res/img/sprites/hune.meta");

	std::regex texture("^texture\\s+(.*)");

	while (loader.good())
	{
		StringData line;
		String id = loader.load_data(line);

		if (!id.empty())
		{
			StringData newline;

			std::smatch match;
			if (std::regex_match(id, match, texture))
			{
				// Load a texture
				vec2i size, pos;

				line.get("width", size(0));
				line.get("height", size(1));
				line.get("left", pos(0));
				line.get("right", pos(1));

				newline.set("pos", pos);
				newline.set("size", size);
			}
			else
			{
				// Load a sprite
				vec2i size, shading, mapping;

				line.get("width", size(0));
				line.get("height", size(1));
				line.get("shading_left", shading(0));
				line.get("shading_top", shading(1));
				line.get("mapping_left", mapping(0));
				line.get("mapping_top", mapping(1));

				newline.set("shading", shading);
				newline.set("mapping", mapping);
				newline.set("size", size);
			}

			saver.save_data(id, newline);
		}
	}
}



// The entry point for the program.
int main()
{
	init("settings.ini");

	worldtest_main();
	//character_creator_setup();
	return 0;
}