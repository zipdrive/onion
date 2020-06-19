#include <onion.h>
#include "../include/charactercreator.h"


mat4x4f g_Transform;


SpriteSheet* g_TestSpriteSheet;
Sprite* g_TestSprite;
Palette* g_TestSpritePalette;

void test_sprite_display()
{
	g_TestSpriteSheet->display(g_TestSprite->key, g_TestSpritePalette);
}

void test_sprite_main()
{
	// Load sprites
	g_TestSpriteSheet = SpriteSheet::generate("test/texmap.png");
	g_TestSprite = Sprite::get_sprite("test");

	// Load palette
	g_TestSpritePalette = new SinglePalette(
		vec4i(255, 0, 0, 255), vec4i(0, 255, 0, 255), vec4i(0, 0, 255, 255)
	);

	// Construct transformation
	Application* app = get_application_settings();
	g_Transform.set(0, 0, 2.f / app->width);
	g_Transform.set(1, 1, 2.f / app->height);

	mat_push();
	mat_custom_transform(g_Transform);

	// Call the main function
	onion_main(test_sprite_display);

	// Clean up
	delete g_TestSpriteSheet;
	g_TestSpriteSheet = nullptr;
	mat_pop();
}



TextureMapSpriteSheet* g_TestTexmapSpriteSheet;
Sprite* g_TestTexmapSprite;
Texture* g_TestTexmapTexture;
Palette* g_TestTexmapPalette;

void test_texmap_display()
{
	g_TestTexmapSpriteSheet->display(g_TestTexmapSprite->key, g_TestTexmapTexture->transform, g_TestTexmapPalette);
}

void test_texmap_main()
{
	// Load sprites
	g_TestTexmapSpriteSheet = TextureMapSpriteSheet::generate("test/texmap.png");
	g_TestTexmapSprite = Sprite::get_sprite("test");
	g_TestTexmapTexture = Texture::get_texture("test");

	// Load palette
	g_TestTexmapPalette = new MultiplePalette(
		vec4i(255, 0, 0, 255), vec4i(255, 128, 128, 255), vec4i(128, 0, 0, 255),
		vec4i(0, 255, 0, 255), vec4i(128, 255, 128, 255), vec4i(0, 128, 0, 255),
		vec4i(0, 0, 255, 255), vec4i(128, 128, 255, 255), vec4i(0, 0, 128, 255)
	);

	// Construct transformation
	Application* app = get_application_settings();
	g_Transform.set(0, 0, 2.f / app->width);
	g_Transform.set(1, 1, 2.f / app->height);

	mat_push();
	mat_custom_transform(g_Transform);

	// Call the main function
	onion_main(test_texmap_display);

	// Clean up
	delete g_TestTexmapSpriteSheet;
	mat_pop();
	g_TestTexmapSpriteSheet = nullptr;
}



HuneGraphic* g_TestHune = nullptr;

void test_hune_display()
{
	g_TestHune->display();
}

void test_hune_main()
{
	// Load graphic
	g_TestHune = new HuneGraphic();

	g_TestHune->set_body_type("lean");
	g_TestHune->set_head_shape("thin");
	g_TestHune->set_snout_shape("nose long");
	g_TestHune->set_ear_shape("small");

	g_TestHune->set_textures("head striped", "monochrome", "monochrome", "monochrome");

	g_TestHune->set_primary_color(vec4i(244, 151, 31, 255), vec4i(255, 255, 220, 255), vec4i(0, 0, 25, 255));
	g_TestHune->set_secondary_color(vec4i(243, 236, 226, 255), vec4i(255, 255, 220, 255), vec4i(0, 0, 25, 255));
	g_TestHune->set_tertiary_color(vec4i(142, 108, 17, 255), vec4i(255, 255, 220, 255), vec4i(0, 0, 20, 255));
	//g_TestHune->set_primary_color(vec4i(133, 76, 25, 255), vec4i(244, 243, 234, 255), vec4i(25, 2, 25, 255));

	g_TestHune->facing = FACING_BACK;

	// Construct transformation
	Application* app = get_application_settings();
	g_Transform.set(0, 0, 6.f / app->width);
	g_Transform.set(1, 1, 6.f / app->height);

	mat_push();
	mat_custom_transform(g_Transform);

	// Call the main function
	onion_main(test_hune_display);

	// Clean up
	delete g_TestHune;
	g_TestHune = nullptr;
	mat_pop();
}



// The entry point for the program.
int main()
{
	onion_init("settings.ini");
	character_creator_setup();
	return 0;
}