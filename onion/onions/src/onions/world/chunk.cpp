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
				m_BasicFlatTileShader = new Shader<FLOAT_MAT4, Int, Int>(
					"world/flat_tile_basic",
					{ "model", "tileTexture", "noiseTexture" }
				);
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

		void FlatChunk::__load_obj(String id, const StringData& line)
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

		void FlatChunk::reset_visible(const WorldCamera::View* view)
		{
			// Reset visible tiles
			// TODO do this more efficiently???
			m_VisibleTiles.clear();

			const Int max_allowed = type_limits<Int>::max();

			UprightRectangle r(vec3i(), vec3i(0, max_allowed, max_allowed));
			Int min_x = r.get_distance(view);

			r.translate(vec3i(max_allowed, 0, 0));
			Int max_x = max_allowed - r.get_distance(view);

			r = UprightRectangle(vec3i(), vec3i(max_allowed, 0, max_allowed));
			Int min_y = r.get_distance(view);

			r.translate(vec3i(0, max_allowed, 0));
			Int max_y = r.get_distance(view);

			Int min_j = std::max<Int>(0, min_y / m_TileSize);
			Int max_j = std::min<Int>(m_Dimensions.get(1) - 1, max_y / m_TileSize);
			for (Int j = min_j; j <= max_j; ++j)
			{
				Int first_i = std::numeric_limits<Int>::min(), last_i = std::numeric_limits<Int>::max();

				Int min_i = std::max<Int>(0, min_x / m_TileSize);
				Int max_i = std::min<Int>(m_Dimensions.get(0) - 1, max_x / m_TileSize);
				for (Int i = min_i; i <= max_i; ++i)
				{
					for (int k = 3; k >= 0; --k)
					{
						// Calculate the 3D position of the corner
						Point corner(vec3i(
							k % 2 == 0 ? m_TileSize * i : (m_TileSize * (i + 1)) - 1,
							k / 2 == 0 ? m_TileSize * j : (m_TileSize * (j + 1)) - 1,
							0
						));

						// Calculate whether the corner is visible
						if (corner.get_distance(view) == 0)
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

		void FlatChunk::update_visible(const WorldCamera::View* view, int frames_passed)
		{
			m_Manager.update_visible(view, frames_passed);
		}

		void FlatChunk::display_objects(const vec3i& normal) const
		{
			m_Manager.display(normal);
		}



		SmoothChunk::SmoothChunk(const char* path) : Chunk(path) {}

		void SmoothChunk::__load_tile(const StringData& line, SmoothChunk::buffer_t* data)
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
								m_TileCornerHeights.insert(
									m_TileCornerHeights.end(), 
									(ddx + 1) * (m_Dimensions.get(1) + 1),
									0
								);
							}
						}
						else
						{
							// Resize the number of vertices per row in the buffer
							int index = get_index(m_Dimensions.get(0), 1);
							for (int r = 0; r < m_Dimensions.get(1) - 1; ++r)
							{
								data->insert(6 * index, 6 * ddx);
								m_TileCornerHeights.insert(m_TileCornerHeights.begin() + (index + r), ddx, 0);
								index += m_Dimensions.get(0) + ddx;
							}
							data->push(6 * ddx);
							m_TileCornerHeights.insert(m_TileCornerHeights.end(), ddx, 0);
						}
					}

					if (ddy > 0)
					{
						data->push(6 * (m_Dimensions.get(0) + ddx) * ddy);

						m_TileCornerHeights.insert(
							m_TileCornerHeights.end(),
							(m_Dimensions.get(0) + ddx + 1) * (m_Dimensions.get(1) > 0 ? ddy : ddy + 1),
							0
						);
					}

					m_Dimensions += vec2i(ddx, ddy);
				}

				Int heights[4] = { -1, -1, -1, -1 };
				if (line.get("bottom_left_height", heights[TILE_CORNER_BOTTOM_LEFT]) 
					|| line.get("bottom_right_height", heights[TILE_CORNER_BOTTOM_RIGHT])
					|| line.get("top_right_height", heights[TILE_CORNER_TOP_RIGHT])
					|| line.get("top_left_height", heights[TILE_CORNER_TOP_LEFT]) )
				{
					for (int k = 3; k >= 0; --k)
					{
						if (heights[k] < 0)
						{
							// If the height wasn't set for that corner, use the previous height of that corner
							heights[k] = get_tile_height(
								m_TileSize * ((k + (k / 2)) % 2 == 0 ? x : x + dx),
								m_TileSize * (k / 2 == 0 ? y : y + dy)
							);
						}
					}

					for (int i = x; i <= x + dx; ++i)
					{
						for (int j = y; j <= y + dy; ++j)
						{
							m_TileCornerHeights[get_index(i, j) + j] = 0; // TODO
						}
					}
				}

				Int sprite; // The index of the sprite for the tile
				if (line.get("sprite", sprite))
				{
					Int sprite_rot; // The rotation of the tile's sprite from its default orientation
					if (!line.get("rotation", sprite_rot))
						sprite_rot = 0;

					const opengl::_Image* img = get_tile_image();
					int sx = (sprite % (img->get_width() / m_TileSize)) * m_TileSize;
					int sy = (sprite / (img->get_height() / m_TileSize)) * m_TileSize;

					Float l = (Float)sx / img->get_width();
					Float r = (Float)(sx + m_TileSize) / img->get_width();
					Float t = (Float)sy / img->get_height();
					Float b = (Float)(sy + m_TileSize) / img->get_height();

					vec2f uv[4];
					uv[TILE_CORNER_BOTTOM_LEFT] = vec2f(l, b);
					uv[TILE_CORNER_BOTTOM_RIGHT] = vec2f(r, b);
					uv[TILE_CORNER_TOP_RIGHT] = vec2f(r, t);
					uv[TILE_CORNER_TOP_LEFT] = vec2f(l, t);

					for (int i = x; i < x + dx; ++i)
					{
						for (int j = y; j < y + dy; ++j)
						{
							// The order in which vertices are inserted
							int ordering[6] = {
								TILE_CORNER_BOTTOM_LEFT,
								TILE_CORNER_BOTTOM_RIGHT,
								i + j % 2 == 0 ? TILE_CORNER_TOP_RIGHT : TILE_CORNER_TOP_LEFT,

								i + j % 2 == 0 ? TILE_CORNER_BOTTOM_LEFT : TILE_CORNER_BOTTOM_RIGHT,
								TILE_CORNER_TOP_LEFT,
								TILE_CORNER_TOP_RIGHT
							};

							// Insert the texture uv information into the buffer for each vertex
							int index = 6 * get_index(i, j);
							for (int k = 5; k >= 0; --k)
								data->set<2>(index + k, uv[(ordering[k] + sprite_rot) % 4]);
						}
					}
				}
			}
		}

		void SmoothChunk::__load_obj(String id, const StringData& line)
		{
			if (Object* obj = ObjectGenerator::generate(id, line))
			{
				m_Manager.add(obj);
			}
		}

		opengl::_VertexBufferData* SmoothChunk::__load()
		{
			buffer_t* data = new buffer_t();
			m_Dimensions = vec2i(0, 0);

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

			// Determine the normal vector of each face on each tile
			std::vector<vec3f> face_normals(2 * m_Dimensions.get(0) * m_Dimensions.get(1));
			for (int i = m_Dimensions.get(0) - 1; i >= 0; --i)
			{
				for (int j = m_Dimensions.get(1) - 1; j >= 0; --j)
				{
					int index = 2 * get_index(i, j);

					int heights[4] = {
						m_TileCornerHeights[get_index(i, j) + j],
						m_TileCornerHeights[get_index(i + 1, j) + j],
						m_TileCornerHeights[get_index(i + 1, j + 1) + j + 1],
						m_TileCornerHeights[get_index(i, j + 1) + j + 1]
					};

					if (i + j % 2 == 0)
					{
						// Faces are:
						//		bottom-left, bottom-right, top-right
						//		bottom-left, top-left, top-right
						vec3i(
							heights[TILE_CORNER_BOTTOM_LEFT] - heights[TILE_CORNER_BOTTOM_RIGHT],
							heights[TILE_CORNER_BOTTOM_RIGHT] - heights[TILE_CORNER_TOP_RIGHT],
							m_TileSize
						).normalize(face_normals[index + 0]);
						vec3i(
							heights[TILE_CORNER_TOP_LEFT] - heights[TILE_CORNER_TOP_RIGHT],
							heights[TILE_CORNER_BOTTOM_LEFT] - heights[TILE_CORNER_TOP_LEFT],
							m_TileSize
						).normalize(face_normals[index + 1]);
					}
					else
					{
						// Faces are:
						//		bottom-left, bottom-right, top-left
						//		top-right, bottom-right, top-left
						vec3i(
							heights[TILE_CORNER_BOTTOM_LEFT] - heights[TILE_CORNER_BOTTOM_RIGHT],
							heights[TILE_CORNER_BOTTOM_LEFT] - heights[TILE_CORNER_TOP_LEFT],
							m_TileSize
						).normalize(face_normals[index + 0]);
						vec3i(
							heights[TILE_CORNER_BOTTOM_RIGHT] - heights[TILE_CORNER_TOP_RIGHT],
							heights[TILE_CORNER_TOP_LEFT] - heights[TILE_CORNER_TOP_RIGHT],
							m_TileSize
						).normalize(face_normals[index + 1]);
					}
				}
			}

			// Determine the normal vector of each vertex
			std::vector<vec3f> vertex_normals((m_Dimensions.get(0) + 1) * (m_Dimensions.get(1) + 1));
			for (int i = m_Dimensions.get(0); i >= 0; --i)
			{
				for (int j = m_Dimensions.get(1); j >= 0; --j)
				{
					vec3f normal_sum;
					int normal_count = 0;

					for (int di = -1; di <= 0; ++di)
					{
						for (int dj = -1; dj <= 0; ++dj)
						{
							if (i + di < m_Dimensions.get(0) && i + di >= 0
								&& j + dj < m_Dimensions.get(1) && j + dj >= 0)
							{
								int index = 2 * get_index(i + di, j + dj);
								vec3f norm;

								if (i + j % 2 == 0)
								{
									// Add both normals
									normal_sum += face_normals[index + 0];
									normal_sum += face_normals[index + 1];
									normal_count += 2;
								}
								else
								{
									normal_sum += face_normals[index + (dj == 0 ? 0 : 1)];
									++normal_count;
								}
							}
						}
					}

					// Shouldn't need to ensure no division by zero because there should always be at least 1 normal being summed
					vertex_normals[get_index(i, j) + j] = normal_sum / normal_count;
				}
			}

			// Set the position and normal vector for each corner of every tile
			for (int i = m_Dimensions.get(0) - 1; i >= 0; --i)
			{
				for (int j = m_Dimensions.get(1) - 1; j >= 0; --j)
				{
					// The order in which vertices are inserted
					int ordering[6] = {
						TILE_CORNER_BOTTOM_LEFT,
						TILE_CORNER_BOTTOM_RIGHT,
						i + j % 2 == 0 ? TILE_CORNER_TOP_RIGHT : TILE_CORNER_TOP_LEFT,

						i + j % 2 == 0 ? TILE_CORNER_BOTTOM_LEFT : TILE_CORNER_BOTTOM_RIGHT,
						TILE_CORNER_TOP_LEFT,
						TILE_CORNER_TOP_RIGHT
					};

					// Calculate the position and retrieve the normal vector of each vertex
					vec3f base_pos = vec3f(m_TileSize * i, m_TileSize * j, 0.f);

					vec3f pos[4], normal[4];
					pos[TILE_CORNER_BOTTOM_LEFT] = base_pos + vec3f(0.f, 0.f, 0.f);
					pos[TILE_CORNER_BOTTOM_RIGHT] = base_pos + vec3f(m_TileSize, 0.f, 0.f);
					pos[TILE_CORNER_TOP_RIGHT] = base_pos + vec3f(m_TileSize, m_TileSize, 0.f);
					pos[TILE_CORNER_TOP_LEFT] = base_pos + vec3f(0.f, m_TileSize, 0.f);

					for (int k = 3; k >= 0; --k)
					{
						vec2i corner(pos[k] / m_TileSize);
						int corner_index = get_index(corner.get(0), corner.get(1)) + corner.get(1);

						pos[k](2) = m_TileCornerHeights[corner_index];
						normal[k] = vertex_normals[corner_index];
					}

					// Insert the vertex information into the buffer
					int index = 6 * get_index(i, j);
					for (int k = 5; k >= 0; --k)
					{
						data->set<0>(index + k, pos[ordering[k]]);
						data->set<1>(index + k, normal[ordering[k]]);
					}
				}
			}

			return data;
		}

		int SmoothChunk::get_tile_height(int x, int y) const
		{
			int i = x / m_TileSize;
			int j = y / m_TileSize;

			if (i < 0)
				return get_tile_height(0, y);
			if (j < 0)
				return get_tile_height(x, 0);

			if (i >= m_Dimensions.get(0))
			{
				if (j >= m_Dimensions.get(1))
					return m_TileCornerHeights.back();
				
				// Interpolate between the bottom and top edge
				int v = y - (j * m_TileSize);

				int bottom_height = m_TileCornerHeights[get_index(m_Dimensions.get(0), j) + j];
				int top_height = m_TileCornerHeights[get_index(m_Dimensions.get(0), j + 1) + j + 1];

				return bottom_height + ((top_height - bottom_height) * v / m_TileSize);
			}
			else if (j >= m_Dimensions.get(1))
			{
				// Interpolate between the left and right edge
				int u = x - (i * m_TileSize);

				int left_height = m_TileCornerHeights[get_index(i, m_Dimensions.get(1)) + m_Dimensions.get(1)];
				int right_height = m_TileCornerHeights[get_index(i + 1, m_Dimensions.get(1)) + m_Dimensions.get(1)];

				return left_height + ((right_height - left_height) * u / m_TileSize);
			}

			int u = x - (i * m_TileSize);
			int v = y - (j * m_TileSize);

			int heights[4] = {
				m_TileCornerHeights[get_index(i, j) + j],
				m_TileCornerHeights[get_index(i + 1, j) + j],
				m_TileCornerHeights[get_index(i + 1, j + 1) + j + 1],
				m_TileCornerHeights[get_index(i, j + 1) + j + 1]
			};

			int dhu, dhv;

			if (i + j % 2 == 0)
			{
				// Interpolate height from either:
				//		bottom-left, bottom-right, top-right
				//		bottom-left, top-left, top-right

				if (u > v)
				{
					// Interpolate height from bottom-left, bottom-right, top-right
					dhu = heights[TILE_CORNER_BOTTOM_RIGHT] - heights[TILE_CORNER_BOTTOM_LEFT];
					dhv = heights[TILE_CORNER_TOP_RIGHT] - heights[TILE_CORNER_BOTTOM_RIGHT];
				}
				else
				{
					// Interpolate height from bottom-left, top-left, top-right
					dhv = heights[TILE_CORNER_TOP_LEFT] - heights[TILE_CORNER_BOTTOM_LEFT];
					dhu = heights[TILE_CORNER_TOP_RIGHT] - heights[TILE_CORNER_TOP_LEFT];
				}
			}
			else
			{
				// Interpolate height from either:
				//		bottom-left, bottom-right, top-left
				//		top-right, bottom-right, top-left

				if (u + v < m_TileSize)
				{
					// Interpolate height from bottom-left, bottom-right, top-left
					dhu = heights[TILE_CORNER_BOTTOM_RIGHT] - heights[TILE_CORNER_BOTTOM_LEFT];
					dhv = heights[TILE_CORNER_TOP_LEFT] - heights[TILE_CORNER_BOTTOM_LEFT];
				}
				else
				{
					// Interpolate height from top-right, bottom-right, top-left
					dhu = heights[TILE_CORNER_TOP_RIGHT] - heights[TILE_CORNER_TOP_LEFT];
					dhv = heights[TILE_CORNER_TOP_RIGHT] - heights[TILE_CORNER_BOTTOM_RIGHT];
				}
			}

			return heights[TILE_CORNER_BOTTOM_LEFT] + (((u * dhu) + (v * dhv)) / m_TileSize);
		}

		const opengl::_Shader* SmoothChunk::get_tile_shader() const
		{
			return Flat3DPixelSpriteSheet::get_shader();
		}

		void SmoothChunk::activate_tile_shader() const
		{
			Flat3DPixelSpriteSheet::get_shader()->activate(Transform::model.get(), 0, 1);
			get_tile_image()->activate(0);
			get_bluenoise_image()->activate(1);
		}

		void SmoothChunk::reset_visible(const WorldCamera::View* view)
		{
			static const Int max_allowed = type_limits<Int>::max();

			// Reset visible tiles
			// TODO take height into account
			m_VisibleTiles.clear();
			
			UprightRectangle r(vec3i(), vec3i(0, max_allowed, 0));
			Int min_x = r.get_distance(view);

			r.translate(vec3i(max_allowed, 0, 0));
			Int max_x = max_allowed - r.get_distance(view);

			r = UprightRectangle(vec3i(), vec3i(max_allowed, 0, 0));
			Int min_y = r.get_distance(view);

			r.translate(vec3i(0, max_allowed, 0));
			Int max_y = max_allowed - r.get_distance(view);

			Int min_j = std::max<Int>(0, min_y / m_TileSize);
			Int max_j = std::min<Int>(m_Dimensions.get(1) - 1, max_y / m_TileSize);
			for (Int j = min_j; j <= max_j; ++j)
			{
				Int first_i = std::numeric_limits<Int>::min(), last_i = std::numeric_limits<Int>::max();

				Int min_i = std::max<Int>(0, min_x / m_TileSize);
				Int max_i = std::min<Int>(m_Dimensions.get(0) - 1, max_x / m_TileSize);
				for (Int i = min_i; i <= max_i; ++i)
				{
					for (int k = 3; k >= 0; --k)
					{
						// Calculate the 3D position of the corner
						Point corner(vec3i(
							k % 2 == 0 ? m_TileSize * i : (m_TileSize * (i + 1)) - 1,
							k / 2 == 0 ? m_TileSize * j : (m_TileSize * (j + 1)) - 1,
							0
						));

						// Calculate whether the corner is visible
						if (corner.get_distance(view) == 0)
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

		void SmoothChunk::update_visible(const WorldCamera::View* view, int frames_passed)
		{
			m_Manager.update_visible(view, frames_passed);
		}

		void SmoothChunk::display_objects(const vec3i& normal) const
		{
			m_Manager.display(normal);
		}


	}
}