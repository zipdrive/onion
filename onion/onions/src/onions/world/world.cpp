#include "../../../include/onions/world/world.h"

namespace onion
{
	namespace world
	{

		World::World()
		{
			set_camera();
		}
		
		World::~World()
		{
			delete m_Camera;
		}


		void ChunkWorld::__display() const
		{
			// Set up the camera
			MatrixStack& c = camera();
			c.push();
			c.custom(m_Camera->get_transform());

			// Display the chunk
			m_Chunk->display_tiles();

			// Clean up the camera
			c.pop();
		}

	}
}