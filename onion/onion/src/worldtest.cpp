#include <onion.h>
#include "../include/test.h"

using namespace onion;

void worldtest_main()
{
	world::Chunk::set_tile_size(16);

	world::Chunk* chunk = new world::FlatChunk("debug.txt");
	world::World* world = new world::BasicWorld(chunk);
	set_state(new world::WorldState(world));

	Lighting::set_ambient_light(vec3f(0.05f, 0.05f, 0.1f));//(vec3f(0.05f, 0.05f, 0.1f));

	CubeLight* light = new CubeLight();
	light->mins = vec3f(40.f, 50.f, 20.f);
	light->maxs = vec3f(60.f, 60.f, 20.f);
	light->color = { vec3f(0.9f, 0.45f, 0.15f), vec3f() };
	light->attenuation = { 1.f, 0.007f, 0.0002f };//{ 1.f, 0.0014f, 0.000007f };
	Lighting::add(light);

	state_main();
}