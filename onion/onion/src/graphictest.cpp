#include <onions/matrix.h>
#include <onions/graphics/sprite.h>
#include <onions/application.h>
#include "../include/graphictest.h"

#define TEST_SHADED_TEXTURE

using namespace onion;


mat4x4f g_TestTransform;

PALETTE_MATRIX g_PaletteMatrix;


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
	g_SpriteSheet->display(g_Sprite, &g_Texture->tex, &g_PaletteMatrix, &g_PaletteMatrix, &g_PaletteMatrix);
#else 
	g_SpriteSheet->display(g_Sprite, &g_PaletteMatrix);
#endif
}

void graphictest_main()
{
	// Construct transformation
	Application* app = get_application_settings();
	g_TestTransform.set(0, 0, 2.f / app->width);
	g_TestTransform.set(1, 1, 2.f / app->height);

	mat_push();
	mat_custom_transform(g_TestTransform);

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
	mat_pop();
}