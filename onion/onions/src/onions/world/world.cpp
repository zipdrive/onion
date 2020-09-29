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
			// Display the chunk
			m_Chunk->display_tiles();
		}

	}
}