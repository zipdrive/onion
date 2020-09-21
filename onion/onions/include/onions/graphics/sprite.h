#pragma once
#include "shader.h"
#include "../fileio.h"
#include "../matrix.h"

namespace onion
{

	// Manages a key-value map, where each value is a pointer created with new.
	template <typename _Key, typename _Value>
	class _Manager
	{
	private:
		// The values being managed.
		std::unordered_map<_Key, _Value*> m_Values;

	public:
		/// <summary>Frees the memory of all managed values.</summary>
		void clear()
		{
			for (auto iter = m_Values.begin(); iter != m_Values.end(); ++iter)
			{
				if (iter->second)
					delete iter->second;
			}

			m_Values.clear();
		}

		/// <summary>Retrieves the value associated with the given key.</summary>
		/// <param name="key">The key to retrieve the value of.</param>
		/// <returns>The value associated with the key, if there is one. NULL otherwise.</returns>
		_Value* get(_Key key)
		{
			auto iter = m_Values.find(key);
			if (iter != m_Values.end())
			{
				return iter->second;
			}
			return nullptr;
		}

		/// <summary>Retrieves the value associated with the given key.</summary>
		/// <param name="key">The key to retrieve the value of.</param>
		/// <returns>The value associated with the key, if there is one. NULL otherwise.</returns>
		const _Value* get(_Key key) const
		{
			auto iter = m_Values.find(key);
			if (iter != m_Values.end())
			{
				return iter->second;
			}
			return nullptr;
		}

		/// <summary>Sets the value of the given key.</summary>
		/// <param name="key">The key to assign a value to.</param>
		/// <param name="value">The value to assign to the given key.</param>
		void set(_Key key, _Value* value)
		{
			auto iter = m_Values.find(key);
			if (iter == m_Values.end())
			{
				m_Values.emplace(key, value);
			}
			else
			{
				delete iter->second;
				m_Values.erase(iter);
				m_Values.emplace_hint(iter, key, value);
			}
		}

		/// <summary>Frees the memory of all managed values.</summary>
		virtual ~_Manager()
		{
			clear();
		}
	};



#define SPRITE_ID std::string
#define SPRITE_KEY BUFFER_KEY

	// An individual sprite on a sprite sheet.
	struct Sprite
	{
		// The key of the sprite.
		// This should be equal to the start index of the sprite's vertex attributes in the buffer, divided by the total number of floats corresponding to a single vertex in the buffer array.
		const SPRITE_KEY key;

		// The width of the sprite
		const int width;

		// The height of the sprite
		const int height;

		/// <summary>Constructs sprite information.</summary>
		/// <param name="key">The key of the sprite.</param>
		/// <param name="width">The width of the sprite.</param>
		/// <param name="height">The height of the sprite.</param>
		Sprite(SPRITE_KEY key, int width, int height);
	};

	typedef _Manager<SPRITE_ID, Sprite> _SpriteManager;

#define TEXTURE_ID std::string
#define TEXTURE_MATRIX mat2x4f

	struct Texture
	{
		// The matrix associated with the texture. Maps RGBA values to a corresponding UV position on an image.
		const TEXTURE_MATRIX tex;

		// The width of the texture.
		const int width;

		// The height of the texture.
		const int height;

		/// <summary>Constructs texture information.</summary>
		/// <param name="tex">The matrix associated with the texture.</param>
		/// <param name="width">The width of the texture, in pixels.</param>
		/// <param name="height">The height of the texture, in pixels.</param>
		Texture(const TEXTURE_MATRIX& tex, int width, int height);

		/// <summary>Automatically constructs a texture matrix, that maps RED to the left edge of the texture, GREEN to the right edge of the texture, and BLUE to the bottom edge of the texture.
		/// (It is constructed this way specifically to make it easier to reflect the sprite horizontally.)</summary>
		/// <param name="left">The left edge of the texture, in pixels.</param>
		/// <param name="top">The top edge of the texture, in pixels.</param>
		/// <param name="width">The width of the texture, in pixels.</param>
		/// <param name="height">The height of the texture, in pixels.</param>
		/// <param name="image_width">The width of the image that the texture is on, in pixels.</param>
		/// <param name="image_height">The height of the image that the texture is on, in pixels.</param>
		Texture(int left, int top, int width, int height, float image_width, float image_height);
	};

	typedef _Manager<TEXTURE_ID, Texture> _TextureManager;




#define PALETTE_MATRIX		mat4x4f
#define RGBA_INT_TO_FLOAT	0.00392157f

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

		/// <summary>Retrieves the color that the palette maps RED to.</summary>
		/// <param name="color">To be filled with the color that the palette maps RED to.</param>
		void get_red_maps_to(vec4f& color) const;

		/// <summary>Retrieves the color that the palette maps GREEN to.</summary>
		/// <param name="color">To be filled with the color that the palette maps GREEN to.</param>
		void get_green_maps_to(vec4f& color) const;

		/// <summary>Retrieves the color that the palette maps BLUE to.</summary>
		/// <param name="color">To be filled with the color that the palette maps BLUE to.</param>
		void get_blue_maps_to(vec4f& color) const;

		/// <summary>Sets the color that the palette maps RED to.</summary>
		/// <param name="red_maps_to">The color that the palette maps RED to.</param>
		void set_red_maps_to(const vec4i& red_maps_to);

		/// <summary>Sets the color that the palette maps RED to.</summary>
		/// <param name="red_maps_to">The color that the palette maps RED to.</param>
		void set_red_maps_to(const vec4f& red_maps_to);

		/// <summary>Sets the color that the palette maps GREEN to.</summary>
		/// <param name="green_maps_to">The color that the palette maps GREEN to.</param>
		void set_green_maps_to(const vec4i& green_maps_to);

		/// <summary>Sets the color that the palette maps GREEN to.</summary>
		/// <param name="green_maps_to">The color that the palette maps GREEN to.</param>
		void set_green_maps_to(const vec4f& green_maps_to);

		/// <summary>Sets the color that the palette maps BLUE to.</summary>
		/// <param name="blue_maps_to">The color that the palette maps BLUE to.</param>
		void set_blue_maps_to(const vec4i& blue_maps_to);

		/// <summary>Sets the color that the palette maps BLUE to.</summary>
		/// <param name="blue_maps_to">The color that the palette maps BLUE to.</param>
		void set_blue_maps_to(const vec4f& blue_maps_to);
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



	// An object that loads and stores data about sprites.
	class _SpriteSheet
	{
	protected:
		// True if the sprite sheet has been loaded, false otherwise.
		bool m_IsLoaded = false;

		// The manager for all sprites contained in the sprite sheet.
		_SpriteManager m_SpriteManager;

		/// <summary>Loads a image from an image file.</summary>
		/// <param name="path">The path to the image file, from the res/img/ folder.</param>
		/// <returns>The loaded image.</returns>
		virtual opengl::_Image* load_image(const char* path) = 0;

		/// <summary>Loads vertex attribute data from a line in a meta file.</summary>
		/// <param name="id">The ID associated with the line of data.</param>
		/// <param name="line">The line of data from the meta file.</param>
		/// <param name="image">The loaded image.</param>
		/// <param name="data">Outputs the vertex attributes into this array of numeric data.</param>
		/// <returns>The sprite generated by the line of data.</returns>
		virtual void load_vertex_data(std::string id, _IntegerData& line, opengl::_Image* image, std::vector<float>& data) = 0;

		/// <summary>Creates a buffer from an array of raw data.</summary>
		/// <param name="image">The loaded image.</param>
		/// <param name="data">The array of vertex attribute data.</param>
		virtual void set_buffer(opengl::_Image* image, const std::vector<float>& data) = 0;

	public:
		/// <summary>Virtual deconstructor.</summary>
		virtual ~_SpriteSheet() {}

		/// <summary>Retrieves the sprite associated with the given ID.</summary>
		/// <param name="id">The ID to retrieve the sprite of.</param>
		/// <returns>The sprite associated with the ID, if there is one. NULL otherwise.</returns>
		Sprite* get_sprite(SPRITE_ID id);

		/// <summary>Checks whether the sprite sheet has finished loading.</summary>
		/// <returns>True if it has finished loading, false otherwise.</returns>
		bool is_loaded() const;

		/// <summary>Loads data into buffer from an image and meta file.</summary>
		/// <param name="path">The path to the image file, from the res/img/ folder.</param>
		virtual void load(const char* path);
	};

	// An object that loads, stores data, and displays sprites.
	template <typename... _Args>
	class SpriteSheet : public _SpriteSheet
	{
	protected:
		typedef Shader<const MatrixStack&, const MatrixStack&, _Args...> _SpriteShader;

		// The shader used for the sprite sheet.
		_SpriteShader* m_Shader;

		// The object used to display the sprites.
		opengl::_BufferDisplayer* m_Displayer;

	public:
		/// <summary>Destroys the displayer object (but not the shader, because shaders may be shared between different sprite sheets).</summary>
		virtual ~SpriteSheet()
		{
			delete m_Displayer;
		}

		/// <summary>Displays a sprite on the sprite sheet.</summary>
		/// <param name="key">The key to the sprite.</param>
		/// <param name="args">Values to pass to the shader.</param>
		void display(SPRITE_KEY key, _Args... args) const
		{
			// Activate the shader
			m_Shader->activate(projection(), model(), args...);

			// Display the sprite
			m_Displayer->display(key);
		}

		/// <summary>Displays a sprite on the sprite sheet.</summary>
		/// <param name="key">The data for the sprite.</param>
		/// <param name="args">Values to pass to the shader.</param>
		void display(const Sprite* sprite, _Args... args) const
		{
			// Activate the shader
			m_Shader->activate(projection(), model(), args...);

			// Display the sprite
			m_Displayer->display(sprite->key);
		}
	};

	template <typename... _Args>
	class PixelSpriteSheet : public SpriteSheet<_Args...>
	{
	protected:
		/// <summary>Generates a pixel-perfect image.</summary>
		/// <param name="path">The path to the image file, from the res/img/ folder.</param>
		/// <returns>The loaded image.</returns>
		opengl::_Image* load_image(const char* path)
		{
			return new opengl::_Image(path, true);
		}
	};


	// A sprite sheet that renders pixel perfect sprites and maps red, green, and blue to separate colors.
	class SimplePixelSpriteSheet : public PixelSpriteSheet<const PALETTE_MATRIX&>
	{
	protected:
		// The shader shared by all simple sprite sheets.
		static _SpriteShader* m_SimpleSpriteShader;

		/// <summary>Loads vertex attribute data from a line in a meta file.</summary>
		/// <param name="id">The ID associated with the line of data.</param>
		/// <param name="line">The line of data from the meta file.</param>
		/// <param name="image">The loaded image.</param>
		/// <param name="data">Outputs the vertex attributes into this array of numeric data.</param>
		/// <returns>The sprite generated by the line of data.</returns>
		void load_vertex_data(std::string id, _IntegerData& line, opengl::_Image* image, std::vector<float>& data);

		/// <summary>Creates a buffer from an array of raw data.</summary>
		/// <param name="image">The loaded image.</param>
		/// <param name="data">The array of vertex attribute data.</param>
		void set_buffer(opengl::_Image* image, const std::vector<float>& data);

	public:
		/// <summary>Gets the shader for pixel perfect sprites.</summary>
		/// <returns>A shader that only takes a palette as an argument.</returns>
		static _SpriteShader* get_shader();

		/// <summary>Creates an empty sprite sheet.</summary>
		SimplePixelSpriteSheet();

		/// <summary>Loads a sprite sheet from an image and meta file.</summary>
		/// <param name="path">The path to the image file, from the res/img/ folder.</param>
		SimplePixelSpriteSheet(const char* path);

		using _SpriteSheet::load;

		/// <summary>Loads data into buffer by partitioning an image file into equally sized sprites.</summary>
		/// <param name="path">The path to the image file, from the res/img/ folder.</param>
		/// <param name="width">The width of each sprite.</param>
		/// <param name="height">The height of each sprite.</param>
		virtual void load(const char* path, int width, int height);

		using PixelSpriteSheet<const PALETTE_MATRIX&>::display;

		/// <summary>Displays a sprite from the sprite sheet.</summary>
		/// <param name="sprite">The sprite to display.</param>
		/// <param name="palette">The color palette to display the sprite with.</param>
		void display(const Sprite* sprite, const Palette* palette) const;
	};


	// A sprite sheet that picks shading from one sprite and colors from another sprite.
	class ShadedTexturePixelSpriteSheet : public PixelSpriteSheet<const TEXTURE_MATRIX&, const PALETTE_MATRIX&, const PALETTE_MATRIX&, const PALETTE_MATRIX&>
	{
	protected:
		// The shader shared by all shaded texture sprite sheets.
		static _SpriteShader* m_ShadedTextureSpriteShader;

		// The manager for all textures in the sprite sheet.
		_TextureManager m_TextureManager;

		/// <summary>Loads vertex attribute data from a line in a meta file.</summary>
		/// <param name="id">The ID associated with the line of data.</param>
		/// <param name="line">The line of data from the meta file.</param>
		/// <param name="image">The loaded image.</param>
		/// <param name="data">Outputs the vertex attributes into this array of numeric data.</param>
		/// <returns>The sprite generated by the line of data.</returns>
		void load_vertex_data(std::string id, _IntegerData& line, opengl::_Image* image, std::vector<float>& data);

		/// <summary>Creates a buffer from an array of raw data.</summary>
		/// <param name="image">The loaded image.</param>
		/// <param name="data">The array of vertex attribute data.</param>
		void set_buffer(opengl::_Image* image, const std::vector<float>& data);

	public:
		/// <summary>Creates an empty sprite sheet.</summary>
		ShadedTexturePixelSpriteSheet();

		/// <summary>Loads a sprite sheet from an image and meta file.</summary>
		/// <param name="path">The path to the image file, from the res/img/ folder.</param>
		ShadedTexturePixelSpriteSheet(const char* path);

		/// <summary>Retrieves the texture associated with the given ID.</summary>
		/// <param name="id">The ID to retrieve the texture of.</param>
		/// <returns>The texture associated with the ID, if there is one. NULL otherwise.</returns>
		Texture* get_texture(TEXTURE_ID id);

		using PixelSpriteSheet<const TEXTURE_MATRIX&, const PALETTE_MATRIX&, const PALETTE_MATRIX&, const PALETTE_MATRIX&>::display;

		/// <summary>Displays a shaded and textured sprite from the sprite sheet.</summary>
		/// <param name="sprite">The shading of the sprite to display.</param>
		/// <param name="flip_horizontally">True if the sprite should be flipped horizontally, false if not.</param>
		/// <param name="texture">The texture to color the sprite with.</param>
		/// <param name="palette">The color palette to display the sprite with.</param>
		void display(const Sprite* sprite, bool flip_horizontally, const Texture* texture, const Palette* palette) const;
	};

}