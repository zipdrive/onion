#include <onion.h>
#include "../include/test.h"

using namespace onion;

void worldtest_main()
{
	world::Chunk::set_tile_size(16);

	world::Chunk* chunk = new world::FlatChunk("debug.txt");
	world::World* world = new world::BasicWorld(chunk);
	set_state(new world::WorldState(world));

	state_main();
}