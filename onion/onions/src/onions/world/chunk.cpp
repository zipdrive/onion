#include <algorithm>
#include "../../../include/onions/error.h"
#include "../../../include/onions/world/camera.h"
#include "../../../include/onions/world/chunk.h"

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


		unsigned int Chunk::m_TileSize{ 0 };

		unsigned int Chunk::get_tile_size()
		{
			return m_TileSize;
		}

		void Chunk::set_tile_size(unsigned int size)
		{
			m_TileSize = size;
		}


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


		/*void Chunk::set_tile_buffer(const vector<Chunk::TileBufferData>& tile_data, bool flat, const opengl::_Shader* tile_shader)
		{
			vector<float> buffer(tile_data.size() * 48); // The buffer of data

			m_TileHeights.resize(tile_data.size());

			// Generate the data for the buffer
			vector<bool> crease(tile_data.size()); // Each element corresponds to a tile: true if the tile's crease is from bottom left to top right, and false if the tile's crease is from bottom right to top left.
			vector<vec3f> normal_sum(tile_data.size() * 4); // The summation of all (normalized) normals connected to each corner
			vector<int> normal_count(tile_data.size() * 4); // The number of faces that the corner is connected to
			for (int i = m_Width - 1; i >= 0; --i)
			{
				for (int j = m_Height - 1; j >= 0; --j)
				{
					int index = get_index(i, j); // The index of the tile
					const TileBufferData& tile = tile_data[index]; // The data of the tile


					// Determine where the crease on each tile should be by maximizing the minimum angle

					// Calculate the position of each corner
					vec3f corner_pos[4];
					for (int n = 3; n >= 0; --n)
					{
						int x = (n == TILE_CORNER_BOTTOM_LEFT || n == TILE_CORNER_TOP_LEFT) ? i : i + 1;
						int y = (n == TILE_CORNER_BOTTOM_LEFT || n == TILE_CORNER_BOTTOM_RIGHT) ? j : j + 1;

						corner_pos[n] = vec3f(
							x * m_TileSize * UNITS_PER_PIXEL,
							y * m_TileSize * UNITS_PER_PIXEL,
							tile.corners[n].height
						);
					}

					// Calculate the direction of each edge
					vec3f edges[6];
					edges[0] = corner_pos[TILE_CORNER_TOP_LEFT] - corner_pos[TILE_CORNER_BOTTOM_LEFT]; // The left edge
					edges[1] = corner_pos[TILE_CORNER_TOP_RIGHT] - corner_pos[TILE_CORNER_BOTTOM_RIGHT]; // The right edge
					edges[2] = corner_pos[TILE_CORNER_BOTTOM_RIGHT] - corner_pos[TILE_CORNER_BOTTOM_LEFT]; // The bottom edge
					edges[3] = corner_pos[TILE_CORNER_TOP_RIGHT] - corner_pos[TILE_CORNER_TOP_LEFT]; // The top edge
					edges[4] = corner_pos[TILE_CORNER_TOP_RIGHT] - corner_pos[TILE_CORNER_BOTTOM_LEFT]; // The diagonal from the bottom left to the top right
					edges[5] = corner_pos[TILE_CORNER_TOP_LEFT] - corner_pos[TILE_CORNER_BOTTOM_RIGHT]; // The diagonal from the bottom right to the top left
					for (int a = 5; a >= 0; --a)
						edges[a].normalize(edges[a]);

					// Calculate the cosine of each angle in each face
					float cosines[12];
					for (int a = 3; a >= 0; --a)
					{
						vec3f v1 = edges[a % 2];
						vec3f v2 = edges[2 + (a / 2)];
						cosines[a] = v1 * v2;
					}
					for (int a = 7; a >= 0; --a)
					{
						vec3f v1 = edges[a % 4];
						vec3f v2 = edges[4 + (a / 4)];
						cosines[4 + a] = v1 * v2;

						// Special case for bottom-right/top-left diagonal and horizontal edges
						if (a == 4 || a == 5)
							cosines[4 + a] *= -1.f;
					}

					// Figure out which diagonal minimizes the maximum cosine of the angles
					float max_cosine_bl_tr = std::max(
						cosines[1], // bottom-right angle
						cosines[2] // top-left angle
					);
					float max_cosine_br_tl = std::max(
						cosines[0], // bottom-left angle
						cosines[3] // top-right angle
					);
					for (int a = 3; a >= 0; --a)
					{
						max_cosine_bl_tr = std::max(max_cosine_bl_tr, cosines[4 + a]);
						max_cosine_br_tl = std::max(max_cosine_br_tl, cosines[8 + a]);
					}

					crease[index] = max_cosine_bl_tr < max_cosine_br_tl;


					// 
					vector<int> corner_heights(4);
					for (int n = 3; n >= 0; --n)
						corner_heights[n] = tile.corners[n].height;
					m_TileHeights[index] = new TileHeightGetter(crease[index], corner_heights);


					// Set the values in the buffer

					for (int k = 1; k >= 0; --k)
					{
						int face_index = (index * 48) + (k * 24); // The base index for the face in the buffer

						int excluded_vertex; // The vertex excluded from this face
						if (crease[index])
						{
							excluded_vertex = k == 0 ? TILE_CORNER_BOTTOM_RIGHT : TILE_CORNER_TOP_LEFT;
						}
						else
						{
							excluded_vertex = k == 0 ? TILE_CORNER_BOTTOM_LEFT : TILE_CORNER_TOP_RIGHT;
						}

						// Set the position and image UV of the vertex, and store the positions in an array we will use to calculate the normals
						vec3f pos[3];
						for (int n = 3; n >= 0; --n)
						{
							if (n != excluded_vertex)
							{
								const TileBufferData::Corner& corner = tile.corners[n]; // The data for the corner associated with this vertex
								int m = n > excluded_vertex ? n - 1 : n; // The vertex number
								int vertex_index = face_index + (m * 8); // The base index for the vertex in the buffer

								// Store the position in our array of vertex positions
								pos[m] = corner_pos[n];

								// Set the position of the vertex in the buffer
								for (int s = 2; s >= 0; --s)
									buffer[vertex_index + s] = pos[m].get(s);

								// Image UV of vertex
								buffer[vertex_index + 6] = corner.uv.get(0);
								buffer[vertex_index + 7] = corner.uv.get(1);
							}
						}

						// Calculate the normal of the face
						vec3f normal;
						vec3f(pos[1] - pos[0]).cross(vec3f(pos[2] - pos[0]), normal);
						if (normal.get(2) < 0.f) // Make sure that the normal is pointing upwards
							normal = -1 * normal;
						normal.normalize(normal);

						// Add the normal to the vertex data of every relevant vertex
						if (flat)
						{
							// Set the normals in the data vector
							for (int m = 2; m >= 0; --m)
							{
								int vertex_index = face_index + (m * 8);

								// Normal vector of vertex
								buffer[vertex_index + 3] = normal.get(0);
								buffer[vertex_index + 4] = normal.get(1);
								buffer[vertex_index + 5] = normal.get(2);
							}
						}
						else
						{
							// Add the normal to the vertex sum of every connected vertex
							for (int n = 3; n >= 0; --n)
							{
								if (n != excluded_vertex)
								{
									int vertex_height = tile.corners[n].height; // The height to check against, to make sure the vertices aren't disjointed
									int corner_index = (4 * index) + n;

									int base_i = (n == TILE_CORNER_BOTTOM_RIGHT || n == TILE_CORNER_TOP_RIGHT) ? i : i - 1;
									int base_j = (n == TILE_CORNER_TOP_LEFT || n == TILE_CORNER_TOP_RIGHT) ? j : j - 1;

									for (int other_i = std::min(base_i + 1, m_Width - 1); other_i >= std::max(base_i, 0); --other_i)
									{
										for (int other_j = std::min(base_j + 1, m_Height - 1); other_j >= std::max(base_j, 0); --other_j)
										{
											int other_index = get_index(other_i, other_j);
											const TileBufferData& other = tile_data[other_index];

											int other_corner;
											if (other_i == base_i)
												other_corner = other_j == base_j ? TILE_CORNER_TOP_RIGHT : TILE_CORNER_BOTTOM_RIGHT;
											else
												other_corner = other_j == base_j ? TILE_CORNER_TOP_LEFT : TILE_CORNER_BOTTOM_LEFT;

											if (other.corners[other_corner].height == vertex_height)
											{
												int normal_index = (4 * other_index) + other_corner; // The index of the normal associated with the corner
												normal_sum[normal_index] += normal;
												++normal_count[normal_index];
											}
										}
									}
								}
							}
						}
					}
				}
			}

			// Second pass through data to set the normal vectors, if smoothing is enabled
			if (!flat)
			{
				for (int i = m_Width - 1; i >= 0; --i)
				{
					for (int j = m_Height - 1; j >= 0; --j)
					{
						int index = get_index(i, j); // The index of the tile
						const TileBufferData& tile = tile_data[index];

						for (int k = 1; k >= 0; --k)
						{
							int face_index = (index * 48) + (k * 24); // The base index for the face in the buffer

							int excluded_vertex; // The vertex excluded from this face
							if (crease[index])
							{
								excluded_vertex = k == 0 ? TILE_CORNER_BOTTOM_RIGHT : TILE_CORNER_TOP_LEFT;
							}
							else
							{
								excluded_vertex = k == 0 ? TILE_CORNER_BOTTOM_LEFT : TILE_CORNER_TOP_RIGHT;
							}

							for (int n = 3; n >= 0; --n)
							{
								if (n != excluded_vertex)
								{
									const TileBufferData::Corner& corner = tile.corners[n]; // The data for the corner associated with this vertex
									int m = n > excluded_vertex ? n - 1 : n; // The vertex number
									int vertex_index = face_index + (m * 8); // The base index for the vertex in the buffer
									int normal_index = (4 * index) + n; // The index of the normal associated with the corner

									// Normal vector of vertex
									int ncount = normal_count[normal_index];
									if (ncount > 0) // Check just to make sure
									{
										const vec3f& nsum = normal_sum[normal_index];
										buffer[vertex_index + 3] = nsum.get(0) / ncount;
										buffer[vertex_index + 4] = nsum.get(1) / ncount;
										buffer[vertex_index + 5] = nsum.get(2) / ncount;
									}
									else
									{
										errlog("ONION: Error during tile buffer construction: No normals associated with the corner found. Defaulting to (0, 0, 1) for the normal vector.");
										buffer[vertex_index + 3] = 0.f;
										buffer[vertex_index + 4] = 0.f;
										buffer[vertex_index + 5] = 1.f;
									}
								}
							}
						}
					}
				}
			}

			// Generate the buffer
			m_Displayer = new opengl::_SquareBufferDisplayer();
			m_Displayer->set_buffer(
				new VertexBuffer(

					// The array of data to put in the buffer
					buffer,

					// The vertex attributes for the tiles
					tile_shader->get_attribs()

				)
			);
		}

		void Chunk::unset_tile_buffer()
		{
			// Free the buffer displayer
			delete m_Displayer;
		}*/


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
			m_Displayer->set_buffer(new VertexBuffer(__load(), get_tile_shader()->get_attribs()));

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
				// TODO
				for (int j = 0; j < m_Dimensions.get(1); ++j)
				{
					int index = get_index(0, j) * 6;
					m_Displayer->display(index, m_Dimensions.get(0));
				}
			}
		}



		Shader<matrix<float, 4, 4>, int>* FlatChunk::m_BasicFlatTileShader{ nullptr };

		FlatChunk::FlatChunk(const char* path) : Chunk(path)
		{
			if (!m_BasicFlatTileShader)
				m_BasicFlatTileShader = new Shader<matrix<float, 4, 4>, int>("world/flat_tile_basic");
		}

		opengl::_VertexBufferData* FlatChunk::__load()
		{
			using buffer_t = VertexBufferData<matrix<float, 2, 1>, matrix<float, 2, 1>>;
			buffer_t* data = new buffer_t();

			string fpath = string("res/data/world/") + m_Path;
			LoadFile file(fpath);
			while (file.good())
			{
				_StringData line;
				string id = file.load_data(line);

				string img;
				if (line.get("sprites", img))
					set_tile_image(img);

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
			m_BasicFlatTileShader->activate(Transform::model.get(), 0);
		}


	}
}