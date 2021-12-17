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


		NormalMapped3DPixelSpriteSheet::_SpriteShader* NormalMapped3DPixelSpriteSheet::m_NormalMapped3DPixelShader{ nullptr };

		NormalMapped3DPixelSpriteSheet::NormalMapped3DPixelSpriteSheet(const char* path)
		{
			if (!m_NormalMapped3DPixelShader)
			{
				m_NormalMapped3DPixelShader = new NormalMapped3DPixelSpriteSheet::_SpriteShader(
					"world/normalmap_object",
					{ "model", "overlayMapping", "paletteMapping", "baseTexture" }
				);
			}

			m_Shader = m_NormalMapped3DPixelShader;

			String fpath("world/");
			fpath += path;

			load(fpath.c_str());
		}

		NormalMapped3DPixelSpriteSheet::_SpriteShader* NormalMapped3DPixelSpriteSheet::get_shader()
		{
			return m_NormalMapped3DPixelShader;
		}

		opengl::_VertexBufferData* NormalMapped3DPixelSpriteSheet::__load(LoadFile& file, opengl::_Image* image)
		{
			auto data = new VertexBufferData<FLOAT_VEC3, FLOAT_VEC2, FLOAT_VEC2>();

			while (file.good())
			{
				StringData line;
				String id = file.load_data(line);

				if (id.substr(0, 6) == "sprite")
				{
					vec2i norm, diff, size;
					if (line.get("normal", norm) && line.get("overlay", diff) && line.get("size", size))
					{
						id = id.substr(7);
						int index = data->buffer_size();

						// Create a sprite data object
						m_SpriteManager.set(id, new Sprite(index, size.get(0), size.get(1)));

						// Construct the corners, in the order bottom-left -> bottom-right -> top-left -> top-right
						vec3f vertex_pos[4];
						vec2f vertex_normalUV[4];
						vec2f vertex_diffuseUV[4];
						for (int k = 0; k < 4; ++k)
						{
							vertex_pos[k](0) = k % 2 == 0
								? 0.f
								: size.get(0);
							vertex_pos[k](1) = 0.f;
							vertex_pos[k](2) = k / 2 == 0
								? 0.f
								: size.get(1);

							vertex_normalUV[k](0) = k % 2 == 0
								? (Float)norm.get(0) / image->get_width()
								: (Float)(norm.get(0) + size.get(0)) / image->get_width();
							vertex_normalUV[k](1) = k / 2 == 0
								? (Float)(norm.get(1) + size.get(1)) / image->get_height()
								: (Float)norm.get(1) / image->get_height();

							vertex_diffuseUV[k](0) = k % 2 == 0
								? (Float)diff.get(0) / image->get_width()
								: (Float)(diff.get(0) + size.get(0)) / image->get_width();
							vertex_diffuseUV[k](1) = k / 2 == 0
								? (Float)(diff.get(1) + size.get(1)) / image->get_height()
								: (Float)diff.get(1) / image->get_height();
						}

						// Insert the data to the buffer in triangles of bottom-left -> top-right -> one of the remaining vertices
						data->push(6);
						for (int k = 0; k < 6; ++k)
						{
							int corner_index = k % 3 == 0 ? 0 : (k % 3 == 1 ? 3 : (k / 3 == 0 ? 1 : 2));
							data->set<0>(index + k, vertex_pos[corner_index]);
							data->set<1>(index + k, vertex_normalUV[corner_index]);
							data->set<2>(index + k, vertex_diffuseUV[corner_index]);
						}
					}
				}
				else if (id.substr(0, 7) == "texture")
				{
					vec2i pos, size;
					if (line.get("pos", pos) && line.get("size", size))
					{
						id = id.substr(8);

						// Create a texture data object
						m_TextureManager.set(id, 
							new Texture(
								pos.get(0), pos.get(1), 
								size.get(0), size.get(1), 
								image->get_width(), image->get_height()
							)
						);
					}
				}
			}

			return data;
		}

		const Texture* NormalMapped3DPixelSpriteSheet::get_texture(TEXTURE_ID id) const
		{
			return m_TextureManager.get(id);
		}

		void NormalMapped3DPixelSpriteSheet::display(const Sprite* sprite, bool flip_horizontally, const Texture* texture, const Palette* palette) const
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







		DynamicShadingSpriteGraphic3D::DynamicShadingSpriteGraphic3D(const NormalMapped3DPixelSpriteSheet* sprite_sheet, const std::vector<const Sprite*>& sprites, bool flip_horizontally, const Texture* texture, Palette* palette) : SpriteGraphic3D(sprite_sheet)
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