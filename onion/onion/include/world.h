#pragma once

#include "graphics.h"

#define TILE_WIDTH 32
#define TILE_HEIGHT 24
#define CHUNK_SIZE 32
#define ZONE_SIZE 4
#define WORLD_SIZE 3


// A block of static information about the world.
class Chunk
{
private:
	static SpriteSheet m_TileSprites;

	SPRITE_KEY m_Tiles[CHUNK_SIZE][CHUNK_SIZE];

public:
	/// <summary>Loads the chunk.</summary>
	Chunk();

	/// <summary>Draws the chunk to the screen.</summary>
	/// <param name="xmin">The leftmost x-coordinate to draw.</param>
	/// <param name="xmax">The rightmost x-coordinate to draw.</param>
	/// <param name="ymin">The bottommost y-coordinate to draw.</param>
	/// <param name="ymax">The topmost y-coordinate to draw.</param>
	void display(int xmin, int xmax, int ymin, int ymax);
};

// A block of static and dynamic information about the world.
class Zone
{
private:
	Chunk* m_Chunks[ZONE_SIZE][ZONE_SIZE];

public:
	/// <summary>Draws the zone to the screen.</summary>
	/// <param name="xmin">The leftmost x-coordinate to draw.</param>
	/// <param name="xmax">The rightmost x-coordinate to draw.</param>
	/// <param name="ymin">The bottommost y-coordinate to draw.</param>
	/// <param name="ymax">The topmost y-coordinate to draw.</param>
	void display(int xmin, int xmax, int ymin, int ymax);
};

// All currently loaded information about the world.
class World
{
private:
	Zone* m_Zones[WORLD_SIZE][WORLD_SIZE];

public:
	/// <summary>Draws the world to the screen.</summary>
	void display();
};