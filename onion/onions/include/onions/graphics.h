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
	/// <summary>Destroys the graphic.</summary>
	virtual ~Graphic() {}

	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	virtual int get_width() const = 0;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	virtual int get_height() const = 0;

	/// <summary>Draws the graphic to the screen.</summary>
	virtual void display() const = 0;
};



class SolidColorGraphic : public Graphic
{
protected:
	/// <summary>Protected so it can't be called from the outside.</summary>
	/// <param name="color">The color of the graphic.</param>
	/// <param name="width">The width of the graphic.</param>
	/// <param name="height">The height of the graphic.</param>
	SolidColorGraphic(const vec4f& color, int width, int height);

public:
	/// <summary>Creates a solid color graphic.</summary>
	/// <param name="r">The red value, from 0 to 255.</param>
	/// <param name="g">The green value, from 0 to 255.</param>
	/// <param name="b">The blue value, from 0 to 255.</param>
	/// <param name="a">The alpha value, from 0 to 255.</param>
	/// <param name="width">The width of the graphic.</param>
	/// <param name="height">The height of the graphic.</param>
	static SolidColorGraphic* generate(int r, int g, int b, int a, int width, int height);

	/// <summary>Creates a solid color graphic.</summary>
	/// <param name="r">The red value, from 0 to 1.</param>
	/// <param name="g">The green value, from 0 to 1.</param>
	/// <param name="b">The blue value, from 0 to 1.</param>
	/// <param name="a">The alpha value, from 0 to 1.</param>
	/// <param name="width">The width of the graphic.</param>
	/// <param name="height">The height of the graphic.</param>
	static SolidColorGraphic* generate(float r, float g, float b, float a, int width, int height);

	// The color of the graphic
	vec4f color;

	// The width of the sprite
	int width;

	// The height of the sprite
	int height;

	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	int get_width() const;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	int get_height() const;
};


class SlicedGraphic : public Graphic
{
protected:
	// The graphics of the sliced
	Graphic* m_Graphics[9];

public:
	// The width of the graphic
	int width;

	// The height of the graphic
	int height;

	/// <summary>Constructs a sliced graphic.</summary>
	/// <param name="top_left">The top left corner graphic.</param>
	/// <param name="top">The center top graphic.</param>
	/// <param name="top_right">The top right corner graphic.</param>
	/// <param name="left">The center left graphic.</param>
	/// <param name="center">The center graphic.</param>
	/// <param name="right">The center right graphic.</param>
	/// <param name="bottom_left">The bottom left corner graphic.</param>
	/// <param name="bottom">The center bottom graphic.</param>
	/// <param name="bottom_right">The bottom right corner graphic.</param>
	/// <param name="width">The width of the graphic.</param>
	/// <param name="height">The height of the graphic.</param>
	SlicedGraphic(
		Graphic* top_left, Graphic* top, Graphic* top_right,
		Graphic* left, Graphic* center, Graphic* right,
		Graphic* bottom_left, Graphic* bottom, Graphic* bottom_right,
		int width, int height
	);

	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	int get_width() const;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	int get_height() const;

	/// <summary>Displays the graphic.</summary>
	void display() const;
};


// Contains multiple sprites on a single texture.
class SpriteSheet
{
protected:
	// Protected so that it cannot be called from the outside.
	SpriteSheet();

public:
	/// <summary>Loads a sprite sheet from file.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base.</param>
	/// <returns>A pointer to the loaded sprite sheet.</returns>
	static SpriteSheet* generate(const char* path);

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
	SpriteGraphic(SpriteSheet* sprite_sheet, const mat4x4f& color);

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
	StaticSpriteGraphic(SpriteSheet* sprite_sheet, Sprite* sprite, const mat4x4f& color);
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
	DynamicSpriteGraphic(SpriteSheet* sprite_sheet, const mat4x4f& color);

	/// <summary>Adds a new frame to the graphic.</summary>
	/// <param name="sprite">The frame's sprite.</param>
	void add_frame(Sprite* sprite);

	/// <summary>Sets the current frame.</summary>
	/// <param name="frame">The index of the frame.</param>
	void set_frame(int frame);
};



// A font that displays text
class Font
{
protected:
	// A map from individual characters to the sprite for that character.
	std::unordered_map<char, Sprite> m_Characters;

	// The separation between individual characters.
	int m_Kerning = -1;

public:
	/// <summary>Loads a font from an image and meta file.</summary>
	/// <param name="path">The path to the image file, from the res/img/ folder.</param>
	static Font* load_sprite_font(const char* path);

	/// <summary>Displays a line of text.</summary>
	/// <param name="text">The line of text to display.</param>
	/// <param name="color">The color palette of the text.</param>
	virtual void display_line(std::string line, const mat4x4f& color) = 0;

	/// <summary>Displays a string of text.</summary>
	/// <param name="text">The text to display.</param>
	/// <param name="color">The color palette of the text.</param>
	/// <param name="width">The maximum width of the text.</param>
	virtual void display_paragraph(std::string text, const mat4x4f& color, int width = INT16_MAX) = 0;
};


// Displays a line of text.
class TextLineGraphic : public Graphic
{
private:
	// The font to use
	Font* m_Font;

	// The palette of the font
	mat4x4f m_Color;

public:
	// The line of text to display
	std::string text;

	/// <summary>Constructs a graphic that displays text.</summary>
	/// <param name="font">The font of the graphic.</param>
	/// <param name="text">The text of the graphic.</param>
	/// <param name="color">The color palette of the graphic.</param>
	TextLineGraphic(Font* font, std::string text, const mat4x4f& color);

	/// <summary>Displays the text.</summary>
	void display() const;
};