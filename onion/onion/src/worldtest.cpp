#include <onion.h>
#include "../include/test.h"

using namespace onion;


class TestTexturedObject : public world::Object
{
public:
	TestTexturedObject(const vec3i pos, const world::Textured3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite, const Texture* texture)
		: world::Object(
			new world::Rectangle(pos, vec3i(sprite->width, 0, sprite->height)),
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
		pos = UNITS_PER_PIXEL * (INT_VEC3)pos;

		return new TestTexturedObject(pos, m_SpriteSheet, m_Sprite, m_Texture);
	}
};


class TestActorGenerator : public world::ObjectGenerator
{
protected:
	const world::Flat3DPixelSpriteSheet* m_SpriteSheet;
	const Sprite* m_Sprite;

public:
	TestActorGenerator(String id, const StringData& params) : ObjectGenerator(id)
	{
		m_SpriteSheet = nullptr;
		m_Sprite = nullptr;

		String sprite_sheet;
		if (params.get("sprite_sheet", sprite_sheet))
		{
			if (const _SpriteSheet* ptr = _SpriteSheet::get_sprite_sheet("world/" + sprite_sheet))
			{
				if (m_SpriteSheet = dynamic_cast<const world::Flat3DPixelSpriteSheet*>(ptr))
				{
					String sprite;
					params.get("sprite", sprite);

					m_Sprite = m_SpriteSheet->get_sprite(sprite);
				}
			}
		}
	}

	world::Object* generate(const StringData& params) const
	{
		vec3i pos;
		params.get("pos", pos);
		pos *= UNITS_PER_PIXEL;

		world::Actor* actor = new world::Actor(new world::Point(pos), new world::PlayerMovementControlledAgent(UNITS_PER_PIXEL * 32), new world::FlatWallGraphic3D(m_SpriteSheet, m_Sprite));
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
		m_MaximumRadius *= UNITS_PER_PIXEL;

		if (!params.get("priority", m_Priority))
			m_Priority = std::numeric_limits<Int>::max();
	}

	virtual world::Object* generate(const StringData& params) const
	{
		vec3i pos;
		params.get("pos", pos);
		pos = UNITS_PER_PIXEL * pos;

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

	// Set up the object types
	world::ObjectGenerator::set<TestTexturedObjectGenerator>("__DEBUG:texture");
	world::ObjectGenerator::set<TestActorGenerator>("__DEBUG:actor");
	world::ObjectGenerator::set<TestInteractableWallGenerator>("__DEBUG:interactable");

	// Load the chunk
	world::Chunk::set_tile_size(16);

	world::Chunk* chunk = new world::SmoothChunk("debug.txt");
	world::World* world = new world::BasicWorld(chunk);

	// Lighting setup
	Lighting::add_light_array<CubeLight>("cubeLights", "numCubeLights", 8);
	Lighting::set_ambient_light(vec3f(0.04f, 0.13f, 0.27f));

	// Run the main loop
	set_state(new world::WorldState(world));
	state_main();
}