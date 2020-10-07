#include <onion.h>
#include "../include/test.h"

using namespace onion;

void worldtest_main()
{
	world::Chunk::set_tile_size(16);

	world::Chunk* chunk = new world::FlatChunk("debug.txt");

	Lighting::set_ambient_light(vec3f(0.04f, 0.13f, 0.27f));//(vec3f(0.05f, 0.05f, 0.1f));

	CubeLight* light = new CubeLight();
	light->mins = vec3f(200.f, 200.f, 50.f);
	light->maxs = vec3f(240.f, 210.f, 50.f);
	light->color = { vec3f(0.9f, 0.45f, 0.15f), 0.25f };
	light->radius = 250.f;

	CubeLight* light2 = new CubeLight();
	light2->mins = vec3f(80.f, 80.f, 10.f);
	light2->maxs = vec3f(90.f, 90.f, 10.f);
	light2->color = { vec3f(0.6f, 0.95f, 0.95f), 0.25f };
	light2->radius = 159.f;

	Lighting::add(light);
	Lighting::add(light2);

	world::World* world = new world::BasicWorld(chunk);
	set_state(new world::WorldState(world));
	state_main();
}