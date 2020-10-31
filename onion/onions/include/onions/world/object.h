#pragma once
#include "../graphics/graphic.h"
#include "graphic.h"
#include "camera.h"

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

			// The graphic used to display the object.
			Graphic3D* m_Graphic;

			/// <summary>Responds to another object colliding with this one.</summary>
			/// <param name="obj">The object that collided with this one.</param>
			/// <returns>True if the object should be pushed back, false otherwise.</returns>
			virtual bool __collision(Object* obj);

		public:
			/// <summary>Constructs an object.</summary>
			/// <param name="bounds">The bounds of the object. Should be constructed with new specifically for this object.</param>
			/// <param name="graphic">The graphic used to display the object. Should be constructed with new specifically for this object.</param>
			Object(Shape* bounds, Graphic3D* graphic = nullptr);

			/// <summary>Virtual deconstructor.</summary>
			virtual ~Object();

			
			/// <summary>Retrieves the bounds of the object.</summary>
			/// <returns>An object that represents the bounds of the object.</returns>
			Shape* get_bounds();
			
			/// <summary>Retrieves the bounds of the object.</summary>
			/// <returns>An object that represents the bounds of the object.</returns>
			const Shape* get_bounds() const;

			
			/// <summary>Checks if an object intersects with this one.</summary>
			/// <param name="obj">The object to check.</param>
			/// <returns>True if the object needs to be pushed back.</returns>
			bool collision(Object* obj);


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




		// A solid object that blocks movement.
		class Wall : public Object
		{
		protected:
			/// <summary>Responds to another object colliding with this one.</summary>
			/// <param name="obj">The object that collided with this one.</param>
			/// <returns>True if the object should be pushed back, false otherwise.</returns>
			bool __collision(Object* obj);

		public:
			/// <summary>Constructs an object.</summary>
			/// <param name="bounds">The bounds of the object. Should be constructed with new specifically for this object.</param>
			/// <param name="graphic">The graphic used to display the object. Should be constructed with new specifically for this object.</param>
			Wall(Shape* bounds, Graphic3D* graphic = nullptr);
		};
		
		// A dimensionless wall aligned with the x-axis, represented graphically by a single fixed sprite.
		class XAlignedWall : public Wall
		{
		public:
			/// <summary>Constructs an x-aligned wall.</summary>
			/// <param name="pos">The position on the wall with the lowest x- and z- coordinates.</param>
			/// <param name="sprite_sheet">The sprite sheet that the wall's sprite is on.</param>
			/// <param name="sprite">The sprite to use for the wall.</param>
			XAlignedWall(const vec3i& pos, const Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite);
		};

		// A dimensionless wall aligned with the y-axis, represented graphically by a single fixed sprite.
		class YAlignedWall : public Wall
		{
		public:
			/// <summary>Constructs an x-aligned wall.</summary>
			/// <param name="pos">The position on the wall with the lowest x- and z- coordinates.</param>
			/// <param name="sprite_sheet">The sprite sheet that the wall's sprite is on.</param>
			/// <param name="sprite">The sprite to use for the wall.</param>
			YAlignedWall(const vec3i& pos, const Flat3DPixelSpriteSheet* sprite_sheet, const Sprite* sprite);
		};


		// A dimensionless wall aligned with either the x or y-axis.
		template <typename T>
		class WallGenerator : public ObjectGenerator
		{
		protected:
			// The sprite sheet that the wall's sprite comes from.
			const Flat3DPixelSpriteSheet* m_SpriteSheet;

			// The sprite used when displaying the wall.
			const Sprite* m_Sprite;

		public:
			/// <summary>Constructs a generator for a wall aligned to the x or y-axis.</summary>
			/// <param name="id">The ID of the generator.</param>
			/// <param name="params">The parameters to pass to the generator.</param>
			WallGenerator(std::string id, const StringData& params) : ObjectGenerator(id)
			{
				m_SpriteSheet = nullptr;
				m_Sprite = nullptr;

				String sprite_sheet;
				if (params.get("sprite_sheet", sprite_sheet))
				{
					if (const _SpriteSheet* ptr = _SpriteSheet::get_sprite_sheet("world/" + sprite_sheet))
					{
						if (m_SpriteSheet = dynamic_cast<const Flat3DPixelSpriteSheet*>(ptr))
						{
							String sprite;
							params.get("sprite", sprite);

							m_Sprite = m_SpriteSheet->get_sprite(sprite);
						}
					}
				}
			}

			/// <summary>Generates an object.</summary>
			/// <returns>An Object created with new.</returns>
			virtual Object* generate(const StringData& params) const
			{
				vec3i pos;
				params.get("pos", pos);
				pos = UNITS_PER_PIXEL * (INT_VEC3)pos;

				return new T(pos, m_SpriteSheet, m_Sprite);
			}
		};

		typedef WallGenerator<XAlignedWall> XAlignedWallGenerator;
		typedef WallGenerator<YAlignedWall> YAlignedWallGenerator;

	}
}