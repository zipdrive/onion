#pragma once
#include <unordered_set>
#include "../matrix.h"
#include "object.h"

namespace onion
{
	namespace world
	{


#define TILE_CORNER_BOTTOM_LEFT		0
#define TILE_CORNER_BOTTOM_RIGHT	1
#define TILE_CORNER_TOP_RIGHT		2
#define TILE_CORNER_TOP_LEFT		3

		class Chunk
		{
		private:
			// Manages an image of tile sprites.
			struct TileImageManager
			{
				// The image managed by this struct.
				opengl::_Image* image;

				// Chunks in memory that use the image.
				std::unordered_set<Chunk*> chunks;

				/// <summary>Constructs a manager for a tile sprite image.</summary>
				/// <param name="image">The image to manage.</param>
				/// <param name="chunk">The first chunk that needed to load the image.</param>
				TileImageManager(opengl::_Image* image, Chunk* chunk);
			};

			// Images containing tile sprites that are currently loaded in memory.
			static std::unordered_map<std::string, TileImageManager*> m_Images;

			// The image that stores the tiles.
			opengl::_Image* m_TileImage = nullptr;


			// Used to display rows of tiles.
			opengl::_VertexBufferDisplayer* m_Displayer = nullptr;


			// True if the chunk is loaded, false otherwise.
			bool m_IsLoaded;

		protected:
			/// <summary>Retrieves the tile shader.</summary>
			/// <returns>A pointer to the tile shader.</returns>
			virtual const opengl::_Shader* get_tile_shader() const = 0;

			/// <summary>Activates and sets the uniforms for the tile shader.</summary>
			virtual void activate_tile_shader() const = 0;

			
			/// <summary>Retrieves the image that stores the tiles.</summary>
			const opengl::_Image* get_tile_image() const;

			/// <summary>Sets the image that stores the tiles.</summary>
			/// <param name="path">The path to the image containing the tiles, from the res/img/world/tiles/ folder.</param>
			void set_tile_image(std::string path);

			/// <summary>Unsets the image storing the tiles, and unloads it from memory if no other chunk is using it.</summary>
			void unset_tile_image();


			// The size of each tile.
			static unsigned int m_TileSize;

			// The number of tiles on each axis.
			vec2i m_Dimensions;


			// The path to the chunk's data, from the res/data/world/ folder.
			const char* m_Path;

			/// <summary>Loads the chunk from file.</summary>
			/// <returns>The vertex data for the buffer.</returns>
			virtual opengl::_VertexBufferData* __load() = 0;

		public:
			/// <summary>Retrieves the size of tiles in all chunks.</summary>
			/// <returns>The size of tiles, in pixels.</returns>
			static unsigned int get_tile_size();

			/// <summary>Sets the size of tiles in all chunks.</summary>
			/// <param name="size">The size of each tile.</param>
			static void set_tile_size(unsigned int size);


			/// <summary>Constructs a chunk.</summary>
			/// <param name="path">The path to the data file, from the res/data/world/ folder.</param>
			Chunk(const char* path);


			/// <summary>Retrieves the index of a tile.</summary>
			/// <param name="x">The x-coordinate of the tile.</param>
			/// <param name="y">The y-coordinate of the tile.</param>
			/// <returns>The index of the tile.</returns>
			int get_index(int x, int y) const;


			/// <summary>Retrieves the height of the tile at the given world coordinates.</summary>
			/// <param name="x">The x-coordinate in the world.</param>
			/// <param name="y">The y-coordinate in the world.</param>
			/// <returns>The z-coordinate of the ground at the given world coordinates.</returns>
			virtual int get_tile_height(int x, int y) const = 0;


			/// <summary>Checks whether the chunk has been loaded.</summary>
			/// <returns>True if the chunk has been loaded, false otherwise.</returns>
			bool is_loaded() const;

			/// <summary>Loads the chunk from its data file.</summary>
			void load();

			/// <summary>Unloads the chunk, freeing any memory it's using.</summary>
			void unload();


			/// <summary>Displays tiles in the chunk.</summary>
			void display_tiles() const;
		};


		class FlatChunk : public Chunk
		{
		protected:
			// 
			static Shader<matrix<float, 4, 4>, int>* m_BasicFlatTileShader;

			/// <summary>Retrieves the tile shader.</summary>
			/// <returns>A pointer to the tile shader.</returns>
			virtual const opengl::_Shader* get_tile_shader() const;

			/// <summary>Activates and sets the uniforms for the tile shader.</summary>
			virtual void activate_tile_shader() const;


			/// <summary>Loads the chunk from file.</summary>
			/// <returns>The vertex data for the buffer.</returns>
			virtual opengl::_VertexBufferData* __load();

		public:
			/// <summary>Constructs a chunk.</summary>
			/// <param name="path">The path to the data file, from the res/data/world/ folder.</param>
			FlatChunk(const char* path);


			/// <summary>Retrieves the height of the tile at the given world coordinates.</summary>
			/// <param name="x">The x-coordinate in the world.</param>
			/// <param name="y">The y-coordinate in the world.</param>
			/// <returns>The z-coordinate of the ground at the given world coordinates.</returns>
			int get_tile_height(int x, int y) const;
		};


	}
}