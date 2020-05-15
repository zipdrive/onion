#pragma once

#include <unordered_map>
#include "matrix.h"


// The ID to an individual sprite
#define SPRITE_ID std::string
// The key to an individual sprite on a sprite sheet
#define SPRITE_KEY int

// The ID to an individual sprite texture
#define TEXTURE_ID std::string


#define PALETTE_MATRIX mat4x4f


// Holds palette matrices for use by shaders. These matrices map pure red, pure green, etc. values to base colors, highlights, and shadows.
class Palette
{
protected:
	/// <summary>Generates a palette matrix using integer values.</summary>
	/// <param name="red_maps_to">The color that the palette matrix should map RED to.</param>
	/// <param name="green_maps_to">The color that the palette matrix should map GREEN to.</param>
	/// <param name="blue_maps_to">The color that the palette matrix should map BLUE to.</param>
	/// <param name="matrix">A reference to the matrix that will store the palette information.</param>
	static void generate_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to, PALETTE_MATRIX& matrix);
	
	/// <summary>Generates a palette matrix using normalized values.</summary>
	/// <param name="red_maps_to">The color that the palette matrix should map RED to.</param>
	/// <param name="green_maps_to">The color that the palette matrix should map GREEN to.</param>
	/// <param name="blue_maps_to">The color that the palette matrix should map BLUE to.</param>
	/// <param name="matrix">A reference to the matrix that will store the palette information.</param>
	static void generate_palette_matrix(const vec4f& red_maps_to, const vec4f& green_maps_to, const vec4f& blue_maps_to, PALETTE_MATRIX& matrix);

public:
	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette matrix mapped to by RED.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	virtual const PALETTE_MATRIX& get_red_palette_matrix() const = 0;

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette matrix mapped to by GREEN.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	virtual const PALETTE_MATRIX& get_green_palette_matrix() const = 0;

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette matrix mapped to by BLUE.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	virtual const PALETTE_MATRIX& get_blue_palette_matrix() const = 0;
};

// A palette that always returns the same palette matrix.
class SinglePalette : public Palette
{
private:
	// The palette matrix
	PALETTE_MATRIX m_Matrix;

public:
	/// <summary>Constructs a single palette matrix using integer values.</summary>
	/// <param name="red_maps_to">The color that the palette matrix should map RED to.</param>
	/// <param name="green_maps_to">The color that the palette matrix should map GREEN to.</param>
	/// <param name="blue_maps_to">The color that the palette matrix should map BLUE to.</param>
	SinglePalette(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to);
	
	/// <summary>Constructs a single palette matrix using normalized values.</summary>
	/// <param name="red_maps_to">The color that the palette matrix should map RED to.</param>
	/// <param name="green_maps_to">The color that the palette matrix should map GREEN to.</param>
	/// <param name="blue_maps_to">The color that the palette matrix should map BLUE to.</param>
	SinglePalette(const vec4f& red_maps_to, const vec4f& green_maps_to, const vec4f& blue_maps_to);

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette mapped to by RED.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	const PALETTE_MATRIX& get_red_palette_matrix() const;

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette mapped to by GREEN.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	const PALETTE_MATRIX& get_green_palette_matrix() const;

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette mapped to by BLUE.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	const PALETTE_MATRIX& get_blue_palette_matrix() const;
};

// A palette that returns different palette matrices. Mainly used for texture-mapped sprites.
class MultiplePalette : public Palette
{
private:
	// The palette matrix that a texture-mapping maps RED to.
	PALETTE_MATRIX m_RedMatrix;

	// The palette matrix that a texture-mapping maps GREEN to.
	PALETTE_MATRIX m_GreenMatrix;

	// The palette matrix that a texture-mapping maps BLUE to.
	PALETTE_MATRIX m_BlueMatrix;

public:
	/// <summary>Constructs a blank palette matrix.</summary>
	MultiplePalette();

	/// <summary>Constructs a single palette matrix using integer values.</summary>
	/// <param name="red_maps_red_maps_to">The color that the palette matrix mapped to from RED by a texture-mapping to should map RED to.</param>
	/// <param name="red_maps_green_maps_to">The color that the palette matrix mapped to from RED by a texture-mapping should map GREEN to.</param>
	/// <param name="red_maps_blue_maps_to">The color that the palette matrix mapped to from RED by a texture-mapping should map BLUE to.</param>
	/// <param name="green_maps_red_maps_to">The color that the palette matrix mapped to from GREEN by a texture-mapping to should map RED to.</param>
	/// <param name="green_maps_green_maps_to">The color that the palette matrix mapped to from GREEN by a texture-mapping should map GREEN to.</param>
	/// <param name="green_maps_blue_maps_to">The color that the palette matrix mapped to from GREEN by a texture-mapping should map BLUE to.</param>
	/// <param name="blue_maps_red_maps_to">The color that the palette matrix mapped to from BLUE by a texture-mapping to should map RED to.</param>
	/// <param name="blue_maps_green_maps_to">The color that the palette matrix mapped to from BLUE by a texture-mapping should map GREEN to.</param>
	/// <param name="blue_maps_blue_maps_to">The color that the palette matrix mapped to from BLUE by a texture-mapping should map BLUE to.</param>
	MultiplePalette(const vec4i& red_maps_red_maps_to, const vec4i& red_maps_green_maps_to, const vec4i& red_maps_blue_maps_to,
		const vec4i& green_maps_red_maps_to, const vec4i& green_maps_green_maps_to, const vec4i& green_maps_blue_maps_to,
		const vec4i& blue_maps_red_maps_to, const vec4i& blue_maps_green_maps_to, const vec4i& blue_maps_blue_maps_to);

	/// <summary>Constructs a single palette matrix using normalized values.</summary>
	/// <param name="red_maps_red_maps_to">The color that the palette matrix mapped to from RED by a texture-mapping to should map RED to.</param>
	/// <param name="red_maps_green_maps_to">The color that the palette matrix mapped to from RED by a texture-mapping should map GREEN to.</param>
	/// <param name="red_maps_blue_maps_to">The color that the palette matrix mapped to from RED by a texture-mapping should map BLUE to.</param>
	/// <param name="green_maps_red_maps_to">The color that the palette matrix mapped to from GREEN by a texture-mapping to should map RED to.</param>
	/// <param name="green_maps_green_maps_to">The color that the palette matrix mapped to from GREEN by a texture-mapping should map GREEN to.</param>
	/// <param name="green_maps_blue_maps_to">The color that the palette matrix mapped to from GREEN by a texture-mapping should map BLUE to.</param>
	/// <param name="blue_maps_red_maps_to">The color that the palette matrix mapped to from BLUE by a texture-mapping to should map RED to.</param>
	/// <param name="blue_maps_green_maps_to">The color that the palette matrix mapped to from BLUE by a texture-mapping should map GREEN to.</param>
	/// <param name="blue_maps_blue_maps_to">The color that the palette matrix mapped to from BLUE by a texture-mapping should map BLUE to.</param>
	MultiplePalette(const vec4f& red_maps_red_maps_to, const vec4f& red_maps_green_maps_to, const vec4f& red_maps_blue_maps_to,
		const vec4f& green_maps_red_maps_to, const vec4f& green_maps_green_maps_to, const vec4f& green_maps_blue_maps_to,
		const vec4f& blue_maps_red_maps_to, const vec4f& blue_maps_green_maps_to, const vec4f& blue_maps_blue_maps_to);

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette mapped to by RED.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	const PALETTE_MATRIX& get_red_palette_matrix() const;

	/// <summary>Sets the palette matrix that will be mapped to by RED in a texture-mapped sprite.</summary>
	/// <param name="red_maps_to">The color mapped to by RED in the palette matrix.</param>
	/// <param name="green_maps_to">The color mapped to by GREEN in the palette matrix.</param>
	/// <param name="blue_maps_to">The color mapped to by BLUE in the palette matrix.</param>
	void set_red_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to);

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette mapped to by GREEN.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	const PALETTE_MATRIX& get_green_palette_matrix() const;

	/// <summary>Sets the palette matrix that will be mapped to by GREEN in a texture-mapped sprite.</summary>
	/// <param name="red_maps_to">The color mapped to by RED in the palette matrix.</param>
	/// <param name="green_maps_to">The color mapped to by GREEN in the palette matrix.</param>
	/// <param name="blue_maps_to">The color mapped to by BLUE in the palette matrix.</param>
	void set_green_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to);

	/// <summary>Retrieves a palette matrix. In a texture-mapped sprite, this will be the palette mapped to by BLUE.</summary>
	/// <returns>A constant reference to the palette matrix.</returns>
	const PALETTE_MATRIX& get_blue_palette_matrix() const;

	/// <summary>Sets the palette matrix that will be mapped to by BLUE in a texture-mapped sprite.</summary>
	/// <param name="red_maps_to">The color mapped to by RED in the palette matrix.</param>
	/// <param name="green_maps_to">The color mapped to by GREEN in the palette matrix.</param>
	/// <param name="blue_maps_to">The color mapped to by BLUE in the palette matrix.</param>
	void set_blue_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to);
};


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
	/// <param name="palette">The color palette of the sprite.</param>
	virtual void display(SPRITE_KEY sprite, const Palette* palette) = 0;
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

	// The color palette for the sprite
	const Palette* m_Palette;

	/// <summary>Constructs a graphic that draws sprites.</summary>
	/// <param name="sprite_sheet">The sprite sheet.</param>
	/// <param name="palette">The color palette for the sprites.</param>
	SpriteGraphic(SpriteSheet* sprite_sheet, const Palette* palette);

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
	/// <param name="palette">The color palette for the sprites.</param>
	StaticSpriteGraphic(SpriteSheet* sprite_sheet, Sprite* sprite, const Palette* palette);
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
	/// <param name="palette">The color palette for the sprites.</param>
	DynamicSpriteGraphic(SpriteSheet* sprite_sheet, const Palette* palette);

	/// <summary>Adds a new frame to the graphic.</summary>
	/// <param name="sprite">The frame's sprite.</param>
	void add_frame(Sprite* sprite);

	/// <summary>Sets the current frame.</summary>
	/// <param name="frame">The index of the frame.</param>
	void set_frame(int frame);
};



// A palette-setting texture that can be mapped onto a sprite.
struct Texture
{
private:
	static std::unordered_map<TEXTURE_ID, Texture*> m_Textures;

public:
	/// <summary>Retrieves the texture with the given ID.</summary>
	/// <param name="id">The ID of the texture.</param>
	static Texture* get_texture(TEXTURE_ID id);

	/// <summary>Sets the texture with the given ID.</summary>
	/// <param name="id">The ID of the texture.</param>
	/// <param name="texture">The texture to set.</param>
	static void set_texture(TEXTURE_ID id, Texture* texture);

	// A matrix that projects a vec4f into a set of texture coordinates.
	mat2x4f transform;

	/// <summary></summary>
	Texture(const mat2x4f& trans);
};

// A sprite sheet that maps textures onto sprites.
class TextureMapSpriteSheet
{
protected:
	// Protected so that it cannot be called from the outside.
	TextureMapSpriteSheet();

public:
	/// <summary>Loads a sprite sheet from file.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base.</param>
	/// <returns>A pointer to the loaded sprite sheet.</returns>
	static TextureMapSpriteSheet* generate(const char* path);

	/// <summary>Generates an empty sprite sheet.</summary>
	/// <returns>A pointer to an empty sprite sheet.</returns>
	static TextureMapSpriteSheet* generate_empty();

	// The width of the sprite sheet in pixels.
	int width;

	// The height of the sprite sheet in pixels.
	int height;

	/// <summary>Loads sprite sheet from an image and meta file.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base. Note: The path to the meta file should be the same as the path to the image file, but with a .meta file extension instead.</param>
	virtual void load_sprite_sheet(const char* path) = 0;

	/// <summary>Draws a sprite from the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	/// <param name="palette">The color palette of the sprite.</param>
	/// <param name="texture">The texture mapped onto the sprite.</param>
	virtual void display(SPRITE_KEY sprite, const mat2x4f& texture, const Palette* palette) const = 0;
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

	/// <summary>Calculates the width of a line of text.</summary>
	/// <param name="text">The line of text.</param>
	/// <returns>The width of the line, as displayed in the font.</returns>
	virtual int get_line_width(std::string line) const = 0;

	/// <summary>Retrieves the height of the font.</summary>
	/// <returns>The height of text, as displayed by the font.</returns>
	virtual int get_line_height() const = 0;

	/// <summary>Displays a line of text.</summary>
	/// <param name="text">The line of text to display.</param>
	/// <param name="palette">The color palette of the text.</param>
	virtual void display_line(std::string line, const Palette* palette) = 0;
};



// Displays a line of text.
class TextGraphic : public Graphic
{
private:
	// The lines of text to display
	std::vector<std::string> m_Lines;

	// The maximum width of a line of text, in pixels
	int m_Width;

	// The font to use
	Font* m_Font;

	// The palette of the font
	const Palette* m_Palette;

public:
	/// <summary>Constructs a graphic that displays text.</summary>
	/// <param name="font">The font of the graphic.</param>
	/// <param name="palette">The color palette of the graphic.</param>
	/// <param name="text">The text of the graphic.</param>
	TextGraphic(Font* font, const Palette* palette, std::string text, int width);

	/// <summary>Retrieves the text of the graphic.</summary>
	/// <returns>The text displayed by the graphic.</returns>
	std::string get_text() const;

	/// <summary>Sets the text to be displayed by the graphic.</summary>
	/// <returns>The text to be displayed by the graphic.</returns>
	void set_text(std::string text);

	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	int get_width() const;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	int get_height() const;

	/// <summary>Displays the text.</summary>
	void display() const;
};