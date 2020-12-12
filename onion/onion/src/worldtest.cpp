#include <onion.h>
#include "../include/test.h"

using namespace onion;


class TestTexturedObject : public world::Object
{
public:
	TestTexturedObject(const vec3i pos, const world::Textured3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite, const Texture* texture)
		: world::Object(
			new world::UprightRectangle(pos, vec3i(sprite->width, 0, sprite->height)),
			new world::DynamicShadingSpriteGraphic3D(
				sprite_sheet,
				{ sprite },
				false,
				texture,
				new SinglePalette(
					vec4f(1.f, 0.f, 0.f, 0.f),
					vec4f(0.f, 1.f, 0.f, 0.f),
					vec4f(0.f, 0.f, 1.f, 0.f)
				)
			)
		) {}
};

class TestTexturedObjectGenerator : public world::ObjectGenerator
{
private:
	// The sprite sheet.
	const world::Textured3DPixelSpriteSheet* m_SpriteSheet;

	// The sprite.
	const Sprite* m_Sprite;

	// The texture.
	const Texture* m_Texture;

public:
	TestTexturedObjectGenerator(String id, const StringData& params) : world::ObjectGenerator(id)
	{
		String path;
		if (params.get("sprite_sheet", path))
		{
			if (const _SpriteSheet* sprite_sheet = _SpriteSheet::get_sprite_sheet("world/" + path))
			{
				if (m_SpriteSheet = dynamic_cast<const world::Textured3DPixelSpriteSheet*>(sprite_sheet))
				{
					String sprite;
					params.get("sprite", sprite);
					m_Sprite = m_SpriteSheet->get_sprite(sprite);

					String texture;
					params.get("texture", texture);
					m_Texture = m_SpriteSheet->get_texture(texture);
				}
			}
		}
	}

	world::Object* generate(const StringData& params) const
	{
		vec3i pos;
		params.get("pos", pos);

		return new TestTexturedObject(pos, m_SpriteSheet, m_Sprite, m_Texture);
	}
};


class TestFurryActorGenerator : public world::FurryGenerator
{
public:
	TestFurryActorGenerator(String id, const StringData& params) : world::FurryGenerator(id, params) {}

	world::Object* generate(const StringData& params) const
	{
		vec3i pos;
		params.get("pos", pos);

		world::Actor* actor = new world::Actor(
			new world::OrthogonalPrism(pos, vec3i(24, 24, 60)), 
			new world::PlayerMovementControlledAgent(world::SubpixelHandler::num_subpixels * 80), 
			generate_graphic(params)
		);

		world::_Interactable::set_interactor(actor);
		return actor;
	}
};

class TestHuneActorGenerator : public world::ObjectGenerator
{
public:
	TestHuneActorGenerator(String id, const StringData& params) : ObjectGenerator(id) {}

	world::Object* generate(const StringData& params) const
	{
		vec3i pos;
		params.get("pos", pos);

		world::Actor* actor = new world::HuneActor(
			new world::OrthogonalPrism(pos, vec3i(24, 24, 60)),
			new world::PlayerMovementControlledAgent(world::SubpixelHandler::num_subpixels * 80),
			new world::HuneCreator()
		);

		world::_Interactable::set_interactor(actor);
		return actor;
	}
};


class TestInteractableWall : public world::_InteractableObject<world::XAlignedWall>
{
public:
	TestInteractableWall(const vec3i& pos, const world::Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite, Int maximum_radius, Int priority) : world::_InteractableObject<world::XAlignedWall>(maximum_radius, priority, pos, sprite_sheet, sprite) {}

	bool interact()
	{
		errlog("TEST: TestInteractableWall interacted with successfully!");
		return true;
	}
};

class TestInteractableWallGenerator : public world::XAlignedWallGenerator
{
protected:
	Int m_MaximumRadius;
	Int m_Priority;

public:
	TestInteractableWallGenerator(String id, const StringData& params) : world::XAlignedWallGenerator(id, params) 
	{
		if (!params.get("interact_radius", m_MaximumRadius))
			m_MaximumRadius = 0;

		if (!params.get("priority", m_Priority))
			m_Priority = std::numeric_limits<Int>::max();
	}

	virtual world::Object* generate(const StringData& params) const
	{
		vec3i pos;
		params.get("pos", pos);

		return new TestInteractableWall(pos, m_SpriteSheet, m_Sprite, m_MaximumRadius, m_Priority);
	}
};




void worldtest_main()
{
	// Register keys
	register_keyboard_control(ONION_KEY_LEFT);
	register_keyboard_control(ONION_KEY_RIGHT);
	register_keyboard_control(ONION_KEY_DOWN);
	register_keyboard_control(ONION_KEY_UP);
	register_keyboard_control(ONION_KEY_SELECT);

	// Load the sprite sheets
	new world::Flat3DPixelSpriteSheet("sprites/debug.png");
	new world::Flat3DPixelSpriteSheet("sprites/debug2.png");
	new world::Textured3DPixelSpriteSheet("sprites/debugtexture.png");

	//world::load_furry_components();

	// Set up the object types
	world::ObjectGenerator::set<TestTexturedObjectGenerator>("__DEBUG:texture");
	world::ObjectGenerator::set<TestHuneActorGenerator>("__DEBUG:actor");
	world::ObjectGenerator::set<TestInteractableWallGenerator>("__DEBUG:interactable");

	// Generate the player
	StringData actor_data;
	actor_data.set("pos", vec3i(180, 120, 0));
	world::Actor* player = dynamic_cast<world::Actor*>(world::ObjectGenerator::generate("DEBUG actor", actor_data));

	// Load the chunk
	world::Chunk::set_tile_size(16);
	world::Chunk* chunk = new world::SmoothChunk("debug.txt");
	chunk->add(player);

	world::World* world = new world::ActorFollowingBasicWorld(player, vec3f(0.25f, 0.25f, 0.f), chunk);
	//world::World* world = new world::BasicWorld(chunk);

	// Lighting setup
	Lighting::add_light_array<CubeLight>("cubeLights", "numCubeLights", 8);
	Lighting::set_ambient_light(vec3f(0.04f, 0.13f, 0.27f));
	//Lighting::set_ambient_light(vec3f(1.f, 1.f, 1.f));

	// Run the main loop
	set_state(new world::WorldState(world));
	state_main();
}