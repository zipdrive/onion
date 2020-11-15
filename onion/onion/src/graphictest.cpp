#include <onions/matrix.h>
#include <onions/graphics/sprite.h>
#include <onions/application.h>
#include "../include/test.h"

#define TEST_SHADED_TEXTURE

using namespace onion;


mat4x4f g_TestTransform;

Palette* g_Palette;


#ifdef TEST_SHADED_TEXTURE
ShadedTexturePixelSpriteSheet* g_SpriteSheet;
Sprite* g_Sprite;
Texture* g_Texture;
#else 
SimplePixelSpriteSheet* g_SpriteSheet;
Sprite* g_Sprite;
#endif


void graphictest_display()
{
#ifdef TEST_SHADED_TEXTURE
	g_SpriteSheet->display(g_Sprite, false, g_Texture, g_Palette);
#else 
	g_SpriteSheet->display(g_Sprite, &g_PaletteMatrix);
#endif
}

void graphictest_main()
{
	// Construct palette
	g_Palette = new SinglePalette(
		vec4i(255, 0, 0, 0),
		vec4i(0, 255, 0, 0),
		vec4i(0, 0, 255, 0)
	);

	// Load the sprite sheet
#ifdef TEST_SHADED_TEXTURE
	g_SpriteSheet = new ShadedTexturePixelSpriteSheet("test/texmap.png");
	g_Sprite = g_SpriteSheet->get_sprite("test");
	g_Texture = g_SpriteSheet->get_texture("test");
#else
	g_SpriteSheet = new SimplePixelSpriteSheet("ui/basic.png");
	g_Sprite = g_SpriteSheet->get_sprite("bg diamonds");
#endif

	// Run the main loop
	main(graphictest_display);

	// Clean up
	delete g_SpriteSheet;
	delete g_Palette;
}