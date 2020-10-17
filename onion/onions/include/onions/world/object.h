#pragma once
#include "../graphics/graphic.h"
#include "geometry.h"

namespace onion
{
	namespace world
	{
		
		// An object in the game world.
		class Object
		{
		protected:
			// The bounds of the object
			Shape* m_Bounds;

		public:
			/// <summary>Constructs an object.</summary>
			/// <param name="bounds">The bounds of the object. Should be constructed with new specifically for this object.</param>
			Object(Shape* bounds);

			/// <summary>Virtual deconstructor.</summary>
			virtual ~Object();

			
			/// <summary>Retrieves the bounds of the object.</summary>
			/// <returns>An object that represents the bounds of the object.</returns>
			const Shape* get_bounds() const;


			/// <summary>Displays the object.</summary>
			/// <param name="center">The ray from the camera position towards the camera.</param>
			virtual void display(const Ray& center) const;
		};



		class ObjectGenerator
		{
		private:
			// All generators.
			static std::unordered_map<std::string, ObjectGenerator*> m_Generators;

			struct Generator
			{
				/// <summary>Virtual deconstructor.</summary>
				virtual ~Generator() = default;

				/// <summary>Finds the type associated with the given string and constructs an instance of it.</summary>
				/// <param name="id">The ID of the object generator.</param>
				/// <param name="params">The parameters to pass to the constructor.</param>
				virtual ObjectGenerator* generate(std::string id, const StringData& params) = 0;
			};

			template <typename _Generator>
			struct TypedGenerator : public Generator
			{
				/// <summary>Finds the type associated with the given string and constructs an instance of it.</summary>
				/// <param name="id">The ID of the object generator.</param>
				/// <param name="params">The parameters to pass to the constructor.</param>
				ObjectGenerator* generate(std::string id, const StringData& params)
				{
					return new _Generator(id, params);
				}
			};

			// Map from a type name to a generator for an object generator.
			static std::unordered_map<std::string, Generator*> m_GeneratorFinder;

		protected:
			/// <summary>Constructs a generator with the given ID.</summary>
			/// <param name="id">The ID of the generator.</param>
			ObjectGenerator(std::string id);

		public:
			/// <summary>Sets the type associated with a given string. Does nothing if the string is already associated with a type.</summary>
			/// <param name="type">The string to associate with the template parameter.</param>
			template <typename _Generator>
			static void set(std::string type)
			{
				auto iter = m_GeneratorFinder.find(type);
				if (iter == m_GeneratorFinder.end())
				{
					m_GeneratorFinder.emplace(type, new TypedGenerator<_Generator>());
				}
			}

			/// <summary>Loads all objects from file.</summary>
			static void init();

			/// <summary>Uses the generator with the given ID to generate an object.</summary>
			/// <param name="id">The ID of the generator to use.</param>
			/// <param name="params">The parameters to pass to the generator.</param>
			/// <returns>An Object created with new by the generator with the given ID.</returns>
			static Object* generate(std::string id, const StringData& params);

			/// <summary>Generates an object.</summary>
			/// <returns>An Object created with new.</returns>
			virtual Object* generate(const StringData& params) const = 0;
		};

	}
}