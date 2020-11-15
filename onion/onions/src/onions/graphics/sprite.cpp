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




	_Manager<String, _SpriteSheet> _SpriteSheet::m_SpriteSheetManager{};

	_SpriteSheet* _SpriteSheet::get_sprite_sheet(String path)
	{
		return m_SpriteSheetManager.get(path);
	}

	void _SpriteSheet::set_sprite_sheet(String path, _SpriteSheet* sprite_sheet)
	{
		m_SpriteSheetManager.erase(sprite_sheet);
		m_SpriteSheetManager.set(path, sprite_sheet);
	}


	_SpriteSheet::~_SpriteSheet()
	{
		m_SpriteSheetManager.erase(this);
	}
	
	
	Sprite* _SpriteSheet::get_sprite(SPRITE_ID id)
	{
		return m_SpriteManager.get(id);
	}

	const Sprite* _SpriteSheet::get_sprite(SPRITE_ID id) const
	{
		return m_SpriteManager.get(id);
	}

	bool _SpriteSheet::is_loaded() const
	{
		return m_IsLoaded;
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
			m_SimpleSpriteShader = new SimplePixelSpriteSheet::_SpriteShader(
				"simple_pixel",
				{ "model", "tintMatrix", "tex2D" }
			);
		}

		return m_SimpleSpriteShader;
	}

	opengl::_VertexBufferData* SimplePixelSpriteSheet::__load(LoadFile& file, opengl::_Image* image)
	{
		VertexBufferData<FLOAT_VEC2, FLOAT_VEC2>* data = new VertexBufferData<FLOAT_VEC2, FLOAT_VEC2>();

		while (file.good())
		{
			StringData line;
			String id = file.load_data(line);

			vec2i pos, size;
			if (line.get("pos", pos) && line.get("size", size))
			{
				// Retrieve the base index
				int index = data->buffer_size();

				// Create a sprite data object
				m_SpriteManager.set(id, new Sprite(index, size.get(0), size.get(1)));

				// Construct the corners, in the order bottom-left -> bottom-right -> top-left -> top-right
				vec2f vertex_pos[4], vertex_uv[4];
				for (int k = 0; k < 4; ++k)
				{
					vertex_pos[k](0) = k % 2 == 0
						? 0.f
						: size.get(0);
					vertex_pos[k](1) = k / 2 == 0
						? 0.f
						: size.get(1);

					vertex_uv[k](0) = k % 2 == 0 
						? (Float)pos.get(0) / image->get_width()
						: (Float)(pos.get(0) + size.get(0)) / image->get_width();
					vertex_uv[k](1) = k / 2 == 0
						? (Float)pos.get(1) / image->get_height()
						: (Float)(pos.get(1) + size.get(1)) / image->get_height();
				}

				// Insert the data to the buffer in triangles of bottom-left -> top-right -> one of the remaining vertices
				data->push(6);
				for (int k = 0; k < 6; ++k)
				{
					int corner_index = k % 3 == 0 ? 0 : (k % 3 == 1 ? 3 : (k / 3 == 0 ? 1 : 2));
					data->set<0>(index + k, vertex_pos[corner_index]);
					data->set<1>(index + k, vertex_uv[corner_index]);
				}
			}
		}

		return data;
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
		VertexBufferData<FLOAT_VEC2, FLOAT_VEC2> data;

		// Partition the image into sprites
		int xmax = image->get_width() / width;
		int ymax = image->get_height() / height;

		for (int y = 0; y < ymax; ++y)
		{
			for (int x = 0; x < xmax; ++x)
			{
				int index = data.buffer_size();

				// Generate the tex coords
				float xr = (Float)width / image->get_width();
				float yr = (Float)height / image->get_height();

				float l = x * xr; // left texcoord
				float r = (x + 1) * xr; // right texcoord
				float w = width;

				float t = y * yr; // top texcoord
				float b = (y + 1) * yr; // bottom texcoord
				float h = height;

				// Construct the corners, in the order bottom-left -> bottom-right -> top-left -> top-right
				vec2f vertex_pos[4], vertex_uv[4];
				for (int k = 0; k < 4; ++k)
				{
					vertex_pos[k](0) = k % 2 == 0 ? 0.f : w;
					vertex_pos[k](1) = k / 2 == 0 ? 0.f : h;

					vertex_uv[k](0) = k % 2 == 0 ? l : r;
					vertex_uv[k](1) = k / 2 == 0 ? b : t;
				}

				// Insert the data to the buffer in triangles of bottom-left -> top-right -> one of the remaining vertices
				data.push(6);
				for (int k = 0; k < 6; ++k)
				{
					int corner_index = k % 3 == 0 ? 0 : (k % 3 == 1 ? 3 : (k / 3 == 0 ? 1 : 2));
					data.set<0>(index + k, vertex_pos[corner_index]);
					data.set<1>(index + k, vertex_uv[corner_index]);
				}
			}
		}

		// Generate an image buffer
		m_Displayer->set_buffer(
			new ImageBuffer(&data, m_Shader->get_attribs(), image)
		);

		// Set the flag that says the sprite sheet has been loaded
		m_IsLoaded = true;
	}

	void SimplePixelSpriteSheet::display(const Sprite* sprite, const Palette* palette) const
	{
		display(sprite->key, palette->get_red_palette_matrix(), 0);
	}



	ShadedTexturePixelSpriteSheet::_SpriteShader* ShadedTexturePixelSpriteSheet::m_ShadedTextureSpriteShader{ nullptr };

	ShadedTexturePixelSpriteSheet::ShadedTexturePixelSpriteSheet()
	{
		if (!m_ShadedTextureSpriteShader)
		{
			// Generate the shader for simple pixel sprite sheets, if it hasn't been generated already
			m_ShadedTextureSpriteShader = new ShadedTexturePixelSpriteSheet::_SpriteShader(
				"shaded_texture_pixel",
				{ "model", "mappingMatrix", "redPaletteMatrix", "greenPaletteMatrix", "bluePaletteMatrix", "tex2D" }
			);
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

	opengl::_VertexBufferData* ShadedTexturePixelSpriteSheet::__load(LoadFile& file, opengl::_Image* image)
	{
		VertexBufferData<FLOAT_VEC2, FLOAT_VEC2, FLOAT_VEC2>* data = new VertexBufferData<FLOAT_VEC2, FLOAT_VEC2, FLOAT_VEC2>();

		// Regex that checks if the data is for shading or a texture
		regex tex_checker("^texture\\s+(\\S.+)");
		smatch tex_idmatch;

		while (file.good())
		{
			StringData line;
			String id = file.load_data(line);

			if (regex_match(id, tex_idmatch, tex_checker)) // Load a texture
			{
				vec2i pos, size;
				if (line.get("pos", pos) && line.get("size", size))
				{
					// Get the ID for the texture
					id = tex_idmatch[1].str();

					// Set the information for the texture
					m_TextureManager.set(id, new Texture(pos.get(0), pos.get(1), size.get(0), size.get(1), image->get_width(), image->get_height()));
				}
			}
			else
			{
				vec2i shading, mapping, size;
				if (line.get("shading", shading) && line.get("mapping", mapping) && line.get("size", size))
				{
					int index = data->buffer_size();

					// Set the information for the sprite
					m_SpriteManager.set(id, new Sprite(index, size.get(0), size.get(1)));

					// Construct the corners, in the order bottom-left -> bottom-right -> top-left -> top-right
					vec2f vertex_pos[4], vertex_shading_uv[4], vertex_mapping_uv[4];
					for (int k = 0; k < 4; ++k)
					{
						vertex_pos[k](0) = k % 2 == 0
							? 0.f
							: size.get(0);
						vertex_pos[k](1) = k / 2 == 0
							? 0.f
							: size.get(1);

						vertex_shading_uv[k](0) = k % 2 == 0
							? (Float)shading.get(0) / image->get_width()
							: (Float)(shading.get(0) + size.get(0)) / image->get_width();
						vertex_shading_uv[k](1) = k / 2 == 0
							? (Float)shading.get(1) / image->get_height()
							: (Float)(shading.get(1) + size.get(1)) / image->get_height();

						vertex_mapping_uv[k](0) = k % 2 == 0
							? (Float)mapping.get(0) / image->get_width()
							: (Float)(mapping.get(0) + size.get(0)) / image->get_width();
						vertex_mapping_uv[k](1) = k / 2 == 0
							? (Float)mapping.get(1) / image->get_height()
							: (Float)(mapping.get(1) + size.get(1)) / image->get_height();
					}

					// Insert the data to the buffer in triangles of bottom-left -> top-right -> one of the remaining vertices
					data->push(6);
					for (int k = 0; k < 6; ++k)
					{
						int corner_index = k % 3 == 0 ? 0 : (k % 3 == 1 ? 3 : (k / 3 == 0 ? 1 : 2));
						data->set<0>(index + k, vertex_pos[corner_index]);
						data->set<1>(index + k, vertex_shading_uv[corner_index]);
						data->set<2>(index + k, vertex_mapping_uv[corner_index]);
					}
				}
			}
		}

		return data;
	}

	void ShadedTexturePixelSpriteSheet::display(const Sprite* sprite, bool flip_horizontally, const Texture* texture, const Palette* palette) const
	{
		if (flip_horizontally)
		{
			// Flip where red and green map to on the texture
			const mat4x2f& tex = texture->tex;
			mat4x2f trans_tex(
				-tex.get(0, 0), -tex.get(0, 1), tex.get(0, 2), tex.get(0, 3),
				-tex.get(1, 0), -tex.get(1, 1), tex.get(1, 2), tex.get(1, 3)
			);

			// Flip and display the textured sprite
			Transform::model.push();
			Transform::model.translate(sprite->width);
			Transform::model.scale(-1.f);
			display(sprite, trans_tex, palette->get_red_palette_matrix(), palette->get_green_palette_matrix(), palette->get_blue_palette_matrix(), 0);
			Transform::model.pop();
		}
		else
		{
			display(sprite, texture->tex, palette->get_red_palette_matrix(), palette->get_green_palette_matrix(), palette->get_blue_palette_matrix(), 0);
		}
	}

}