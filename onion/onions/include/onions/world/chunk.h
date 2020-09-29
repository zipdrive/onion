#pragma once
#include <unordered_set>
#include "../matrix.h"
#include "object.h"

namespace onion
{
	namespace world
	{

		struct Tile
		{
			// The key of the sprite 
			SPRITE_KEY key;

			// The heights of each corner.
			mat2i heights;
		};

		class Chunk
		{
		protected:
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

			
			// The size of each tile.
			static unsigned int m_TileSize;


			// The path to the chunk's data, from the res/data/world/ folder.
			const char* m_Path;

			// True if the chunk is loaded, false otherwise.
			bool m_IsLoaded;


			
			// The shader for the tiles.
			static opengl::_Shader* m_TileShader;
			
			// The image that stores the tiles.
			opengl::_Image* m_TileImage = nullptr;

			// Used to display rows of tiles.
			opengl::_VertexBufferDisplayer* m_Displayer = nullptr;


			// The x-axis width of the chunk.
			int m_Width;

			// The y-axis height of the chunk.
			int m_Height;

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

	}
}