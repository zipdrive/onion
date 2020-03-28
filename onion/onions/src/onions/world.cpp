#include <fstream>
#include <string>
#include "../../include/onions/world.h"


#define MAXIMUM_NUMBER_OF_CHUNKS 100
#define CHUNK_SIZE 64
#define CHUNK_INDEX(x, y) ((x) + (width * (y)))



std::unordered_map<CHUNK_KEY, Chunk*> Chunk::m_Chunks{};
SpriteSheet* Chunk::m_TileSprites{ nullptr };

Chunk* Chunk::get_chunk(CHUNK_KEY key)
{
	auto iter = m_Chunks.find(key);
	if (iter != m_Chunks.end())
		return iter->second;
	else
		return nullptr;
}

Chunk* Chunk::set_chunk(CHUNK_KEY key, const char* path)
{
	auto iter = m_Chunks.find(key);
	if (iter == m_Chunks.end())
	{
		Chunk* chunk = new Chunk(path);
		m_Chunks.emplace(key, chunk);
		return chunk;
	}

	return nullptr; // TODO something else???
}

SpriteSheet* Chunk::get_tile_sprite_sheet()
{
	return m_TileSprites;
}

Chunk::Chunk(const char* path) : m_ChunkFilepath(path)
{
	// Load tile sprites
	if (!m_TileSprites)
	{
		m_TileSprites = SpriteSheet::generate_empty();
		m_TileSprites->load_partitioned_sprite_sheet("tiles.png", TILE_WIDTH, TILE_HEIGHT);
	}

	m_Tiles = nullptr;
	m_Objects = nullptr;
}

void Chunk::load()
{
	if (!m_Objects) // Make sure chunk hasn't already been loaded.
	{
		int s = width * height; // Size of arrays

								// Initialize tiles and objects and set default values
		m_Tiles = new SPRITE_KEY[s];
		memset(m_Tiles, 0, s * sizeof(SPRITE_KEY));

		m_Objects = new Object*[s];
		memset(m_Objects, NULL, s * sizeof(Object*));

		// Initialize tiles
		for (int i = width - 1; i >= 0; --i)
		{
			for (int j = height - 1; j >= 0; --j)
			{
				m_Tiles[CHUNK_INDEX(i, j)] = 6 * ((i + j) % 4);
			}
		}
	}
}

void Chunk::unload()
{
	if (m_Objects) // Make sure chunk has been loaded previously.
	{
		// Unload tiles
		delete[] m_Tiles;

		// Unload objects
		for (int k = (width * height) - 1; k >= 0; --k)
		{
			if (m_Objects[k])
			{
				delete m_Objects[k];
			}
		}
		delete[] m_Objects;
	}
}

void Chunk::display(int xmin, int ymin, int xmax, int ymax)
{
	const mat4x4f identityMatrix;

	if (m_Objects) // Check to make sure the chunk has actually been loaded first
	{
		// Set up transformation for tiles.
		int imin = xmin / TILE_WIDTH;
		int imax = xmax / TILE_WIDTH;
		int jmin = ymin / TILE_HEIGHT;
		int jmax = ymax / TILE_HEIGHT;

		mat_push();
		mat_translate((imin - 1) * TILE_WIDTH, (jmin - 1) * TILE_HEIGHT, 0.f);

		// Draw tiles
		for (int i = imin; i <= imax; ++i)
		{
			mat_translate(TILE_WIDTH, 0.f, 0.f);
			mat_push();

			for (int j = jmin; j <= jmax; ++j)
			{
				mat_translate(0.f, TILE_HEIGHT, 0.f);
				m_TileSprites->display(m_Tiles[CHUNK_INDEX(i, j)], identityMatrix);
			}
			mat_pop();
		}
		mat_pop();
	}
}

int Chunk::trigger(const MouseMoveEvent& event_data)
{
	return EVENT_CONTINUE;
}

int Chunk::trigger(const MousePressEvent& event_data)
{
	return EVENT_CONTINUE;
}

void Chunk::freeze()
{
	MouseMoveListener::freeze();
	MousePressListener::freeze();
}

void Chunk::unfreeze()
{
	MouseMoveListener::unfreeze();
	MousePressListener::unfreeze();
}



World* World::m_World{ nullptr };

World* World::get_world()
{
	if (m_World)
		return m_World;
	return m_World = new World();
}

Chunk* World::get_chunk()
{
	return get_world()->m_Chunk;
}

World::World()
{
	// Load information about all chunks
	for (int k = MAXIMUM_NUMBER_OF_CHUNKS; k > 0; --k)
	{
		std::string filename_to_check("res/data/chunks/chunk");
		filename_to_check.append(std::to_string(k)).append(".dat");
		const char* filename_to_check_cstr = filename_to_check.c_str();

		std::ifstream file_to_check(filename_to_check_cstr);

		if (file_to_check.good())
		{
			Chunk* chunk = Chunk::set_chunk(k, filename_to_check_cstr);

			// TODO change this
			chunk->width = CHUNK_SIZE;
			chunk->height = CHUNK_SIZE;
		}
	}

	// TODO
	m_Chunk = Chunk::get_chunk(1);
	m_Chunk->load();
}