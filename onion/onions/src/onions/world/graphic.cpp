#include "../../../include/onions/world/graphic.h"
#include "../../../include/onions/world/lighting.h"

namespace onion
{
	namespace world
	{
		
		Flat3DPixelSpriteSheet::_SpriteShader* Flat3DPixelSpriteSheet::m_Flat3DPixelShader{ nullptr };

		Flat3DPixelSpriteSheet::Flat3DPixelSpriteSheet(const char* path)
		{
			if (!m_Flat3DPixelShader)
			{
				m_Flat3DPixelShader = new Flat3DPixelSpriteSheet::_SpriteShader(
					"world/dithered_object",
					{ "model", "tileTexture", "noiseTexture" }
				);
			}

			m_Shader = m_Flat3DPixelShader;

			String fpath("world/");
			fpath += path;

			load(fpath.c_str());
		}

		Flat3DPixelSpriteSheet::_SpriteShader* Flat3DPixelSpriteSheet::get_shader()
		{
			return m_Flat3DPixelShader;
		}
		
		opengl::_VertexBufferData* Flat3DPixelSpriteSheet::__load(LoadFile& file, opengl::_Image* image)
		{
			auto data = new VertexBufferData<FLOAT_VEC3, FLOAT_VEC3, FLOAT_VEC2>();

			while (file.good())
			{
				StringData line;
				String id = file.load_data(line);

				vec2i pos, size;
				if (line.get("pos", pos) && line.get("size", size))
				{
					int index = data->buffer_size();

					// Create a sprite data object
					m_SpriteManager.set(id, new Sprite(index, size.get(0), size.get(1)));

					// Construct the corners, in the order bottom-left -> bottom-right -> top-left -> top-right
					vec3f vertex_pos[4];
					vec2f vertex_uv[4];
					for (int k = 0; k < 4; ++k)
					{
						vertex_pos[k](0) = k % 2 == 0
							? 0.f
							: size.get(0);
						vertex_pos[k](1) = 0.f;
						vertex_pos[k](2) = k / 2 == 0
							? 0.f
							: size.get(1);

						vertex_uv[k](0) = k % 2 == 0
							? (Float)pos.get(0) / image->get_width()
							: (Float)(pos.get(0) + size.get(0)) / image->get_width();
						vertex_uv[k](1) = k / 2 == 0
							? (Float)(pos.get(1) + size.get(1)) / image->get_height()
							: (Float)pos.get(1) / image->get_height();
					}

					// Construct the normal vectors for each corner, using the same order as above
					vec3f vertex_normal[4];
					if (line.get("normal", vertex_normal[0]))
					{
						for (int k = 1; k < 4; ++k)
							vertex_normal[k] = vertex_normal[0];
					}
					else
					{
						for (int k = 0; k < 4; ++k)
						{
							String key("normal:");
							key += k / 2 == 0 ? "lower_" : "upper_";
							key += k % 2 == 0 ? "left" : "right";

							if (!line.get(key, vertex_normal[k])) vertex_normal[k] = vec3f(0.f, -1.f, 0.f);
						}
					}

					// Insert the data to the buffer in triangles of bottom-left -> top-right -> one of the remaining vertices
					data->push(6);
					for (int k = 0; k < 6; ++k)
					{
						int corner_index = k % 3 == 0 ? 0 : (k % 3 == 1 ? 3 : (k / 3 == 0 ? 1 : 2));
						data->set<0>(index + k, vertex_pos[corner_index]);
						data->set<1>(index + k, vertex_normal[corner_index]);
						data->set<2>(index + k, vertex_uv[corner_index]);
					}
				}
			}

			return data;
		}

		void Flat3DPixelSpriteSheet::display(const Sprite* sprite) const
		{
			// Activate the noise image
			get_bluenoise_image()->activate(1);

			// Display the sprite
			PixelSpriteSheet<Int, Int>::display(sprite, 0, 1);
		}



		Textured3DPixelSpriteSheet::_SpriteShader* Textured3DPixelSpriteSheet::m_Textured3DPixelShader{ nullptr };

		Textured3DPixelSpriteSheet::Textured3DPixelSpriteSheet(const char* path)
		{
			if (!m_Textured3DPixelShader)
			{
				m_Textured3DPixelShader = new Textured3DPixelSpriteSheet::_SpriteShader(
					"world/textured_object",
					{ "model", "mappingMatrix", "paletteMatrix", "objTexture" }
				);
			}

			m_Shader = m_Textured3DPixelShader;

			String fpath("world/");
			fpath += path;

			load(fpath.c_str());
		}

		Textured3DPixelSpriteSheet::_SpriteShader* Textured3DPixelSpriteSheet::get_shader()
		{
			return m_Textured3DPixelShader;
		}

		Texture* Textured3DPixelSpriteSheet::get_texture(TEXTURE_ID id)
		{
			return m_TextureManager.get(id);
		}

		const Texture* Textured3DPixelSpriteSheet::get_texture(TEXTURE_ID id) const
		{
			return m_TextureManager.get(id);
		}

		opengl::_VertexBufferData* Textured3DPixelSpriteSheet::__load(LoadFile& file, opengl::_Image* image)
		{
			auto data = new VertexBufferData<FLOAT_VEC3, FLOAT_VEC2, FLOAT_VEC2>();

			// Regex that checks if the data is for shading or a texture
			std::regex tex_checker("^texture\\s+(\\S.+)");
			std::smatch tex_idmatch;

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
						vec3f vertex_pos[4];
						vec2f vertex_shading_uv[4], vertex_mapping_uv[4];
						for (int k = 0; k < 4; ++k)
						{
							vertex_pos[k](0) = k % 2 == 0
								? 0.f
								: size.get(0);
							vertex_pos[k](1) = 0.f;
							vertex_pos[k](2) = k / 2 == 0
								? 0.f
								: size.get(1);

							vertex_shading_uv[k](0) = k % 2 == 0
								? (Float)shading.get(0) / image->get_width()
								: (Float)(shading.get(0) + size.get(0)) / image->get_width();
							vertex_shading_uv[k](1) = k / 2 == 0
								? (Float)(shading.get(1) + size.get(1)) / image->get_height()
								: (Float)shading.get(1) / image->get_height();

							vertex_mapping_uv[k](0) = k % 2 == 0
								? (Float)mapping.get(0) / image->get_width()
								: (Float)(mapping.get(0) + size.get(0)) / image->get_width();
							vertex_mapping_uv[k](1) = k / 2 == 0
								? (Float)(mapping.get(1) + size.get(1)) / image->get_height()
								: (Float)mapping.get(1) / image->get_height();
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

		void Textured3DPixelSpriteSheet::display(const Sprite* sprite, bool flip_horizontally, const Texture* texture, const Palette* palette) const
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
				display(sprite, trans_tex, palette->get_red_palette_matrix(), 0);
				Transform::model.pop();
			}
			else
			{
				display(sprite, texture->tex, palette->get_red_palette_matrix(), 0);
			}
		}


		
		FlatWallGraphic3D::FlatWallGraphic3D(const Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite) : SpriteGraphic3D<Flat3DPixelSpriteSheet>(sprite_sheet), m_Sprite(sprite) {}

		void FlatWallGraphic3D::display(const vec3i& normal) const
		{
			m_SpriteSheet->display(m_Sprite);
		}


		TransformedFlatWallGraphic3D::TransformedFlatWallGraphic3D(const Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite, const vec2f& scale) : FlatWallGraphic3D(sprite_sheet, sprite)
		{
			m_Transform.identity();
			for (int k = 1; k >= 0; --k)
			{
				m_Transform.set(k, 0, scale.get(k));
				if (scale.get(k) < 0)
					m_Transform.set(k, 3, -scale.get(k) * m_Sprite->width);
			}
		}
		
		TransformedFlatWallGraphic3D::TransformedFlatWallGraphic3D(const Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite, Float angle) : FlatWallGraphic3D(sprite_sheet, sprite)
		{
			m_Transform.identity();
			m_Transform.rotatez(angle);
		}

		void TransformedFlatWallGraphic3D::display(const vec3i& normal) const
		{
			// Set up the transform
			Transform::model.push();
			Transform::model.custom(m_Transform);

			// Display the sprite
			m_SpriteSheet->display(m_Sprite);

			// Clean up
			Transform::model.pop();
		}



		DynamicShadingSpriteGraphic3D::DynamicShadingSpriteGraphic3D(const Textured3DPixelSpriteSheet* sprite_sheet, const std::vector<const Sprite*>& sprites, bool flip_horizontally, const Texture* texture, Palette* palette) : SpriteGraphic3D(sprite_sheet)
		{
			m_Sprites = sprites;
			m_Texture = texture;
			m_Palette = palette;

			m_FlipHorizontally = flip_horizontally;
			m_SpriteIndex = 0;
		}

		void DynamicShadingSpriteGraphic3D::display(const vec3i& normal) const
		{
			m_SpriteSheet->display(m_Sprites[m_SpriteIndex], false, m_Texture, m_Palette);
		}

	}
}