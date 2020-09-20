#pragma once
#include "../graphics/frame.h"
#include "chunk.h"
#include "camera.h"

namespace onion
{
	namespace world
	{

		// Handles loading and displaying chunks
		class World : public Frame
		{
		protected:
			// The camera that displays the world.
			Camera* m_Camera;

			/// <summary>Generates the camera object.</summary>
			virtual void set_camera() = 0;

		public:
			/// <summary>Sets up the camera.</summary>
			World();

			/// <summary>Destroys the camera.</summary>
			~World();
		};


		// A world where only one chunk is loaded into memory at a time.
		class ChunkWorld : public World
		{
		protected:
			// Chunks that have been loaded into memory.
			Chunk* m_Chunk;

			/// <summary>Displays the world.</summary>
			void __display() const;
		};

	}
}