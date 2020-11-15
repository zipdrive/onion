#pragma once
#include <unordered_set>
#include "../matrix.h"
#include "object.h"
#include "manager.h"

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
			static int m_TileSize;

			// The number of tiles on each axis.
			vec2i m_Dimensions;


			// A visible row of tiles.
			struct TileRow
			{
				// The index to start displaying.
				BUFFER_KEY index;

				// The number of tiles to display in this row.
				Int count;

				/// <summary>Constructs a struct representing a visible row of tiles.</summary>
				/// <param name="index">The index to start displaying.</param>
				/// <param name="count">The number of tiles to display in this row.</param>
				TileRow(BUFFER_KEY index, Int count);
			};

			// All visible rows of tiles, listed from front to back.
			std::vector<TileRow> m_VisibleTiles;


			// The path to the chunk's data, from the res/data/world/ folder.
			const char* m_Path;

			/// <summary>Loads the chunk from file.</summary>
			/// <returns>The vertex data for the buffer.</returns>
			virtual opengl::_VertexBufferData* __load() = 0;

		public:
			/// <summary>Retrieves the size of tiles in all chunks.</summary>
			/// <returns>The size of tiles, in pixels.</returns>
			static int get_tile_size();

			/// <summary>Sets the size of tiles in all chunks.</summary>
			/// <param name="size">The size of each tile.</param>
			static void set_tile_size(int size);


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


			/// <summary>Resets what is visible, in response to the view changing.</summary>
			virtual void reset_visible(const WorldCamera::View* view) = 0;

			/// <summary>Updates what is visible, in response to the passage of time.</summary>
			virtual void update_visible(const WorldCamera::View* view, int frames_passed) = 0;

			/// <summary>Displays tiles in the chunk.</summary>
			void display_tiles() const;

			/// <summary>Displays objects in the chunk.</summary>
			/// <param name="normal">A vector pointing towards the camera.</param>
			virtual void display_objects(const vec3i& normal) const = 0;
		};


		// A chunk where all tiles are flat with a height of 0.
		class FlatChunk : public Chunk
		{
		protected:
			using buffer_t = VertexBufferData<FLOAT_VEC2, FLOAT_VEC2>;

			// The shader used for flat chunks.
			static Shader<FLOAT_MAT4, Int, Int>* m_BasicFlatTileShader;

			/// <summary>Retrieves the tile shader.</summary>
			/// <returns>A pointer to the tile shader.</returns>
			virtual const opengl::_Shader* get_tile_shader() const;

			/// <summary>Activates and sets the uniforms for the tile shader.</summary>
			virtual void activate_tile_shader() const;


			// Manages all objects for the chunk.
			ObjectManager m_Manager;


			/// <summary>Loads the chunk from file.</summary>
			/// <returns>The vertex data for the buffer.</returns>
			virtual opengl::_VertexBufferData* __load();

			/// <summary>Loads the data of a tile.</summary>
			/// <param name="line">The data loaded from a line in the file.</param>
			/// <param name="data">The buffer of data.</param>
			virtual void __load_tile(const StringData& line, buffer_t* data);

			/// <summary>Loads the data of a static object.</summary>
			/// <param name="id">The ID of the object.</param>
			/// <param name="line">The data loaded from a line in the file.</param>
			/// <param name="data">The buffer of data.</param>
			virtual void __load_obj(String id, const StringData& line);

		public:
			/// <summary>Constructs a chunk.</summary>
			/// <param name="path">The path to the data file, from the res/data/world/ folder.</param>
			FlatChunk(const char* path);


			/// <summary>Retrieves the height of the tile at the given world coordinates.</summary>
			/// <param name="x">The x-coordinate in the world.</param>
			/// <param name="y">The y-coordinate in the world.</param>
			/// <returns>The z-coordinate of the ground at the given world coordinates.</returns>
			int get_tile_height(int x, int y) const;


			/// <summary>Resets what is visible, in response to the view changing.</summary>
			void reset_visible(const WorldCamera::View* view);

			/// <summary>Updates what is visible, in response to the passage of time.</summary>
			void update_visible(const WorldCamera::View* view, int frames_passed);

			/// <summary>Displays objects in the chunk.</summary>
			/// <param name="normal">A vector pointing towards the camera.</param>
			void display_objects(const vec3i& normal) const;
		};


		// A chunk where the geometry of the ground is continuous.
		class SmoothChunk : public Chunk
		{
		protected:
			using buffer_t = VertexBufferData<FLOAT_VEC3, FLOAT_VEC3, FLOAT_VEC2>;

			/// <summary>Retrieves the tile shader.</summary>
			/// <returns>A pointer to the tile shader.</returns>
			virtual const opengl::_Shader* get_tile_shader() const;

			/// <summary>Activates and sets the uniforms for the tile shader.</summary>
			virtual void activate_tile_shader() const;


			// The heights of the ground at each corner of a tile.
			std::vector<Int> m_TileCornerHeights;


			// Manages all objects for the chunk.
			ObjectManager m_Manager;


			/// <summary>Loads the chunk from file.</summary>
			/// <returns>The vertex data for the buffer.</returns>
			virtual opengl::_VertexBufferData* __load();

			/// <summary>Loads the data of a tile.</summary>
			/// <param name="line">The data loaded from a line in the file.</param>
			/// <param name="data">The buffer of data.</param>
			virtual void __load_tile(const StringData& line, buffer_t* data);

			/// <summary>Loads the data of a static object.</summary>
			/// <param name="id">The ID of the object.</param>
			/// <param name="line">The data loaded from a line in the file.</param>
			/// <param name="data">The buffer of data.</param>
			virtual void __load_obj(String id, const StringData& line);

		public:
			/// <summary>Constructs a chunk.</summary>
			/// <param name="path">The path to the data file, from the res/data/world/ folder.</param>
			SmoothChunk(const char* path);


			/// <summary>Retrieves the height of the tile at the given world coordinates.</summary>
			/// <param name="x">The x-coordinate in the world.</param>
			/// <param name="y">The y-coordinate in the world.</param>
			/// <returns>The z-coordinate of the ground at the given world coordinates.</returns>
			int get_tile_height(int x, int y) const;


			/// <summary>Resets what is visible, in response to the view changing.</summary>
			void reset_visible(const WorldCamera::View* view);

			/// <summary>Updates what is visible, in response to the passage of time.</summary>
			void update_visible(const WorldCamera::View* view, int frames_passed);

			/// <summary>Displays objects in the chunk.</summary>
			/// <param name="normal">A vector pointing towards the camera.</param>
			void display_objects(const vec3i& normal) const;
		};


	}
}