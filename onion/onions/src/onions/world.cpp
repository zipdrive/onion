#include <fstream>
#include <string>
#include "../../include/onions/world.h"
#include "../../include/onions/fileio.h"


#define MAXIMUM_NUMBER_OF_CHUNKS 100
#define CHUNK_SIZE 64
#define CHUNK_INDEX(x, y) ((x) + (width * (y)))

using namespace std;


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

Chunk::Chunk(const char* name) : name(name)
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
		// Load the chunk
		string path("res/data/chunks/");
		path.append(name);
		path.append(".dat");

		LoadFile file(path);

		if (file.good())
		{
			width = file.load_int();
			height = file.load_int();

			int s = width * height; // Size of arrays

			// Load the tiles
			m_Tiles = new SPRITE_KEY[s];
			for (int k = (width * height) - 1; k >= 0; --k)
			{
				m_Tiles[k] = file.load_int();
			}

			// Load the objects
			m_Objects = new Object*[s];
			memset(m_Objects, NULL, s * sizeof(Object*));
		}
		else
		{
			int s = width * height; // Size of arrays

			// Load the tiles
			m_Tiles = new SPRITE_KEY[s];
			memset(m_Tiles, 0, s * sizeof(SPRITE_KEY));

			// Load the objects
			m_Objects = new Object*[s];
			memset(m_Objects, NULL, s * sizeof(Object*));
		}
	}
}

void Chunk::unload()
{
	if (m_Objects) // Make sure chunk has been loaded previously.
	{
		// Save the chunk
		string path("res/data/chunks/");
		path.append(name);
		path.append(".dat");

		SaveFile file(path);

		// Save the width and height
		file.save_int(width);
		file.save_int(height);

		int s = width * height;

		// Save the tiles
		for (int k = s - 1; k >= 0; --k)
		{
			file.save_int(m_Tiles[k]);
		}

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

void Chunk::set_tile(int x, int y, SPRITE_KEY tile)
{
	m_Tiles[CHUNK_INDEX(x, y)] = tile;
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
	// Load information about chunks
	LoadFile file("res/data/world.dat");

	if (file.good())
	{
		while (file.good())
		{
			CHUNK_KEY key = file.load_int();
			string name = file.load_string();

			Chunk::set_chunk(key, name.c_str());
		}
	}
	else
	{
		// TODO
	}
}