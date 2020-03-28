#pragma once

#include <unordered_map>
#include "matrix.h"


// The ID to an individual sprite
#define SPRITE_ID int16_t
// The key to an individual sprite on a sprite sheet
#define SPRITE_KEY int



/// <summary>Generates a palette matrix for use in graphics.</summary>
/// <param name="rr">The red-value of the color that red maps to.</param>
/// <param name="rg">The green-value of the color that red maps to.</param>
/// <param name="rb">The blue-value of the color that red maps to.</param>
/// <param name="ra">The alpha-value of the color that red maps to.</param>
/// <param name="gr">The red-value of the color that green maps to.</param>
/// <param name="gg">The green-value of the color that green maps to.</param>
/// <param name="gb">The blue-value of the color that green maps to.</param>
/// <param name="ga">The alpha-value of the color that green maps to.</param>
/// <param name="br">The red-value of the color that blue maps to.</param>
/// <param name="bg">The green-value of the color that blue maps to.</param>
/// <param name="bb">The blue-value of the color that blue maps to.</param>
/// <param name="ba">The alpha-value of the color that blue maps to.</param>
/// <param name="ar">The red-value of the color that alpha maps to.</param>
/// <param name="ag">The green-value of the color that alpha maps to.</param>
/// <param name="ab">The blue-value of the color that alpha maps to.</param>
/// <param name="aa">The alpha-value of the color that alpha maps to.</param>
mat4x4f generate_palette_matrix(
	int rr = 255, int rg = 0, int rb = 0, int ra = 0,
	int gr = 0, int gg = 255, int gb = 0, int ga = 0,
	int br = 0, int bg = 0, int bb = 255, int ba = 0,
	int ar = 0, int ag = 0, int ab = 0, int aa = 255
);


// Something visible on-screen.
class Graphic
{
public:
	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	virtual int get_width() const = 0;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	virtual int get_height() const = 0;

	/// <summary>Draws the graphic to the screen.</summary>
	virtual void display() const = 0;
};


/// <summary>Creates a solid color graphic.</summary>
/// <param name="r">The red value.</param>
/// <param name="g">The green value.</param>
/// <param name="b">The blue value.</param>
/// <param name="a">The alpha value.</param>
/// <param name="width">The width of the graphic.</param>
/// <param name="height">The height of the graphic.</param>
Graphic* generate_solid_color_graphic(int r, int g, int b, int a, int width, int height);

/// <summary>Creates a solid color graphic.</summary>
/// <param name="r">The red value.</param>
/// <param name="g">The green value.</param>
/// <param name="b">The blue value.</param>
/// <param name="a">The alpha value.</param>
/// <param name="width">The width of the graphic.</param>
/// <param name="height">The height of the graphic.</param>
Graphic* generate_solid_color_graphic(float r, float g, float b, float a, int width, int height);


// Contains multiple sprites on a single texture.
class SpriteSheet
{
protected:
	// Protected so that it cannot be called from the outside.
	SpriteSheet();

public:
	/// <summary>Generates an empty sprite sheet.</summary>
	/// <returns>A pointer to an empty sprite sheet.</returns>
	static SpriteSheet* generate_empty();

	// The width of the sprite sheet in pixels.
	int width;

	// The height of the sprite sheet in pixels.
	int height;

	/// <summary>Loads sprite sheet from an image and meta file.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base. Note: The path to the meta file should be the same as the path to the image file, but with a .meta file extension instead.</param>
	virtual void load_sprite_sheet(const char* path) = 0;

	/// <summary>Loads sprite sheet from an image file, and equally partitions it into sprites.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base.</param>
	/// <param name="partition_width">The width of the individual sprites.</param>
	/// <param name="partition_height">The height of the individual sprites.</param>
	virtual void load_partitioned_sprite_sheet(const char* path, int partition_width, int partition_height) = 0;

	/// <summary>Draws a sprite from the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	/// <param name="color">The color tint matrix of the sprite.</param>
	virtual void display(SPRITE_KEY sprite, const mat4x4f& color) = 0;
};


// An individual sprite on a sprite sheet.
struct Sprite
{
private:
	static std::unordered_map<SPRITE_ID, Sprite*> m_Sprites;

public:
	/// <summary>Retrieves the sprite with the given ID.</summary>
	/// <param name="id">The ID of the sprite.</param>
	static Sprite* get_sprite(SPRITE_ID id);

	/// <summary>Sets the sprite with the given ID.</summary>
	/// <param name="id">The ID of the sprite.</param>
	/// <param name="sprite">The sprite to set.</param>
	static void set_sprite(SPRITE_ID id, Sprite* sprite);


	// The key of the sprite
	SPRITE_KEY key;

	// The width of the sprite
	int width;

	// The height of the sprite
	int height;

	/// <summary>Constructs sprite information.</summary>
	/// <param name="key">The key of the sprite.</param>
	/// <param name="width">The width of the sprite.</param>
	/// <param name="height">The height of the sprite.</param>
	Sprite(SPRITE_KEY key, int width, int height);
};

class SpriteGraphic : public Graphic
{
protected:
	// A pointer to the sprite information
	SpriteSheet* m_SpriteSheet;

	// The color tint matrix for the sprite
	mat4x4f m_TintMatrix;

	/// <summary>Constructs a graphic that draws sprites.</summary>
	/// <param name="sprite_sheet">The sprite sheet.</param>
	/// <param name="color">The color tint matrix for the sprites.</param>
	SpriteGraphic(SpriteSheet* sprite_sheet, mat4x4f& color);

	/// <summary>Retrieves the current sprite to be drawn.</summary>
	/// <returns>The current sprite.</returns>
	virtual Sprite* get_sprite() const = 0;

public:
	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	int get_width() const;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	int get_height() const;

	/// <summary>Displays the sprite with the given key.</summary>
	void display() const;
};

// A graphic that always displays the same sprite.
class StaticSpriteGraphic : public SpriteGraphic
{
private:
	// The key to the sprite
	Sprite* m_Sprite;

	/// <summary>Retrieves the current sprite to be drawn.</summary>
	/// <returns>The key of the current sprite.</returns>
	Sprite* get_sprite() const;

public:
	/// <summary>Constructs a static sprite graphic.</summary>
	/// <param name="sprite_sheet">The sprite sheet that the sprites are on.</param>
	/// <param name="sprite">The sprite data.</param>
	/// <param name="color">The color tint matrix for the sprites.</param>
	StaticSpriteGraphic(SpriteSheet* sprite_sheet, Sprite* sprite, mat4x4f& color);
};

// A graphic that can swap between multiple frames of sprites.
class DynamicSpriteGraphic : public SpriteGraphic
{
private:
	// The keys to the sprites
	std::vector<Sprite*> m_Sprites;

	// The current frame
	int m_Current;

	/// <summary>Retrieves the current sprite to be drawn.</summary>
	/// <returns>The key of the current sprite.</returns>
	Sprite* get_sprite() const;

public:
	/// <summary>Constructs a static sprite graphic.</summary>
	/// <param name="sprite_sheet">The sprite sheet that the sprites are on.</param>
	/// <param name="color">The color tint matrix for the sprites.</param>
	DynamicSpriteGraphic(SpriteSheet* sprite_sheet, mat4x4f& color);

	/// <summary>Adds a new frame to the graphic.</summary>
	/// <param name="sprite">The frame's sprite.</param>
	void add_frame(Sprite* sprite);

	/// <summary>Sets the current frame.</summary>
	/// <param name="frame">The index of the frame.</param>
	void set_frame(int frame);
};


class Font
{
protected:
	// A map from individual characters to the sprite for that character.
	std::unordered_map<char, Sprite> m_Characters;

public:
	/// <summary>Loads a font from an image and meta file.</summary>
	/// <param name="path">The path to the image file, from the res/img/ folder.</param>
	static Font* load_sprite_font(const char* path);

	/// <summary>Displays a string of text.</summary>
	/// <param name="text">The text to display.</param>
	virtual void display(std::string text) = 0;
};