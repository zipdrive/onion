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

		void World::__set_bounds()
		{
			m_Camera->__set_bounds();
			reset_camera();
		}

		void World::display() const
		{
			if (m_Camera)
				m_Camera->activate();
			__display();
		}


		WorldState::WorldState(World* world)
		{
			m_World = world;
		}

		void WorldState::set_bounds(int width, int height)
		{
			int depth = width > height ? width : height;
			m_World->set_bounds(0, 0, depth, width, height, -depth);
		}

		void WorldState::display() const
		{
			m_World->display();
		}


		BasicWorld::BasicWorld(Chunk* chunk)
		{
			m_Chunk = nullptr;
			set_chunk(chunk);

			m_Camera = new StaticBoundedTopDownWorldCamera(m_Bounds, vec3i(0, 0, 0), vec3i(chunk->get_tile_size() * chunk->get_dimensions(), 0));
			//m_Camera = new StaticTopDownWorldCamera(m_Bounds);
			//m_Camera = new DynamicAxonometricWorldCamera(m_Bounds, 0.f, 0.f);
			m_Camera->set_position(vec3i(200, 0, 0));

			unfreeze(INT_MAX);
		}

		void BasicWorld::reset_camera()
		{
			m_Chunk->reset_visible(m_Camera);
		}

		void BasicWorld::update(int frames_passed)
		{
			m_Chunk->update_visible(m_Camera, frames_passed);
		}
		
		void BasicWorld::__display() const
		{
			// Display the chunk
			m_Chunk->display_tiles();
			m_Chunk->display_objects(m_Camera->get_normal());
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