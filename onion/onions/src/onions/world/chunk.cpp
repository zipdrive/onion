#include <algorithm>
#include <regex>
#include "../../../include/onions/error.h"
#include "../../../include/onions/world/camera.h"
#include "../../../include/onions/world/chunk.h"
#include "../../../include/onions/world/lighting.h"

using namespace std;

namespace onion
{
	namespace world
	{


		std::unordered_map<std::string, Chunk::TileImageManager*> Chunk::m_Images{};
		
		Chunk::TileImageManager::TileImageManager(opengl::_Image* image, Chunk* chunk) : image(image)
		{
			chunks.insert(chunk);
		}


		int Chunk::m_TileSize{ 0 };

		int Chunk::get_tile_size()
		{
			return m_TileSize;
		}

		void Chunk::set_tile_size(int size)
		{
			m_TileSize = size;
		}


		Chunk::TileRow::TileRow(BUFFER_KEY index, Int count) : index(index), count(count) {}


		Chunk::Chunk(const char* path) : m_Path(path) {}


		const opengl::_Image* Chunk::get_tile_image() const
		{
			return m_TileImage;
		}

		void Chunk::set_tile_image(std::string path)
		{
			auto iter = m_Images.find(path);
			if (iter != m_Images.end())
			{
				// Use an already-loaded image
				m_TileImage = iter->second->image;
				iter->second->chunks.insert(this);
			}
			else
			{
				// Load an unused image
				m_TileImage = new opengl::_Image(("world/tiles/" + path).c_str(), true);
				m_Images.emplace(path, new TileImageManager(m_TileImage, this));
			}
		}

		void Chunk::unset_tile_image()
		{
			// Unload the tile sprite image, if this was the last chunk using it
			for (auto iter = m_Images.begin(); iter != m_Images.end(); ++iter)
			{
				if (iter->second->image == m_TileImage)
				{
					iter->second->chunks.erase(this);
					if (iter->second->chunks.empty())
					{
						delete m_TileImage;
						delete iter->second;
						m_Images.erase(iter);
					}

					break;
				}
			}
			m_TileImage = nullptr;
		}


		int Chunk::get_index(int x, int y) const
		{
			return (m_Dimensions.get(0) * y) + x;
		}

		bool Chunk::is_loaded() const
		{
			return m_IsLoaded;
		}

		void Chunk::load()
		{
			// Unset the flag saying that the chunk is loaded
			m_IsLoaded = false;

			// Clear the prior width and height
			m_Dimensions = vec2i(-1, -1);

			// Load the chunk
			m_Displayer = new opengl::_SquareBufferDisplayer();
			m_Displayer->set_buffer(new opengl::_VertexBuffer(__load(), get_tile_shader()->get_attribs()));

			// Set the flag saying the chunk is loaded
			m_IsLoaded = true;
		}

		void Chunk::unload()
		{
			// Unset the flag saying the chunk is loaded
			m_IsLoaded = false;

			// Unset the tile image
			unset_tile_image();

			// Unset the tile buffer
			delete m_Displayer;
		}

		void Chunk::reset_visible(const WorldCamera::View& view)
		{
			m_VisibleTiles.clear();

			// TODO do this more efficiently?
			bool ydir = view.edges[BOTTOM_VIEW_EDGE].normal.get(1) >= 0;
			for (int j = ydir ? 0 : m_Dimensions.get(1) - 1; 
				ydir ? j < m_Dimensions.get(1) : j >= 0; 
				ydir ? ++j : --j)
			{
				int first_i = INT_MIN;
				int last_i = INT_MAX;

				bool xdir = view.edges[BOTTOM_VIEW_EDGE].normal.get(0) >= 0;

				for (int i = xdir ? 0 : m_Dimensions.get(0) - 1;
					xdir ? i < m_Dimensions.get(0) : i >= 0;
					xdir ? ++i : --i)
				{
					for (int k = 3; k >= 0; --k)
					{
						// Calculate the 2D position of the corner
						vec2i corner_pos(
							k % 2 == 0 ? m_TileSize * UNITS_PER_PIXEL * i : (m_TileSize * UNITS_PER_PIXEL * (i + 1)) - 1,
							k / 2 == 0 ? m_TileSize * UNITS_PER_PIXEL * j : (m_TileSize * UNITS_PER_PIXEL * (j + 1)) - 1
						);

						// Calculate the 3D position of the corner
						vec3i corner(corner_pos, get_tile_height(corner_pos.get(0), corner_pos.get(1)));

						// Calculate whether the corner is visible
						if (view.is_visible(corner))
						{
							if (first_i < 0)
								first_i = i;
							last_i = i;
							break;
						}
					}

					// Break if the end of the visible row has been reached
					if (last_i < i)
						break;
				}

				// Add a visible row
				if (first_i >= 0)
				{
					m_VisibleTiles.emplace_back(get_index(first_i, j) * 6, last_i - first_i + 1);
				}
			}
		}

		void Chunk::display_tiles() const
		{
			if (m_IsLoaded) // Make sure everything is loaded
			{
				// Activate the tile shader
				activate_tile_shader();

				// Iterate through all rows of tiles
				for (auto iter = m_VisibleTiles.begin(); iter != m_VisibleTiles.end(); ++iter)
					m_Displayer->display(iter->index, iter->count);
			}
		}



		Shader<FLOAT_MAT4, Int, Int>* FlatChunk::m_BasicFlatTileShader{ nullptr };

		FlatChunk::FlatChunk(const char* path) : Chunk(path)
		{
			if (!m_BasicFlatTileShader)
				m_BasicFlatTileShader = new Shader<FLOAT_MAT4, Int, Int>("world/flat_tile_basic");
		}

		void FlatChunk::__load_tile(const StringData& line, FlatChunk::buffer_t* data)
		{
			int x, y;
			if (line.get("x", x) && line.get("y", y))
			{
				int dx, dy;
				if (!line.get("dx", dx))
					dx = 1;
				if (!line.get("dy", dy))
					dy = 1;

				if (x + dx >= m_Dimensions.get(0) || y + dy >= m_Dimensions.get(1))
				{
					// Resize the dimensions of the vertex array in the buffer

					int ddx = max(x + dx - m_Dimensions.get(0), 0);
					int ddy = max(y + dy - m_Dimensions.get(1), 0);

					if (ddx > 0)
					{
						if (m_Dimensions.get(0) == 0)
						{
							if (m_Dimensions.get(1) > 0)
							{
								data->push(6 * ddx * m_Dimensions.get(1));
							}
						}
						else
						{
							// Resize the number of vertices per row in the buffer
							int index = get_index(m_Dimensions.get(0), 1);
							for (int r = 0; r < m_Dimensions.get(1) - 1; ++r)
							{
								data->insert(index, 6 * ddx);
								index += m_Dimensions.get(0) + ddx;
							}
							data->push(6 * ddx);
						}
					}

					if (ddy > 0)
					{
						data->push(6 * (m_Dimensions.get(0) + ddx) * ddy);
					}

					m_Dimensions += vec2i(ddx, ddy);
				}

				int sprite; // The index of the sprite for the tile
				if (line.get("sprite", sprite))
				{
					int sprite_rot; // The rotation of the tile's sprite from its default orientation
					if (!line.get("sprite_rot", sprite_rot))
						sprite_rot = 0;

					const opengl::_Image* img = get_tile_image();
					int sx = (sprite % (img->get_width() / m_TileSize)) * m_TileSize;
					int sy = (sprite / (img->get_height() / m_TileSize)) * m_TileSize;

					float l = (float)sx / img->get_width();
					float r = (float)(sx + m_TileSize) / img->get_width();
					float t = (float)sy / img->get_height();
					float b = (float)(sy + m_TileSize) / img->get_height();

					vec2f uv[4];
					uv[TILE_CORNER_BOTTOM_LEFT] = vec2f(l, b);
					uv[TILE_CORNER_BOTTOM_RIGHT] = vec2f(r, b);
					uv[TILE_CORNER_TOP_RIGHT] = vec2f(r, t);
					uv[TILE_CORNER_TOP_LEFT] = vec2f(l, t);

					for (int i = x; i < x + dx; ++i)
					{
						for (int j = y; j < y + dy; ++j)
						{
							int index = 6 * get_index(i, j);

							data->set<1>(index + 0, uv[(TILE_CORNER_BOTTOM_LEFT + sprite_rot) % 4]);
							data->set<1>(index + 1, uv[(TILE_CORNER_BOTTOM_RIGHT + sprite_rot) % 4]);
							data->set<1>(index + 2, uv[(TILE_CORNER_TOP_RIGHT + sprite_rot) % 4]);

							data->set<1>(index + 3, uv[(TILE_CORNER_BOTTOM_LEFT + sprite_rot) % 4]);
							data->set<1>(index + 4, uv[(TILE_CORNER_TOP_LEFT + sprite_rot) % 4]);
							data->set<1>(index + 5, uv[(TILE_CORNER_TOP_RIGHT + sprite_rot) % 4]);
						}
					}
				}
			}
		}

		void FlatChunk::__load_obj(std::string id, const StringData& line)
		{
			if (Object* obj = ObjectGenerator::generate(id, line))
			{
				m_Manager.add(obj);
			}
		}

		opengl::_VertexBufferData* FlatChunk::__load()
		{
			buffer_t* data = new buffer_t();

			regex tile_regex("^tile");
			regex obj_regex("^obj\\s+(.*)");

			string fpath = string("res/data/world/") + m_Path;
			LoadFile file(fpath);
			while (file.good())
			{
				StringData line;
				string id = file.load_data(line);

				smatch match;
				if (regex_match(id, match, tile_regex))
				{
					__load_tile(line, data);
				}
				else if (regex_match(id, match, obj_regex))
				{
					id = match[1].str();
					__load_obj(id, line);
				}
				else
				{
					string img;
					if (line.get("sprites", img))
						set_tile_image(img);
				}
			}

			// Set the position of each vertex
			for (int i = m_Dimensions.get(0) - 1; i >= 0; --i)
			{
				for (int j = m_Dimensions.get(1) - 1; j >= 0; --j)
				{
					vec2f base_pos = vec2f(m_TileSize * i, m_TileSize * j);

					vec2f pos[4];
					pos[TILE_CORNER_BOTTOM_LEFT] = base_pos + vec2f(0.f, 0.f);
					pos[TILE_CORNER_BOTTOM_RIGHT] = base_pos + vec2f(m_TileSize, 0.f);
					pos[TILE_CORNER_TOP_RIGHT] = base_pos + vec2f(m_TileSize, m_TileSize);
					pos[TILE_CORNER_TOP_LEFT] = base_pos + vec2f(0.f, m_TileSize);

					int index = 6 * get_index(i, j);

					data->set<0>(index + 0, pos[TILE_CORNER_BOTTOM_LEFT]);
					data->set<0>(index + 1, pos[TILE_CORNER_BOTTOM_RIGHT]);
					data->set<0>(index + 2, pos[TILE_CORNER_TOP_RIGHT]);

					data->set<0>(index + 3, pos[TILE_CORNER_BOTTOM_LEFT]);
					data->set<0>(index + 4, pos[TILE_CORNER_TOP_LEFT]);
					data->set<0>(index + 5, pos[TILE_CORNER_TOP_RIGHT]);
				}
			}

			return data;
		}

		int FlatChunk::get_tile_height(int x, int y) const
		{
			return 0;
		}

		const opengl::_Shader* FlatChunk::get_tile_shader() const
		{
			return m_BasicFlatTileShader;
		}

		void FlatChunk::activate_tile_shader() const
		{
			m_BasicFlatTileShader->activate(Transform::model.get(), 0, 1);
			get_tile_image()->activate(0);
			get_bluenoise_image()->activate(1);
		}

		void FlatChunk::reset_visible(const WorldCamera::View& view)
		{
			// Reset visible tiles
			// TODO do this more efficiently???
			m_VisibleTiles.clear();

			int min_x = INT_MAX, min_y = INT_MAX, max_x = INT_MIN, max_y = INT_MIN;
			for (int k = 3; k >= 0; --k)
			{
				int e1 = k % 2 == 0 ? LEFT_VIEW_EDGE : RIGHT_VIEW_EDGE;
				int e2 = k / 2 == 0 ? BOTTOM_VIEW_EDGE : TOP_VIEW_EDGE;

				int y = ((view.edges[e1].normal.get(0) * view.edges[e2].dot) - (view.edges[e2].normal.get(0) * view.edges[e1].dot))
					/ ((view.edges[e1].normal.get(0) * view.edges[e2].normal.get(1)) - (view.edges[e2].normal.get(0) * view.edges[e1].normal.get(1)));
				int x = (view.edges[e1].dot - (view.edges[e1].normal.get(1) * y)) / view.edges[e1].normal.get(0);

				if (x < min_x)
					min_x = x;
				else if (x > max_x)
					max_x = x;
				
				if (y < min_y)
					min_y = y;
				else if (y > max_y)
					max_y = y;
			}

			int min_j = std::max<int>(0, min_y / (m_TileSize * UNITS_PER_PIXEL));
			int max_j = std::min<int>(m_Dimensions.get(1) - 1, max_y / (m_TileSize * UNITS_PER_PIXEL));
			for (int j = min_j; j <= max_j; ++j)
			{
				int first_i = INT_MIN, last_i = INT_MAX;

				int min_i = std::max<int>(0, min_x / (m_TileSize * UNITS_PER_PIXEL));
				int max_i = std::min<int>(m_Dimensions.get(0) - 1, max_x / (m_TileSize * UNITS_PER_PIXEL));
				for (int i = min_i; i <= max_i; ++i)
				{
					for (int k = 3; k >= 0; --k)
					{
						// Calculate the 3D position of the corner
						vec3i corner(
							k % 2 == 0 ? m_TileSize * UNITS_PER_PIXEL * i : (m_TileSize * UNITS_PER_PIXEL * (i + 1)) - 1,
							k / 2 == 0 ? m_TileSize * UNITS_PER_PIXEL * j : (m_TileSize * UNITS_PER_PIXEL * (j + 1)) - 1,
							0
						);

						// Calculate whether the corner is visible
						if (view.is_visible(corner))
						{
							if (first_i < 0)
								first_i = i;
							last_i = i;
							break;
						}
					}

					// Break if at the end of the visible row
					if (last_i < i)
						break;
				}

				// Add the visible row
				if (first_i >= 0)
					m_VisibleTiles.emplace_back(get_index(first_i, j) * 6, last_i - first_i + 1);
			}

			// Reset visible objects
			m_Manager.reset_visible(view);
		}

		void FlatChunk::update_visible(const WorldCamera::View& view)
		{
			m_Manager.update_visible(view);
		}

		void FlatChunk::display_objects(const Ray& center) const
		{
			m_Manager.display(center);
		}


	}
}