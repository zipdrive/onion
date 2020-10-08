#pragma once
#include "../graphics/graphic.h"

namespace onion
{
	namespace world
	{


		// A three-dimensional raycast.
		struct Ray
		{
			// The point of origin for the ray.
			vec3i origin;

			// The direction of the ray.
			vec3i direction;
		};
		
		
		class Bounds
		{
		protected:
			// The base position of the bounds.
			vec3i m_Position;

		public:
			/// <summary>Retrieves the base position of the bounds.</summary>
			/// <returns>A reference to the base position of the bounds.</returns>
			const vec3i& get_position() const;

			/// <summary>Sets the base position of the bounds.</summary>
			/// <param name="pos">The new base position of the bounds.</param>
			virtual void set_position(const vec3i& pos);

			/// <summary>Translates the bounds by a vector.</summary>
			/// <param name="trans">The vector to translate the bounds by.</param>
			void translate(const vec3i& trans);


			/// <summary>Retrieves the first intersection of the bounds with the given raycast.</summary>
			/// <param name="ray">The raycast to retrieve an intersection with.</param>
			/// <param name="intersection">Outputs the first intersection in the bounds with the given ray.</param>
			/// <returns>True if there is an intersection with the ray, false otherwise.</returns>
			virtual bool get_intersection(const Ray& ray, vec3i& intersection) const = 0;
			
			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const = 0;
		};

		// A bounds object modeled by a single point.
		class PointBounds : public Bounds
		{
		public:
			/// <summary>Retrieves the first intersection of the bounds with the given raycast.</summary>
			/// <param name="ray">The raycast to retrieve an intersection with.</param>
			/// <param name="intersection">Outputs the first intersection in the bounds with the given ray.</param>
			/// <returns>True if there is an intersection with the ray, false otherwise.</returns>
			virtual bool get_intersection(const Ray& ray, vec3i& intersection) const;

			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;
		};

		// A bounds object modeled by a sphere, with the position as its center.
		class SphereBounds : public Bounds
		{
		protected:
			// The radius of the sphere, squared.
			int m_RadiusSquared;

		public:
			/// <summary>Retrieves the first intersection of the bounds with the given raycast.</summary>
			/// <param name="ray">The raycast to retrieve an intersection with.</param>
			/// <param name="intersection">Outputs the first intersection in the bounds with the given ray.</param>
			/// <returns>True if there is an intersection with the ray, false otherwise.</returns>
			virtual bool get_intersection(const Ray& ray, vec3i& intersection) const;

			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;
		};

		// A bounds object modeled by an orthogonal rectangular prism, with the position as its minimum point.
		class RectangleBounds : public Bounds
		{
		protected:
			// The dimensions of the rectangular prism.
			vec3i m_Dimensions;

		public:
			RectangleBounds(const vec3i& pos, const vec3i& dimensions);

			/// <summary>Retrieves the first intersection of the bounds with the given raycast.</summary>
			/// <param name="ray">The raycast to retrieve an intersection with.</param>
			/// <param name="intersection">Outputs the first intersection in the bounds with the given ray.</param>
			/// <returns>True if there is an intersection with the ray, false otherwise.</returns>
			virtual bool get_intersection(const Ray& ray, vec3i& intersection) const;

			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;
		};


		// An object in the game world.
		class Object
		{
		protected:
			// The bounds of the object
			Bounds* m_Bounds;

		public:
			/// <summary>Constructs an object.</summary>
			/// <param name="bounds">The bounds of the object. Should be constructed with new specifically for this object.</param>
			Object(Bounds* bounds);

			/// <summary>Virtual deconstructor.</summary>
			virtual ~Object();

			/// <summary>Retrieves the bounds of the object.</summary>
			/// <returns>An object that represents the bounds of the object.</returns>
			const Bounds* get_bounds() const;

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


		
		
		class ObjectManager
		{
		private:
			static Int m_CubeSize;
			
			// Stores all objects within the cube.
			struct Cube
			{
				// The point at the center of the cube.
				vec3i center;

				// All objects that intersect the cube.
				std::vector<Object*> objects;
			};

			std::unordered_map<vec3i, Cube*, std::hash<INT_VEC3>> m_Cubes;




			// The direction that the objects are sorted in, from back to front.
			vec2i m_SortDirection;

			/// <summary>Checks which bounds should be displayed in front of the other.</summary>
			/// <param name="lhs">The bounds on the left-hand side.</param>
			/// <param name="rhs">The bounds on the right-hand side.</param>
			/// <returns>True if the LHS should be displayed in front of the RHS, false otherwise.</returns>
			bool compare(Bounds* lhs, Bounds* rhs);

		public:
			/// <summary>Displays all objects managed.</summary>
			/// <param name="direction">The direction facing towards the camera. Each element is equal to:
			/// +1 if the positive axis is facing towards the camera;
			/// -1 if the negative axis is facing towards the camera;
			/// +0 if the axis is perpendicular to the screen.</param>
			void display(const vec2i& direction) const {}
		};

	}
}