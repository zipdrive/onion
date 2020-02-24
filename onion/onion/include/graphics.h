#pragma once

#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#define SPRITE_SHEET_KEY int
#define SPRITE_KEY int



// Something visible on-screen.
class Graphic
{
public:
	/// <summary>Draws the graphic to the screen.</summary>
	virtual void display() = 0;
};



// Data about an individual sprite on a sprite sheet.
struct SpriteInfo
{
	// The key to draw the sprite.
	SPRITE_KEY key;

	// The distance from the left edge of the sprite to the left edge of the sprite sheet, in pixels.
	int left;

	// The distance from the top edge of the sprite to the top edge of the sprite sheet, in pixels.
	int top;

	// The width of the sprite, in pixels.
	int width;

	// The height of the sprite, in pixels.
	int height;

	/// <summary>Initializes the data about the individual sprite.</summary>
	/// <param name="left">The distance from the left edge of the sprite to the left edge of the sprite sheet, in pixels.</param>
	/// <param name="top">The distance from the top edge of the sprite to the top edge of the sprite sheet, in pixels.</param>
	/// <param name="width">The width of the sprite, in pixels.</param>
	/// <param name="height">The height of the sprite, in pixels.</param>
	SpriteInfo(int left, int top, int width, int height);
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

	/// <summary>Creates an empty sprite sheet.</summary>
	SpriteSheet();

	/// <summary>Loads sprite sheet from file.</summary>
	/// <param name="file">The file path.</param>
	SpriteSheet(const char* file);

	/// <summary>Indicates if individual sprites have been loaded or not.</summary>
	/// <returns>True if individual sprites have been loaded, false otherwise.</returns>
	bool sprites_loaded();

	/// <summary>Loads sprite sheet from file.</summary>
	/// <param name="file">The file path.</param>
	void load_sprite_sheet(const char* file);

	/// <summary>Loads individual sprites from a vector of data.</summary>
	/// <param name="sprites">The data about the sprites.</param>
	void load_sprites(std::vector<SpriteInfo*>& sprites);

	/// <summary>Draws a sprite on the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	void display(SPRITE_KEY sprite);
};


class StaticSprite : Graphic
{
private:
	SpriteSheet* m_SpriteSheet;

	SPRITE_KEY m_SpriteKey;

public:
	void display();
};

class DynamicSprite : Graphic
{
private:
	SpriteSheet* m_SpriteSheet;

	SPRITE_KEY* m_SpriteKeys;

public:
	int frame;

	void display();
};