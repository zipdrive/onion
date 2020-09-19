#pragma once
#include "../graphics/graphic.h"

namespace onion
{
	namespace world
	{


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


			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const = 0;
		};

		// A bounds object modeled by a single point.
		class PointBounds : public Bounds
		{
		public:
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

			/// <summary>Displays the object.</summary>
			virtual void display() const;
		};

	}
}