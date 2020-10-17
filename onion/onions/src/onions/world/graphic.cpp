#include "../../../include/onions/world/graphic.h"
#include "../../../include/onions/world/lighting.h"

namespace onion
{
	namespace world
	{

		std::unordered_map<String, const Generic3DSpriteSheet*> Generic3DSpriteSheet::m_SpriteSheets{};
		
		Generic3DSpriteSheet::Generic3DSpriteSheet(const char* path)
		{
			m_Path = path;

			// Register the sprite sheet
			auto iter = m_SpriteSheets.find(m_Path);
			if (iter != m_SpriteSheets.end())
			{
				delete iter->second;
				m_SpriteSheets.emplace_hint(iter, m_Path, this);
			}
			else
			{
				m_SpriteSheets.emplace(m_Path, this);
			}
		}

		Generic3DSpriteSheet::~Generic3DSpriteSheet()
		{
			m_SpriteSheets.erase(m_Path);
		}

		const Generic3DSpriteSheet* Generic3DSpriteSheet::get_sprite_sheet(String path)
		{
			auto iter = m_SpriteSheets.find(path);
			return iter != m_SpriteSheets.end() ? iter->second : nullptr;
		}
		
		
		Flat3DPixelSpriteSheet::_SpriteShader* Flat3DPixelSpriteSheet::m_Flat3DPixelShader{ nullptr };

		Flat3DPixelSpriteSheet::Flat3DPixelSpriteSheet(const char* path) : Generic3DSpriteSheet(path)
		{
			if (!m_Flat3DPixelShader)
			{
				m_Flat3DPixelShader = new Flat3DPixelSpriteSheet::_SpriteShader("world/dithered_object");
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

		const Sprite* Flat3DPixelSpriteSheet::get_sprite(SPRITE_ID id) const
		{
			return _SpriteSheet::get_sprite(id);
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
							? (Float)pos.get(1) / image->get_height()
							: (Float)(pos.get(1) + size.get(1)) / image->get_height();
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


		
		SpriteGraphic3D::SpriteGraphic3D(const Generic3DSpriteSheet* sprite_sheet) : m_SpriteSheet(sprite_sheet) {}
		
		
		BillboardedSpriteGraphic3D::BillboardedSpriteGraphic3D(const Generic3DSpriteSheet* sprite_sheet) : SpriteGraphic3D(sprite_sheet) {}

		void BillboardedSpriteGraphic3D::display(const Ray& center) const
		{
			const Sprite* sprite = get_sprite(center);

			// Push a new transform matrix onto the stack
			Transform::model.push();

			// Pivot the sprite around the bottom-center, so that dir => (0, -1)
			Int x = center.direction.get(0), y = center.direction.get(1);
			Float len = sqrtf((x * x) + (y * y));
			Float c = -y / len;
			Float s = abs(x) / len;

			Int middle = -sprite->width / 2;

			Transform::model.custom(
				TransformMatrix(
					c, -s, 0.f, c * middle,
					s,  c, 0.f, s * middle
				)
			);

			// Display the sprite
			m_SpriteSheet->display(sprite);

			// Clean up the transformation
			Transform::model.pop();
		}


		XAlignedSpriteGraphic3D::XAlignedSpriteGraphic3D(const Generic3DSpriteSheet* sprite_sheet) : SpriteGraphic3D(sprite_sheet) {}
		
		void XAlignedSpriteGraphic3D::display(const Ray& center) const
		{
			const Sprite* sprite = get_sprite(center);

			if (center.direction.get(1) > 0)
			{
				// Push a new transform matrix onto the stack
				Transform::model.push();

				// Flip the sprite 180 degrees
				Transform::model.custom(TransformMatrix(-1.f, 0.f, 0.f, sprite->width));

				// Display the sprite
				m_SpriteSheet->display(sprite);

				// Clean up the transformation
				Transform::model.pop();
			}
			else
			{
				// Just display the sprite using the default vertex attributes
				m_SpriteSheet->display(sprite);
			}
		}


		template <typename T>
		StaticSpriteGraphic3D<T>::StaticSpriteGraphic3D(const Generic3DSpriteSheet* sprite_sheet, const Sprite* sprite) : T(sprite_sheet), m_Sprite(sprite) {}

		template <typename T>
		const Sprite* StaticSpriteGraphic3D<T>::get_sprite(const Ray& center) const
		{
			return m_Sprite;
		}



		Graphic3DObject::Graphic3DObject(Shape* bounds, Graphic3D* graphic) : Object(bounds)
		{
			m_Graphic = graphic;
		}

		Graphic3DObject::~Graphic3DObject()
		{
			if (m_Graphic)
				delete m_Graphic;
		}
		
		void Graphic3DObject::display(const Ray& center) const
		{
			if (m_Graphic)
			{
				const vec3i pos = m_Bounds->get_position();

				// Translate to the position of the object, then display the sprite, then translate back
				Transform::model.push();
				Transform::model.translate(pos.get(0) / UNITS_PER_PIXEL, pos.get(1) / UNITS_PER_PIXEL, pos.get(2) / UNITS_PER_PIXEL);
				m_Graphic->display(center);
				Transform::model.pop();
			}
		}



		XAlignedWall::XAlignedWall(const vec3i& pos, const Generic3DSpriteSheet* sprite_sheet, const Sprite* sprite) : 
			Graphic3DObject(
				new Rectangle(pos, vec3i(UNITS_PER_PIXEL * sprite->width, 0, UNITS_PER_PIXEL * sprite->height)), 
				(sprite_sheet != nullptr && sprite != nullptr) ? new StaticXAlignedSpriteGraphic3D(sprite_sheet, sprite) : nullptr
			) {}

	}
}