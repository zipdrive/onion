#include "../../../include/onions/world/object.h"
#include "../../../include/onions/world/lighting.h"

namespace onion
{
	namespace world
	{

		Object::Object(Shape* bounds)
		{
			m_Bounds = bounds;
		}

		Object::~Object()
		{
			if (m_Bounds)
				delete m_Bounds;
		}

		const Shape* Object::get_bounds() const
		{
			return m_Bounds;
		}

		void Object::display(const vec2i& direction) const {}



		std::unordered_map<std::string, ObjectGenerator*> ObjectGenerator::m_Generators{};
		std::unordered_map<std::string, ObjectGenerator::Generator*> ObjectGenerator::m_GeneratorFinder{};

		ObjectGenerator::ObjectGenerator(std::string id)
		{
			m_Generators.emplace(id, this);
		}

		void ObjectGenerator::init()
		{
			// Register all types included as part of the Onion library
			set<CubeLightObjectGenerator>("light, cube");

			// Load all objects from file
			LoadFile file("res/data/world/objects.txt");
			while (file.good())
			{
				_StringData line;
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

		Object* ObjectGenerator::generate(std::string id, const _StringData& params)
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

	}
}