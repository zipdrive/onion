#include "../../../include/onions/world/world.h"
#include "../../../include/onions/application.h"

namespace onion
{
	namespace world
	{

		World::World() : m_Camera(nullptr) {}
		
		World::~World()
		{
			delete m_Camera;
		}


		WorldState::WorldState(World* world)
		{
			m_World = world;
		}

		void WorldState::set_bounds(int width, int height)
		{
			int depth = width > height ? width : height;
			m_World->set_bounds(0, 0, -depth, width, height, depth);
		}

		void WorldState::display() const
		{
			m_World->display();
		}


		BasicWorld::BasicWorld(Chunk* chunk)
		{
			m_Chunk = nullptr;
			set_chunk(chunk);

			m_Camera = new StaticTopDownWorldCamera(m_Bounds);
		}
		
		void BasicWorld::__display() const
		{
			// Activate the camera
			m_Camera->activate();

			// Display the chunk
			m_Chunk->display_tiles();
		}

		void BasicWorld::set_chunk(Chunk* chunk)
		{
			if (m_Chunk)
				m_Chunk->unload();

			m_Chunk = chunk;

			if (m_Chunk)
				m_Chunk->load();
		}

	}
}