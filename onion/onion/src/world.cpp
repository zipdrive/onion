#include "../include/world.h"
#include "../include/game.h"
#include "../include/math.h"


#define CHUNK_INDEX(x, y) ((x) + (width * (y)))


SpriteSheet Chunk::m_TileSprites;

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

int Chunk::get_index(int x, int y)
{
	return CHUNK_INDEX(x, y);
}

SPRITE_KEY& Chunk::get_tile(int x, int y)
{
	return m_Tiles[CHUNK_INDEX(x, y)];
}

Object*& Chunk::get_object(int x, int y)
{
	return m_Objects[CHUNK_INDEX(x, y)];
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
				m_TileSprites.display(m_Tiles[CHUNK_INDEX(i, j)]);
			}
			mat_pop();
		}
		mat_pop();
	}
}


void World::camera_set(float x, float y)
{
	// Sets the camera object
	m_Camera.set(0, 0, x);
	m_Camera.set(1, 0, y);

	// Prepares to set transform center and bounds
	Application* app = get_application_settings();

	int cx = (int)roundf(x); // the centered x-coordinate
	int cy = (int)roundf(y); // the centered y-coordinate
	int wHalf = app->width / 2; // half the width of the screen
	int hHalf = app->height / 2; // half the height of the screen
	int wc = (m_Chunk->width * TILE_WIDTH) - 1; // width of chunk, in pixels (minus 1)
	int hc = (m_Chunk->height * TILE_HEIGHT) - 1; // height of chunk, in pixels (minus 1)

	if (cx - wHalf < 0)
	{
		// If left edge would be less than 0, set left edge to be 0.
		m_Bounds.set(0, 0, 0);
		m_Bounds.set(0, 1, 2 * wHalf);
		m_Transform.set(0, 3, -wHalf);
	}
	else if (cx + wHalf >= wc)
	{
		// If right edge would be greater than width of chunk, set right edge to width of chunk.
		m_Bounds.set(0, 0, wc - (2 * wHalf));
		m_Bounds.set(0, 1, wc);
		m_Transform.set(0, 3, wHalf - wc);
	}
	else
	{
		// Set center to camera coordinates
		m_Bounds.set(0, 0, cx - wHalf);
		m_Bounds.set(0, 1, cx + wHalf);
		m_Transform.set(0, 3, -cx);
	}

	if (cy - hHalf < 0)
	{
		// If bottom edge would be less than 0, set bottom edge to be 0.
		m_Bounds.set(1, 0, 0);
		m_Bounds.set(1, 1, 2 * hHalf);
		m_Transform.set(1, 3, -hHalf);
		m_Transform.set(2, 3, -hHalf);
	}
	else if (cy + hHalf >= hc)
	{
		// If top edge would be greater than height of chunk, set top edge to height of chunk.
		m_Bounds.set(1, 0, hc - (2 * hHalf));
		m_Bounds.set(1, 1, hc);
		m_Transform.set(1, 3, hHalf - hc);
		m_Transform.set(2, 3, hHalf - hc);
	}
	else
	{
		m_Bounds.set(1, 0, cy - hHalf);
		m_Bounds.set(1, 1, cy + hHalf);
		m_Transform.set(1, 3, -cy);
		m_Transform.set(2, 3, -cy);
	}
}

void World::camera_adjust(float dx, float dy)
{
	m_Camera.get(0) -= dx;
	m_Camera.get(1) -= dy;
}

void World::display()
{
	// Set up transformation.
	mat_push();
	mat_custom_transform(m_Transform);

	// Draw chunk
	m_Chunk->display(m_Bounds.get(0, 0), m_Bounds.get(1, 0), m_Bounds.get(0, 1), m_Bounds.get(1, 1));

	mat_pop();
}