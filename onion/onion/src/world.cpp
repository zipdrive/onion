#include "../include/world.h"
#include "../include/math.h"


SpriteSheet Chunk::m_TileSprites;

Chunk::Chunk()
{
	// Load tile sprites
	if (!m_TileSprites.sprites_loaded())
	{
		m_TileSprites.load_sprite_sheet("res/img/tiles.png");

		int imax = m_TileSprites.width / TILE_WIDTH;
		int jmax = m_TileSprites.height / TILE_HEIGHT;

		std::vector<SpriteInfo*> sprites;
		for (int i = 0; i < imax; ++i)
		{
			for (int j = 0; j < jmax; ++j)
			{
				sprites.push_back(new SpriteInfo(TILE_WIDTH * i, TILE_HEIGHT * j, TILE_WIDTH, TILE_HEIGHT));
			}
		}

		m_TileSprites.load_sprites(sprites);

		for (auto iter = sprites.begin(); iter != sprites.end(); ++iter)
		{
			delete *iter;
		}
	}

	for (int i = CHUNK_SIZE - 1; i >= 0; --i)
	{
		for (int j = CHUNK_SIZE - 1; j >= 0; --j)
		{
			m_Tiles[i][j] = 6 * ((i + j) % 4);
		}
	}
}

void Chunk::display(int xmin, int xmax, int ymin, int ymax)
{
	// Draw tiles
	int imin = xmin / TILE_WIDTH;
	int imax = xmax / TILE_WIDTH;
	int jmin = ymin / TILE_HEIGHT;
	int jmax = ymax / TILE_HEIGHT;

	mat_push();
	mat_translate((imin - 1) * TILE_WIDTH, (jmin - 1) * TILE_HEIGHT, 0.f);

	for (int i = imin; i <= imax; ++i)
	{
		mat_translate(TILE_WIDTH, 0.f, 0.f);
		mat_push();

		for (int j = jmin; j <= jmax; ++j)
		{
			mat_translate(0.f, TILE_HEIGHT, 0.f);
			m_TileSprites.display(m_Tiles[i][j]);
		}
		mat_pop();
	}
	mat_pop();
}