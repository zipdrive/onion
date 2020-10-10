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
			/// <param name="direction">The direction facing towards the camera. Each element is equal to:
			/// +1 if the positive axis is facing towards the camera;
			/// -1 if the negative axis is facing towards the camera;
			/// +0 if the axis is perpendicular to the screen.</param>
			virtual void display(const vec2i& direction) const;
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
				virtual ObjectGenerator* generate(std::string id, const _StringData& params) = 0;
			};

			template <typename _Generator>
			struct TypedGenerator : public Generator
			{
				/// <summary>Finds the type associated with the given string and constructs an instance of it.</summary>
				/// <param name="id">The ID of the object generator.</param>
				/// <param name="params">The parameters to pass to the constructor.</param>
				ObjectGenerator* generate(std::string id, const _StringData& params)
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
			/// <summary>Sets the type associated with a given string.</summary>
			/// <param name="type">The string to associate with the template parameter.</param>
			template <typename _Generator>
			static void set(std::string type)
			{
				m_GeneratorFinder.emplace(type, new TypedGenerator<_Generator>());
			}

			/// <summary>Loads all objects from file.</summary>
			static void init();

			/// <summary>Uses the generator with the given ID to generate an object.</summary>
			/// <param name="id">The ID of the generator to use.</param>
			/// <param name="params">The parameters to pass to the generator.</param>
			/// <returns>An Object created with new by the generator with the given ID.</returns>
			static Object* generate(std::string id, const _StringData& params);

			/// <summary>Generates an object.</summary>
			/// <returns>An Object created with new.</returns>
			virtual Object* generate(const _StringData& params) const = 0;
		};

	}
}