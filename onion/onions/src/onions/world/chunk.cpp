#include "../../../include/onions/world/camera.h"
#include "../../../include/onions/world/chunk.h"

using namespace std;

namespace onion
{
	namespace world
	{


		struct TileVertexData
		{
			int height = 0; // The height of the vertex
			vec2f uv; // The image UV of the vertex

			vec3f normal_sum; // The sum of normals of adjacent faces
			int normal_count = 0; // The number of normals being summed
		};

#define BOTTOM_LEFT		0
#define BOTTOM_RIGHT	1
#define TOP_RIGHT		2
#define TOP_LEFT		3


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


		opengl::_Shader* Chunk::m_TileShader{ nullptr };


		Chunk::Chunk(const char* path) : m_Path(path) {}

		int Chunk::get_index(int x, int y) const
		{
			return (m_Width * y) + x;
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
			m_Width = -1;
			m_Height = -1;

			bool smooth = false;

			// Load the tile shader
			if (!m_TileShader)
			{
				m_TileShader = new Shader<const vec3i*>(

					// The raw text of the vertex shader
					"#version 330 core\n"
					""
					"in vec3 vertexPosition;\n"
					"in vec3 vertexNormal;\n"
					"in vec2 vertexUV;\n"
					""
					"uniform mat4 camera;\n"
					"uniform mat4 model;\n"
					""
					"out vec3 fragmentNormal;\n"
					"out vec2 fragmentUV;\n"
					""
					"void main() {\n"
					"	gl_Position = camera * model * vec4(vertexPosition, 1);\n"
					"   fragmentNormal = vertexNormal;\n"
					"	fragmentUV = vertexUV;\n"
					"}",

					// The raw text of the fragment shader
					"#version 330 core\n"
					""
					"in vec3 fragmentNormal;\n"
					"in vec2 fragmentUV;\n"
					""
					"uniform sampler2D tex2D;\n"
					""
					"void main() {\n"
					//"   vec3 norm = normalize(fragmentNormal);\n"
					"   vec4 fragColor = texture(tex2D, UV);\n"
					"	gl_FragColor = fragColor;\n"
					"}",

					// The uniform variables of the shader program
					{ "camera", "model" }

				);
			}

			// Set up the buffer vectors
			vector<TileVertexData> temp; // Used to store the vertex attributes before deciding how to order them in the actual buffer vector
			vector<float> data; // The actual buffer vector

			// Construct the path to the data file
			string fpath("res/data/world/");
			fpath.append(m_Path);

			// Load the file
			LoadFile file(fpath);

			while (file.good())
			{
				// Load a line of data
				_StringData line;
				string id = file.load_data(line);

				// Load the tile sprite image
				string img;
				if (line.get("sprites", img))
				{
					auto iter = m_Images.find(img);
					if (iter != m_Images.end())
					{
						// Use an already-loaded image
						m_TileImage = iter->second->image;
						iter->second->chunks.insert(this);
					}
					else
					{
						// Load an unused image
						m_TileImage = new opengl::_Image(("world/tiles/" + img).c_str(), true);
						m_Images.emplace(img, new TileImageManager(m_TileImage, this));
					}
				}

				// Set the flag if the chunk uses smooth shading
				line.get("smooth", smooth);

				// Set the dimensions of the chunk
				int width, height;
				if (line.get("width", width))
				{
					m_Width = width;

					if (m_Height >= 0) 
						temp.resize(m_Width * m_Height * 4);
				}
				if (line.get("height", height))
				{
					m_Height = height;

					if (m_Width >= 0) 
						temp.resize(m_Width * m_Height * 4);
				}

				int x, y;
				if (line.get("x", x) && line.get("y", y))
				{
					int dx, dy;
					if (!line.get("dx", dx)) dx = 1;
					if (!line.get("dy", dy)) dy = 1;

					// Set the height of each vertex of the tiles
					// TODO

					// Set the sprite of the tiles
					int sprite_key;
					if (line.get("sprite_key", sprite_key))
					{
						float w = m_TileImage->get_width();
						float h = m_TileImage->get_height();

						int wn = m_TileImage->get_width() / m_TileSize; // The number of tile sprites per row

						float l = (sprite_key % wn) * m_TileSize / w;
						float r = ((sprite_key % wn) + 1) * m_TileSize / w;
						float t = (sprite_key / wn) * m_TileSize / h;
						float b = ((sprite_key / wn) + 1) * m_TileSize / h;

						for (int i = x + dx; i >= x; --i)
						{
							for (int j = y + dy; j >= y; --j)
							{
								int index = get_index(i, j) * 4;
								int k;
								if (!line.get("sprite_rot", k)) k = 0;

								temp[index + k].uv = vec2i(l, b);
								temp[index + ((k + 1) % 4)].uv = vec2i(r, b);
								temp[index + ((k + 2) % 4)].uv = vec2i(r, t);
								temp[index + ((k + 3) % 4)].uv = vec2i(l, t);
							}
						}
					}
				}
			}

			// Figure out which vertex should be excluded from each face
			vector<int> excluded(m_Width * m_Height * 2); // The vertex excluded from each triangular face composing a square tile
			// TODO maybe do this better?
			for (int k = (m_Width * m_Height) - 1; k >= 0; --k)
			{
				if (k % 2 == 0) // Alternate between the two possible orientations of the faces
				{
					excluded[(k * 2) + 0] = TOP_RIGHT;
					excluded[(k * 2) + 1] = BOTTOM_LEFT;
				}
				else
				{
					excluded[(k * 2) + 0] = TOP_LEFT;
					excluded[(k * 2) + 1] = BOTTOM_RIGHT;
				}
			}

			// Generate the data for the buffer
			data.resize(m_Width * m_Height * 48);
			for (int i = m_Width - 1; i >= 0; --i)
			{
				for (int j = m_Height - 1; j >= 0; --j)
				{
					int index = get_index(i, j);
					for (int k = 1; k >= 0; --k)
					{
						int face_index = (index * 48) + (k * 24);
						int excluded_vertex = excluded[(2 * index) + k]; // The vertex excluded from this face

						// Set the position and image UV of the vertex
						for (int n = 3; n >= 0; --n)
						{
							if (n != excluded_vertex)
							{
								TileVertexData& vertex_temp = temp[(index * 4) + n];
								int vertex_index = face_index + ((n > excluded_vertex ? n - 1 : n) * 8);

								// Position of vertex
								data[vertex_index + 0] = i * m_TileSize * UNITS_PER_PIXEL;
								data[vertex_index + 1] = j * m_TileSize * UNITS_PER_PIXEL;
								data[vertex_index + 2] = vertex_temp.height;

								// Image UV of vertex
								data[vertex_index + 6] = vertex_temp.uv.get(0);
								data[vertex_index + 7] = vertex_temp.uv.get(1);
							}
						}

						// Calculate the normal of the face
						vec3f pos[3];
						for (int n = 2; n >= 0; --n)
							for (int m = 2; m >= 0; --m)
								pos[n].set(m, 0, data[face_index + (n * 8) + 0]);

						vec3f normal;
						vec3f(pos[1] - pos[0]).cross(vec3f(pos[2] - pos[0]), normal);
						if (normal.get(2) < 0.f) // Make sure that the normal is pointing upwards
							normal = -1 * normal;
						normal.normalize(normal);

						// Add the normal to the vertex data of every relevant vertex
						if (smooth)
						{
							// Add the normal to the vertex sum of every connected vertex
							for (int n = 3; n >= 0; --n)
							{
								if (n != excluded_vertex)
								{
									int vertex_height = temp[(index * 4) + n].height; // The height to check against, to make sure the vertices aren't disjointed

									int base_i = (n == BOTTOM_RIGHT || n == TOP_RIGHT) ? i : i - 1;
									int base_j = (n == TOP_LEFT || n == TOP_RIGHT) ? j : j - 1;

									if (base_i >= 0)
									{
										if (base_j >= 0)
										{
											// Set the normal for the face where this vertex is the top right one
											TileVertexData& vertex_temp = temp[(get_index(base_i, base_j) * 4) + TOP_RIGHT];

											if (vertex_temp.height == vertex_height)
											{
												vertex_temp.normal_sum += normal;
												++vertex_temp.normal_count;
											}
										}
										if (base_j < m_Height - 1)
										{
											// Set the normal for the face where this vertex is the bottom right one
											TileVertexData& vertex_temp = temp[(get_index(base_i, base_j + 1) * 4) + BOTTOM_RIGHT];

											if (vertex_temp.height == vertex_height)
											{
												vertex_temp.normal_sum += normal;
												++vertex_temp.normal_count;
											}
										}
									}
									if (base_i < m_Width - 1)
									{
										if (base_j >= 0)
										{
											// Set the normal for the face where this vertex is the top left one
											TileVertexData& vertex_temp = temp[(get_index(base_i + 1, base_j) * 4) + TOP_LEFT];

											if (vertex_temp.height == vertex_height)
											{
												vertex_temp.normal_sum += normal;
												++vertex_temp.normal_count;
											}
										}
										if (base_j < m_Height - 1)
										{
											// Set the normal for the face where this vertex is the bottom left one
											TileVertexData& vertex_temp = temp[(get_index(base_i + 1, base_j + 1) * 4) + BOTTOM_LEFT];

											if (vertex_temp.height == vertex_height)
											{
												vertex_temp.normal_sum += normal;
												++vertex_temp.normal_count;
											}
										}
									}
								}
							}
						}
						else
						{
							// Set the normals in the data vector
							for (int n = 2; n >= 0; --n)
							{
								int vertex_index = face_index + (n * 8);

								// Normal vector of vertex
								data[vertex_index + 3] = normal.get(0);
								data[vertex_index + 4] = normal.get(1);
								data[vertex_index + 5] = normal.get(2);
							}
						}
					}
				}
			}

			// Second pass through data to set the normal vectors, if smoothing is enabled
			if (smooth)
			{
				for (int i = m_Width - 1; i >= 0; --i)
				{
					for (int j = m_Height - 1; j >= 0; --j)
					{
						int index = get_index(i, j);
						for (int k = 1; k >= 0; --k)
						{
							int excluded_vertex = excluded[(2 * index) + k]; // The vertex excluded from this face

							for (int n = 3; n >= 0; --n)
							{
								if (n != excluded_vertex)
								{
									TileVertexData& vertex_temp = temp[(index * 4) + n];
									int vertex_index = ((index * 6) + (k * 3) + (n > excluded_vertex ? n - 1 : n)) * 8;

									// Normal vector of vertex
									if (vertex_temp.normal_count > 0) // Check just to make sure
									{
										data[vertex_index + 3] = vertex_temp.normal_sum.get(0) / vertex_temp.normal_count;
										data[vertex_index + 4] = vertex_temp.normal_sum.get(1) / vertex_temp.normal_count;
										data[vertex_index + 5] = vertex_temp.normal_sum.get(2) / vertex_temp.normal_count;
									}
									else
									{
										// TODO throw error
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
				new Buffer<3, 3, 2>(

					// The shader for the tiles
					m_TileShader,

					// The vertex attributes
					{ "vertexPosition", "vertexNormal", "vertexUV" },

					// The array of data to put in the buffer
					data
				)
			);

			// Set the flag saying the chunk is loaded
			m_IsLoaded = true;
		}

		void Chunk::unload()
		{
			if (m_IsLoaded)
			{
				// Unset the flag that the chunk is loaded.
				m_IsLoaded = false;

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
		}

		void Chunk::display_tiles() const
		{
			if (m_IsLoaded) // Make sure everything is loaded
			{
				for (int j = 0; j < m_Height; ++j)
				{
					int index = get_index(0, j) * 6;
					m_Displayer->display(index, m_Width);
				}
			}
		}

	}
}