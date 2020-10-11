#include <onion.h>
#include "../include/test.h"

using namespace onion;

void worldtest_main()
{
	// Load the chunk
	world::Chunk::set_tile_size(16);

	world::Chunk* chunk = new world::FlatChunk("debug.txt");
	world::World* world = new world::BasicWorld(chunk);

	// Lighting setup
	Lighting::add_light_array<CubeLight>("cubeLights", "numCubeLights", 8);
	Lighting::set_ambient_light(vec3f(0.04f, 0.13f, 0.27f));

	// Run the main loop
	set_state(new world::WorldState(world));
	state_main();
}