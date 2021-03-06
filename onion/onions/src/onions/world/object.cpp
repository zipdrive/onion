#include "../../../include/onions/world/object.h"
#include "../../../include/onions/world/lighting.h"
#include "../../../include/onions/world/graphic.h"

namespace onion
{
	namespace world
	{

		Object::Object(Shape* bounds, Graphic3D* graphic)
		{
			m_Bounds = bounds;
			m_Graphic = graphic;
		}

		Object::~Object()
		{
			// Free the bounds shape
			if (m_Bounds)
				delete m_Bounds;

			// Free the graphic
			if (m_Graphic)
				delete m_Graphic;
		}

		Shape* Object::get_bounds()
		{
			return m_Bounds;
		}

		const Shape* Object::get_bounds() const
		{
			return m_Bounds;
		}

		bool Object::__collision(Object* obj)
		{
			return false;
		}

		bool Object::collision(Object* obj)
		{
			if (m_Bounds->get_distance(obj->get_bounds()) == 0)
			{
				return __collision(obj);
			}

			return false;
		}

		void Object::display(const vec3i& normal) const 
		{
			if (m_Graphic)
			{
				const vec3i& pos = m_Bounds->get_position();

				// Set up the transform
				Transform::model.push();
				Transform::model.translate(pos.get(0), pos.get(1), pos.get(2));

				// Display the graphic
				m_Graphic->display(normal);

				// Clean up
				Transform::model.pop();
			}
		}



		std::unordered_map<std::string, ObjectGenerator*> ObjectGenerator::m_Generators{};
		std::unordered_map<std::string, ObjectGenerator::Generator*> ObjectGenerator::m_GeneratorFinder{};

		ObjectGenerator::ObjectGenerator(std::string id)
		{
			m_Generators.emplace(id, this);
		}

		void ObjectGenerator::init()
		{
			// Register all types included as part of the Onion library
			// Lights
			set<CubeLightObjectGenerator>("light:cube");
			set<FlickeringCubeLightObjectGenerator>("light:cube:flickering");
			set<ConeLightObjectGenerator>("light:cone");
			set<FlickeringConeLightObjectGenerator>("light:cone:flickering");

			// Walls and other static objects
			set<XAlignedWallGenerator>("wall:xalign");
			set<YAlignedWallGenerator>("wall:yalign");


			// Load all objects from file
			LoadFile file("res/data/world/objects.txt");
			while (file.good())
			{
				StringData line;
				std::string id = file.load_data(line);

				std::string type;
				if (line.get("type", type))
				{
					auto iter = m_GeneratorFinder.find(type);
					if (iter != m_GeneratorFinder.end())
					{
						iter->second->generate(id, line);
					}
				}
			}
		}

		Object* ObjectGenerator::generate(std::string id, const StringData& params)
		{
			auto iter = m_Generators.find(id);
			if (iter != m_Generators.end())
				return iter->second->generate(params);
			
			// Check if generators haven't been loaded yet
			if (m_Generators.empty())
			{
				init();

				iter = m_Generators.find(id);
				if (iter != m_Generators.end())
					return iter->second->generate(params);
			}

			return nullptr;
		}



		Wall::Wall(Shape* bounds, Graphic3D* graphic) : Object(bounds, graphic) {}
		
		bool Wall::__collision(Object* obj)
		{
			return true;
		}


		XAlignedWall::XAlignedWall(const vec3i& pos, const Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite) :
			Wall(
				new UprightRectangle(pos, vec3i(sprite->width, 0, sprite->height)),
				(sprite_sheet != nullptr && sprite != nullptr) ? new FlatWallGraphic3D(sprite_sheet, sprite) : nullptr
			) {}

		YAlignedWall::YAlignedWall(const vec3i& pos, const Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite) :
			Wall(
				new UprightRectangle(pos, vec3i(0, sprite->width, sprite->height)),
				(sprite_sheet != nullptr && sprite != nullptr) ? new TransformedFlatWallGraphic3D(sprite_sheet, sprite, vec2f(0.f, 1.f)) : nullptr
			) {}

	}
}