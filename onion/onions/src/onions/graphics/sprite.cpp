#include <regex>
#include "../../../include/onions/graphics/sprite.h"

using namespace std;
using namespace onion::opengl;

namespace onion
{

	Sprite::Sprite(SPRITE_KEY key, int width, int height) : key(key), width(width), height(height) {}

	Texture::Texture(const TEXTURE_MATRIX& tex, int width, int height) : tex(tex), width(width), height(height) {}

	Texture::Texture(int left, int top, int width, int height, float image_width, float image_height)
		: tex(
			-(0.5f * width) / image_width, (0.5f * width) / image_width, 0.f, (left + (0.5f * width)) / image_width,
			0.f, 0.f, height / image_height, top / image_height
		),
		width(width), height(height) {}




	void Palette::generate_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to, PALETTE_MATRIX& matrix)
	{
		matrix.set(0, 0, RGBA_INT_TO_FLOAT * red_maps_to.get(0));
		matrix.set(1, 0, RGBA_INT_TO_FLOAT * red_maps_to.get(1));
		matrix.set(2, 0, RGBA_INT_TO_FLOAT * red_maps_to.get(2));
		matrix.set(3, 0, RGBA_INT_TO_FLOAT * red_maps_to.get(3));

		matrix.set(0, 1, RGBA_INT_TO_FLOAT * green_maps_to.get(0));
		matrix.set(1, 1, RGBA_INT_TO_FLOAT * green_maps_to.get(1));
		matrix.set(2, 1, RGBA_INT_TO_FLOAT * green_maps_to.get(2));
		matrix.set(3, 1, RGBA_INT_TO_FLOAT * green_maps_to.get(3));

		matrix.set(0, 2, RGBA_INT_TO_FLOAT * blue_maps_to.get(0));
		matrix.set(1, 2, RGBA_INT_TO_FLOAT * blue_maps_to.get(1));
		matrix.set(2, 2, RGBA_INT_TO_FLOAT * blue_maps_to.get(2));
		matrix.set(3, 2, RGBA_INT_TO_FLOAT * blue_maps_to.get(3));

		matrix.set(0, 3, 0.f);
		matrix.set(1, 3, 0.f);
		matrix.set(2, 3, 0.f);
		matrix.set(3, 3, 1.f);
	}

	void Palette::generate_palette_matrix(const vec4f& red_maps_to, const vec4f& green_maps_to, const vec4f& blue_maps_to, PALETTE_MATRIX& matrix)
	{
		matrix.set(0, 0, red_maps_to.get(0));
		matrix.set(1, 0, red_maps_to.get(1));
		matrix.set(2, 0, red_maps_to.get(2));
		matrix.set(3, 0, red_maps_to.get(3));

		matrix.set(0, 1, green_maps_to.get(0));
		matrix.set(1, 1, green_maps_to.get(1));
		matrix.set(2, 1, green_maps_to.get(2));
		matrix.set(3, 1, green_maps_to.get(3));

		matrix.set(0, 2, blue_maps_to.get(0));
		matrix.set(1, 2, blue_maps_to.get(1));
		matrix.set(2, 2, blue_maps_to.get(2));
		matrix.set(3, 2, blue_maps_to.get(3));

		matrix.set(0, 3, 0.f);
		matrix.set(1, 3, 0.f);
		matrix.set(2, 3, 0.f);
		matrix.set(3, 3, 1.f);
	}


	SinglePalette::SinglePalette(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
	{
		generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_Matrix);
	}

	SinglePalette::SinglePalette(const vec4f& red_maps_to, const vec4f& green_maps_to, const vec4f& blue_maps_to)
	{
		generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_Matrix);
	}

	const PALETTE_MATRIX& SinglePalette::get_red_palette_matrix() const
	{
		return m_Matrix;
	}

	const PALETTE_MATRIX& SinglePalette::get_green_palette_matrix() const
	{
		return m_Matrix;
	}

	const PALETTE_MATRIX& SinglePalette::get_blue_palette_matrix() const
	{
		return m_Matrix;
	}

	void SinglePalette::get_red_maps_to(vec4f& color) const
	{
		color = vec4f(
			m_Matrix.get(0, 0),
			m_Matrix.get(1, 0),
			m_Matrix.get(2, 0),
			m_Matrix.get(3, 0)
		);
	}

	void SinglePalette::get_green_maps_to(vec4f& color) const
	{
		color = vec4f(
			m_Matrix.get(0, 1),
			m_Matrix.get(1, 1),
			m_Matrix.get(2, 1),
			m_Matrix.get(3, 1)
		);
	}

	void SinglePalette::get_blue_maps_to(vec4f& color) const
	{
		color = vec4f(
			m_Matrix.get(0, 2),
			m_Matrix.get(1, 2),
			m_Matrix.get(2, 2),
			m_Matrix.get(3, 2)
		);
	}

	void SinglePalette::set_red_maps_to(const vec4i& red_maps_to)
	{
		set_red_maps_to(vec4f(RGBA_INT_TO_FLOAT * vec4f(red_maps_to)));
	}

	void SinglePalette::set_red_maps_to(const vec4f& red_maps_to)
	{
		m_Matrix.set(0, 0, red_maps_to.get(0));
		m_Matrix.set(1, 0, red_maps_to.get(1));
		m_Matrix.set(2, 0, red_maps_to.get(2));
		m_Matrix.set(3, 0, red_maps_to.get(3));
	}

	void SinglePalette::set_green_maps_to(const vec4i& green_maps_to)
	{
		set_green_maps_to(vec4f(RGBA_INT_TO_FLOAT * vec4f(green_maps_to)));
	}

	void SinglePalette::set_green_maps_to(const vec4f& green_maps_to)
	{
		m_Matrix.set(0, 1, green_maps_to.get(0));
		m_Matrix.set(1, 1, green_maps_to.get(1));
		m_Matrix.set(2, 1, green_maps_to.get(2));
		m_Matrix.set(3, 1, green_maps_to.get(3));
	}

	void SinglePalette::set_blue_maps_to(const vec4i& blue_maps_to)
	{
		set_blue_maps_to(vec4f(RGBA_INT_TO_FLOAT * vec4f(blue_maps_to)));
	}

	void SinglePalette::set_blue_maps_to(const vec4f& blue_maps_to)
	{
		m_Matrix.set(0, 2, blue_maps_to.get(0));
		m_Matrix.set(1, 2, blue_maps_to.get(1));
		m_Matrix.set(2, 2, blue_maps_to.get(2));
		m_Matrix.set(3, 2, blue_maps_to.get(3));
	}


	MultiplePalette::MultiplePalette()
	{
		generate_palette_matrix(vec4i(255, 0, 0, 0), vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), m_RedMatrix);
		generate_palette_matrix(vec4i(0, 255, 0, 0), vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), m_GreenMatrix);
		generate_palette_matrix(vec4i(0, 0, 255, 0), vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), m_BlueMatrix);
	}

	MultiplePalette::MultiplePalette(const vec4i& red_maps_red_maps_to, const vec4i& red_maps_green_maps_to, const vec4i& red_maps_blue_maps_to,
		const vec4i& green_maps_red_maps_to, const vec4i& green_maps_green_maps_to, const vec4i& green_maps_blue_maps_to,
		const vec4i& blue_maps_red_maps_to, const vec4i& blue_maps_green_maps_to, const vec4i& blue_maps_blue_maps_to)
	{
		generate_palette_matrix(red_maps_red_maps_to, red_maps_green_maps_to, red_maps_blue_maps_to, m_RedMatrix);
		generate_palette_matrix(green_maps_red_maps_to, green_maps_green_maps_to, green_maps_blue_maps_to, m_GreenMatrix);
		generate_palette_matrix(blue_maps_red_maps_to, blue_maps_green_maps_to, blue_maps_blue_maps_to, m_BlueMatrix);
	}

	MultiplePalette::MultiplePalette(const vec4f& red_maps_red_maps_to, const vec4f& red_maps_green_maps_to, const vec4f& red_maps_blue_maps_to,
		const vec4f& green_maps_red_maps_to, const vec4f& green_maps_green_maps_to, const vec4f& green_maps_blue_maps_to,
		const vec4f& blue_maps_red_maps_to, const vec4f& blue_maps_green_maps_to, const vec4f& blue_maps_blue_maps_to)
	{
		generate_palette_matrix(red_maps_red_maps_to, red_maps_green_maps_to, red_maps_blue_maps_to, m_RedMatrix);
		generate_palette_matrix(green_maps_red_maps_to, green_maps_green_maps_to, green_maps_blue_maps_to, m_GreenMatrix);
		generate_palette_matrix(blue_maps_red_maps_to, blue_maps_green_maps_to, blue_maps_blue_maps_to, m_BlueMatrix);
	}

	const PALETTE_MATRIX& MultiplePalette::get_red_palette_matrix() const
	{
		return m_RedMatrix;
	}

	void MultiplePalette::set_red_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
	{
		generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_RedMatrix);
	}

	const PALETTE_MATRIX& MultiplePalette::get_green_palette_matrix() const
	{
		return m_GreenMatrix;
	}

	void MultiplePalette::set_green_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
	{
		generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_GreenMatrix);
	}

	const PALETTE_MATRIX& MultiplePalette::get_blue_palette_matrix() const
	{
		return m_BlueMatrix;
	}

	void MultiplePalette::set_blue_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
	{
		generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_BlueMatrix);
	}




	Sprite* _SpriteSheet::get_sprite(SPRITE_ID id)
	{
		return m_SpriteManager.get(id);
	}

	bool _SpriteSheet::is_loaded() const
	{
		return m_IsLoaded;
	}

	void _SpriteSheet::load(const char* path)
	{
		// Unset the flag that says the sprite sheet has been loaded
		m_IsLoaded = false;

		// Clear existing sprites
		m_SpriteManager.clear();

		// Load the image
		_Image* image = load_image(path);

		// Construct the path to the meta file.
		string fpath("res/img/");
		fpath.append(path);
		regex fext_finder("(.*)\\.[^\\.]+"); // Regex to find the path excluding file extension
		fpath = regex_replace(fpath, fext_finder, "$1.meta");

		// Load the file
		LoadFile file(fpath);
		vector<float> data;

		while (file.good())
		{
			// Load a line of data from the meta file
			_IntegerData line;
			SPRITE_ID id = file.load_data(line);

			// Load the vertex attributes and sprite data from the line data
			load_vertex_data(id, line, image, data);
		}

		// Generate an image buffer
		set_buffer(image, data);

		// Set the flag that says the sprite sheet has been loaded
		m_IsLoaded = true;
	}




	/// <summary>Retrieves the int value associated with a key from a line of data.</summary>
	/// <param name="data">The processed line of data.</param>
	/// <param name="key">The key to retrieve the value of.</param>
	/// <param name="value">Outputs the value associated with the given key.</param>
	/// <returns>True if the key exists, false otherwise.</returns>
	bool load_int(const unordered_map<string, int>& data, string key, int& value)
	{
		auto iter = data.find(key);
		if (iter == data.end())
			return false;

		value = iter->second;
		return true;
	}



	SimplePixelSpriteSheet::_SpriteShader* SimplePixelSpriteSheet::m_SimpleSpriteShader{ nullptr };

	SimplePixelSpriteSheet::SimplePixelSpriteSheet()
	{
		m_Shader = get_shader();
		m_Displayer = new _SquareBufferDisplayer();
	}

	SimplePixelSpriteSheet::SimplePixelSpriteSheet(const char* path) : SimplePixelSpriteSheet()
	{
		load(path);
	}

	SimplePixelSpriteSheet::_SpriteShader* SimplePixelSpriteSheet::get_shader()
	{
		if (!m_SimpleSpriteShader)
		{
			// Generate the shader for simple pixel sprite sheets, if it hasn't been generated already
			m_SimpleSpriteShader = new SimplePixelSpriteSheet::_SpriteShader("simple_pixel");
		}

		return m_SimpleSpriteShader;
	}

	void SimplePixelSpriteSheet::load_vertex_data(string id, _IntegerData& line, opengl::_Image* image, vector<float>& data)
	{
		int left, top, width, height;
		if (line.get("left", left) && line.get("top", top) && line.get("width", width) && line.get("height", height))
		{
			// Create a sprite data object
			m_SpriteManager.set(id, new Sprite(data.size() / 4, width, height));

			// Calculate texcoord numbers
			float l = (float)left / image->get_width(); // left texcoord
			float r = (float)(left + width) / image->get_width(); // right texcoord
			float w = (float)width;

			float t = (float)top / image->get_height(); // top texcoord
			float b = (float)(top + height) / image->get_height(); // bottom texcoord
			float h = (float)height;

			// First triangle: bottom-left, bottom-right, top-right
			// Bottom-left corner, vertices
			data.push_back(0.0f);
			data.push_back(0.0f);
			// Bottom-left corner, tex coord
			data.push_back(l);
			data.push_back(b);
			// Bottom-right corner, vertices
			data.push_back(w);
			data.push_back(0.0f);
			// Bottom-right corner, tex coord
			data.push_back(r);
			data.push_back(b);
			// Top-right corner, vertices
			data.push_back(w);
			data.push_back(h);
			// Top-right corner, tex coord
			data.push_back(r);
			data.push_back(t);

			// Second triangle: bottom-left, top-left, top-right
			// Bottom-left corner, vertices
			data.push_back(0.0f);
			data.push_back(0.0f);
			// Bottom-left corner, tex coord
			data.push_back(l);
			data.push_back(b);
			// Top-left corner, vertices
			data.push_back(0.0f);
			data.push_back(h);
			// Top-left corner, tex coord
			data.push_back(l);
			data.push_back(t);
			// Top-right corner, vertices
			data.push_back(w);
			data.push_back(h);
			// Top-right corner, tex coord
			data.push_back(r);
			data.push_back(t);
		}
	}

	void SimplePixelSpriteSheet::load(const char* path, int width, int height)
	{
		// Unset the flag that says the sprite sheet has been loaded
		m_IsLoaded = false;

		// Clear existing sprites
		m_SpriteManager.clear();

		// Load the image
		_Image* image = load_image(path);

		// Create the data vector
		vector<float> data;

		// Partition the image into sprites
		int xmax = image->get_width() / width;
		int ymax = image->get_height() / height;

		for (int y = 0; y < ymax; ++y)
		{
			for (int x = 0; x < xmax; ++x)
			{
				// Generate the tex coords
				float xr = (float)width / image->get_width();
				float yr = (float)height / image->get_height();

				float l = x * xr; // left texcoord
				float r = (x + 1) * xr; // right texcoord
				float w = width;

				float t = y * yr; // top texcoord
				float b = (y + 1) * yr; // bottom texcoord
				float h = height;

				// First triangle: bottom-left, bottom-right, top-right
				// Bottom-left corner, vertices
				data.push_back(0.0f);
				data.push_back(0.0f);
				// Bottom-left corner, tex coord
				data.push_back(l);
				data.push_back(b);
				// Bottom-right corner, vertices
				data.push_back(w);
				data.push_back(0.0f);
				// Bottom-right corner, tex coord
				data.push_back(r);
				data.push_back(b);
				// Top-right corner, vertices
				data.push_back(w);
				data.push_back(h);
				// Top-right corner, tex coord
				data.push_back(r);
				data.push_back(t);

				// Second triangle: bottom-left, top-left, top-right
				// Bottom-left corner, vertices
				data.push_back(0.0f);
				data.push_back(0.0f);
				// Bottom-left corner, tex coord
				data.push_back(l);
				data.push_back(b);
				// Top-left corner, vertices
				data.push_back(0.0f);
				data.push_back(h);
				// Top-left corner, tex coord
				data.push_back(l);
				data.push_back(t);
				// Top-right corner, vertices
				data.push_back(w);
				data.push_back(h);
				// Top-right corner, tex coord
				data.push_back(r);
				data.push_back(t);
			}
		}

		// Generate an image buffer
		set_buffer(image, data);

		// Set the flag that says the sprite sheet has been loaded
		m_IsLoaded = true;
	}

	void SimplePixelSpriteSheet::set_buffer(_Image* image, const vector<float>& data)
	{
		m_Displayer->set_buffer(
			// Set an image buffer that sets the values of two vertex attributes, each with length 2
			new ImageBuffer(

				// The array of data
				data,

				// All vertex attributes recognized by the shader program
				m_Shader->get_attribs(),

				// The previously loaded image
				image
			)
		);
	}

	void SimplePixelSpriteSheet::display(const Sprite* sprite, const Palette* palette) const
	{
		display(sprite->key, palette->get_red_palette_matrix());
	}



	ShadedTexturePixelSpriteSheet::_SpriteShader* ShadedTexturePixelSpriteSheet::m_ShadedTextureSpriteShader{ nullptr };

	ShadedTexturePixelSpriteSheet::ShadedTexturePixelSpriteSheet()
	{
		if (!m_ShadedTextureSpriteShader)
		{
			// Generate the shader for simple pixel sprite sheets, if it hasn't been generated already
			m_ShadedTextureSpriteShader = new ShadedTexturePixelSpriteSheet::_SpriteShader("shaded_texture_pixel");
		}

		m_Shader = m_ShadedTextureSpriteShader;
		m_Displayer = new _SquareBufferDisplayer();
	}

	ShadedTexturePixelSpriteSheet::ShadedTexturePixelSpriteSheet(const char* path) : ShadedTexturePixelSpriteSheet()
	{
		load(path);
	}

	Texture* ShadedTexturePixelSpriteSheet::get_texture(TEXTURE_ID id)
	{
		return m_TextureManager.get(id);
	}

	void ShadedTexturePixelSpriteSheet::load_vertex_data(std::string id, _IntegerData& line, opengl::_Image* image, std::vector<float>& data)
	{
		// Regex that checks if the data is for shading or a texture
		regex tex_checker("texture\\s+(\\S.+)");
		smatch tex_idmatch;

		if (regex_match(id, tex_idmatch, tex_checker)) // Load a texture
		{
			int left, top, width, height;
			if (line.get("left", left) && line.get("top", top) && line.get("width", width) && line.get("height", height))
			{
				// Get the ID for the texture
				id = tex_idmatch[1].str();

				// Set the information for the texture
				m_TextureManager.set(id, new Texture(left, top, width, height, image->get_width(), image->get_height()));
			}
		}
		else // Load a shading sprite
		{
			int shading_left, shading_top, mapping_left, mapping_top, width, height;
			if (line.get("shading_left", shading_left) && line.get("shading_top", shading_top)
				&& line.get("mapping_left", mapping_left) && line.get("mapping_top", mapping_top)
				&& line.get("width", width) && line.get("height", height))
			{
				// Set the information for the sprite
				m_SpriteManager.set(id, new Sprite(data.size() / 6, width, height));

				// Calculate texcoords
				float w = (float)width;
				float h = (float)height;

				float sl = (float)shading_left / image->get_width(); // left texcoord for shading
				float sr = (float)(shading_left + width) / image->get_width(); // right texcoord for shading
				float st = (float)shading_top / image->get_height(); // top texcoord for shading
				float sb = (float)(shading_top + height) / image->get_height(); // bottom texcoord for shading

				float ml = (float)mapping_left / image->get_width(); // left texcoord for shading
				float mr = (float)(mapping_left + width) / image->get_width(); // right texcoord for shading
				float mt = (float)mapping_top / image->get_height(); // top texcoord for shading
				float mb = (float)(mapping_top + height) / image->get_height(); // bottom texcoord for shading

				// First triangle: bottom-left, bottom-right, top-right
				// Bottom-left corner, vertices
				data.push_back(0.0f);
				data.push_back(0.0f);
				// Bottom-left corner, shading tex coord
				data.push_back(sl);
				data.push_back(sb);
				// Bottom-left corner, mapping tex coord
				data.push_back(ml);
				data.push_back(mb);
				// Bottom-right corner, vertices
				data.push_back(w);
				data.push_back(0.0f);
				// Bottom-right corner, shading tex coord
				data.push_back(sr);
				data.push_back(sb);
				// Bottom-right corner, mapping tex coord
				data.push_back(mr);
				data.push_back(mb);
				// Top-right corner, vertices
				data.push_back(w);
				data.push_back(h);
				// Top-right corner, shading tex coord
				data.push_back(sr);
				data.push_back(st);
				// Top-right corner, mapping tex coord
				data.push_back(mr);
				data.push_back(mt);

				// Second triangle: bottom-left, top-left, top-right
				// Bottom-left corner, vertices
				data.push_back(0.0f);
				data.push_back(0.0f);
				// Bottom-left corner, shading tex coord
				data.push_back(sl);
				data.push_back(sb);
				// Bottom-left corner, mapping tex coord
				data.push_back(ml);
				data.push_back(mb);
				// Top-left corner, vertices
				data.push_back(0.0f);
				data.push_back(h);
				// Top-left corner, shading tex coord
				data.push_back(sl);
				data.push_back(st);
				// Top-left corner, shading tex coord
				data.push_back(ml);
				data.push_back(mt);
				// Top-right corner, vertices
				data.push_back(w);
				data.push_back(h);
				// Top-right corner, shading tex coord
				data.push_back(sr);
				data.push_back(st);
				// Top-right corner, mapping tex coord
				data.push_back(mr);
				data.push_back(mt);
			}
		}
	}

	void ShadedTexturePixelSpriteSheet::set_buffer(_Image* image, const vector<float>& data)
	{
		m_Displayer->set_buffer(
			// Set an image buffer that sets the values of three vertex attributes, each with length 2
			new ImageBuffer(

				// The array of data
				data,

				// All vertex attributes recognized by the program
				m_Shader->get_attribs(),

				// The previously loaded image
				image
			)
		);
	}

	void ShadedTexturePixelSpriteSheet::display(const Sprite* sprite, bool flip_horizontally, const Texture* texture, const Palette* palette) const
	{
		if (flip_horizontally)
		{
			// Flip where red and green map to on the texture
			const mat2x4f& tex = texture->tex;
			mat2x4f trans_tex(
				-tex.get(0, 0), -tex.get(0, 1), tex.get(0, 2), tex.get(0, 3),
				-tex.get(1, 0), -tex.get(1, 1), tex.get(1, 2), tex.get(1, 3)
			);

			// Flip and display the textured sprite
			Transform::model.push();
			Transform::model.translate(sprite->width);
			Transform::model.scale(-1.f);
			display(sprite, texture->tex, palette->get_red_palette_matrix(), palette->get_green_palette_matrix(), palette->get_blue_palette_matrix());
			Transform::model.pop();
		}
		else
		{
			display(sprite, texture->tex, palette->get_red_palette_matrix(), palette->get_green_palette_matrix(), palette->get_blue_palette_matrix());
		}
	}

}