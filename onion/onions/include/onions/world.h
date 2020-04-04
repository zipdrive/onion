#pragma once

#include "graphics.h"


// The key to a chunk of static data
#define CHUNK_KEY int

// The width and height of a tile in the world.
#define TILE_WIDTH 16
#define TILE_HEIGHT 16



// A thing within the game world.
class Object
{
public:
	/// <summary>Display the object.</summary>
	virtual void display() = 0;
};



//typedef void(*tileClickCallbackFunc)(int x, int y);

// A block of static information about the world.
class Chunk
{
private:
	// A collection of all chunks
	static std::unordered_map<CHUNK_KEY, Chunk*> m_Chunks;

	// The sprite sheet for the tiles
	static SpriteSheet* m_TileSprites;

	// Tiles on the ground
	SPRITE_KEY* m_Tiles;

	// Objects in the chunk that don't move
	Object** m_Objects;

public:
	/// <summary>Retrieves the chunk with the given key.</summary>
	/// <param name="key">The key of the chunk</param>
	/// <returns>A pointer to the chunk with the given key</returns>
	static Chunk* get_chunk(CHUNK_KEY key);

	/// <summary>Creates a chunk with the given key and file path.</summary>
	/// <param name="key">The key of the chunk</param>
	/// <param name="path">The path to the file containing the chunk's data</param>
	/// <returns>A pointer to the created chunk</returns>
	static Chunk* set_chunk(CHUNK_KEY key, const char* path);

	/// <summary>Retrieves the sprite sheet of tiles.</summary>
	/// <returns>A pointer to the tile sprite sheet.</returns>
	static SpriteSheet* get_tile_sprite_sheet();

	// The name of the chunk
	const char* name;

	// The width of the chunk, in tiles.
	int width;

	// The height of the chunk, in tiles.
	int height;

	/// <summary>Creates a chunk that uses data from the given file.</summary>
	/// <param name="name">The name of the chunk.</param>
	Chunk(const char* name);

	/// <summary>Loads the chunk into memory.</summary>
	void load();

	/// <summary>Unloads the chunk from memory.</summary>
	void unload();

	/// <summary>Sets the sprite of the tile at the given coordinates.</summary>
	/// <param name="x">The x-coordinate of the tile.</param>
	/// <param name="y">The y-coordinate of the tile.</param>
	/// <param name="tile">The key of the tile.</param>
	void set_tile(int x, int y, SPRITE_KEY tile);

	/// <summary>Draws the tiles of the chunk to the screen.</summary>
	/// <param name="xmin">The leftmost x-coordinate to draw.</param>
	/// <param name="xmax">The rightmost x-coordinate to draw.</param>
	/// <param name="ymin">The bottommost y-coordinate to draw.</param>
	/// <param name="ymax">The topmost y-coordinate to draw.</param>
	void display(int xmin, int ymin, int xmax, int ymax);
};

// All currently loaded information about the world.
class World
{
private:
	// The singleton world object
	static World* m_World;

	// The current chunk
	Chunk* m_Chunk;

	/// <summary>Loads the world.</summary>
	World();

public:
	/// <summary>Retrieves the world information.</summary>
	/// <returns>A pointer to the singleton world object.</summary>
	static World* get_world();

	/// <summary>Retrieves a pointer to the current chunk.</summary>
	/// <returns>A pointer to the current chunk.</returns>
	static Chunk* get_chunk();
};