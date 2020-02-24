#pragma once

#include <unordered_map>

#include "math.h"
#include "graphics.h"
#include "object.h"

#define CHUNK_KEY int
#define MAXIMUM_NUMBER_OF_CHUNKS 100

#define TILE_WIDTH 32
#define TILE_HEIGHT 24
#define CHUNK_SIZE 32
#define WORLD_SIZE 5


// A block of static information about the world.
class Chunk
{
private:
	// A collection of all chunks
	static std::unordered_map<CHUNK_KEY, Chunk*> m_Chunks;

	// The sprite sheet for the tiles
	static SpriteSheet m_TileSprites;

	// The path to the file containing the chunk's data
	const char* m_ChunkFilepath;

	// Tiles on the ground
	SPRITE_KEY* m_Tiles;

	// Objects in the chunk that don't move
	Object** m_Objects;

	/// <summary>Retrieves the index of the tile/object at the given coordinates.</summary>
	/// <param name="x">The x-coordinate of the index.</param>
	/// <param name="y">The y-coordinate of the index.</param>
	/// <returns>The index of the given coordinates.</returns>
	int get_index(int x, int y);

	/// <summary>Retrieves a reference to the tile at the given coordinates.</summary>
	/// <param name="x">The x-coordinate of the tile.</param>
	/// <param name="y">The y-coordinate of the tile.</param>
	/// <returns>A reference to the tile at the given coordinates.</returns>
	SPRITE_KEY& get_tile(int x, int y);

	/// <summary>Retrieves a reference to the object at the given coordinates.</summary>
	/// <param name="x">The x-coordinate of the object.</param>
	/// <param name="y">The y-coordinate of the object.</param>
	/// <returns>A reference to the object at the given coordinates.</returns>
	Object*& get_object(int x, int y);

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

	// The width of the chunk, in tiles.
	int width;

	// The height of the chunk, in tiles.
	int height;

	/// <summary>Creates a chunk that uses data from the given file.</summary>
	/// <param name="path">The path to the file containing the chunk's data.</param>
	Chunk(const char* path);

	/// <summary>Loads the chunk into memory.</summary>
	void load();

	/// <summary>Unloads the chunk from memory.</summary>
	void unload();

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
	// The current chunk
	Chunk* m_Chunk;

	/// <summary>A transform that does the following:
	/// Translates so the camera is at (0,0) on-screen.
	/// Rotates by -45 degrees, so the positive y-axis is pointing away from the screen.
	/// Scales up the y- and z-axes by sqrt(2).
	/// </summary>
	mat4x4f m_Transform;

	// The camera position
	vec2f m_Camera;

	// The camera bounds, in x- and y-coordinates
	mat2x2i m_Bounds;

public:
	World();

	/// <summary>Sets the position of the camera.</summary>
	/// <param name="x">The x-coordinate of the camera.</param>
	/// <param name="y">The y-coordinate of the camera.</param>
	void camera_set(float x, float y);

	/// <summary>Adjusts the position of the camera.</summary>
	/// <param name="dx">The x-axis adjustment.</param>
	/// <param name="dy">The y-axis adjustment.</param>
	void camera_adjust(float dx, float dy);

	/// <summary>Draws the world to the screen.</summary>
	void display();
};