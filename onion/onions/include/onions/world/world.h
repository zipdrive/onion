#pragma once
#include "../graphics/frame.h"
#include "../state.h"
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
			WorldCamera* m_Camera;

		public:
			/// <summary>Sets up the camera.</summary>
			World();

			/// <summary>Destroys the camera.</summary>
			virtual ~World();

			/// <summary>Displays the world.</summary>
			virtual void display() const;
		};

		// A state that displays the world.
		class WorldState : public State
		{
		protected:
			// The world.
			World* m_World;

		public:
			/// <summary>Constructs a state that displays the world.</summary>
			/// <param name="world">The world to display.</param>
			WorldState(World* world);

			/// <summary>Is called when the state is made or when the window is resized.</summary>
			/// <param name="width">The width of the window.</param>
			/// <param name="height">The height of the window.</param>
			virtual void set_bounds(int width, int height);

			/// <summary>Displays the world.</summary>
			virtual void display() const;
		};


		// A world where only one chunk is loaded into memory at a time and the camera is fixed.
		class BasicWorld : public World
		{
		protected:
			// The current chunk loaded into memory.
			Chunk* m_Chunk;

			/// <summary>Displays the world.</summary>
			void __display() const;

		public:
			/// <summary>Sets the current chunk
			BasicWorld(Chunk* chunk);

			/// <summary>Sets the chunk being displayed.</summary>
			void set_chunk(Chunk* chunk);
		};

	}
}