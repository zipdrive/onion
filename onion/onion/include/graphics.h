#pragma once
#include <vector>
#include <GLFW\glfw3.h>


#define SPRITE_SHEET_KEY int
#define SPRITE_KEY int



struct Sprite
{
	// The distance from the left edge of the sprite to the left edge of the sprite sheet, in pixels.
	int left;

	// The distance from the top edge of the sprite to the top edge of the sprite sheet, in pixels.
	int top;

	// The width of the sprite, in pixels.
	int width;

	// The height of the sprite, in pixels.
	int height;
};


// Contains multiple sprites on a single texture.
class SpriteSheet
{
private:
	// The ID for which sprite sheet is currently being drawn from.
	static SPRITE_SHEET_KEY m_Enabled;

	// The next available sprite sheet ID.
	static SPRITE_SHEET_KEY m_NextAvailableID;

	// The ID for this sprite sheet.
	SPRITE_SHEET_KEY m_ID;


	// The OpenGL ID for the sprite shader program.
	static GLuint m_SpriteShader;

	// The OpenGL ID for the MVP in the sprite shader program.
	static GLuint m_SpriteShaderMVP;


	// The OpenGL ID for the texture.
	GLuint m_TextureID;

	// An array that contains information about the sprites.
	GLuint m_SpriteBuffer;

public:
	// The width of the sprite sheet in pixels.
	int width;

	// The height of the sprite sheet in pixels.
	int height;

	/// <summary>Loads sprite sheet from file.</summary>
	/// <param name="file">The file path.</param>
	SpriteSheet(const char* file);

	/// <summary>Creates all sprites. ONLY CALL THIS FUNCTION ONCE.</summary>
	/// <param name="sprites">The data about the sprites.</param>
	/// <param name="sprite_keys">An empty vector to be filled with the sprite keys.</param>
	void add_sprites(std::vector<Sprite>& sprites, std::vector<SPRITE_KEY>& sprite_keys);

	/// <summary>Draws a sprite on the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	void display(SPRITE_KEY sprite);
};