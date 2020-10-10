#pragma once
#include "../matrix.h"

namespace onion
{
	namespace world
	{

		// A two-dimensional plane in three-dimensional space.
		struct Plane
		{
			// The normal vector from the plane.
			vec3i normal;

			// The dot product of any point on the plane with the normal vector.
			Int dot;
		};


		// A raycast in three-dimensional space.
		struct Ray
		{
			// The point of origin for the ray.
			vec3i origin;

			// The direction of the ray.
			vec3i direction;
		};




		// A three-dimensional shape.
		class Shape
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

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D raycast.</summary>
			/// <param name="pos">The raycast to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given raycast.</param>
			virtual void get_closest_point(const Ray& ray, vec3i& closest) const = 0;
		};

		// A single point in three-dimensional space.
		class Point : public Shape
		{
		public:
			/// <summary>Constructs a point in three-dimensional space.</summary>
			/// <param name="pos">The point.</param>
			Point(const vec3i& pos);

			/// <summary>Retrieves the first intersection of the bounds with the given raycast.</summary>
			/// <param name="ray">The raycast to retrieve an intersection with.</param>
			/// <param name="intersection">Outputs the first intersection in the bounds with the given ray.</param>
			/// <returns>True if there is an intersection with the ray, false otherwise.</returns>
			virtual bool get_intersection(const Ray& ray, vec3i& intersection) const;

			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D raycast.</summary>
			/// <param name="pos">The raycast to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given raycast.</param>
			virtual void get_closest_point(const Ray& ray, vec3i& closest) const;
		};

		// A bounds object modeled by a sphere, with the position as its center.
		class Sphere : public Shape
		{
		protected:
			// The radius of the sphere, squared.
			Int m_RadiusSquared;

		public:
			/// <summary>Constructs a spherical shape.</summary>
			/// <param name="pos">The center of the sphere.</param>
			/// <param name="radius">The radius of the sphere.</param>
			Sphere(const vec3i& pos, Int radius);

			/// <summary>Retrieves the first intersection of the bounds with the given raycast.</summary>
			/// <param name="ray">The raycast to retrieve an intersection with.</param>
			/// <param name="intersection">Outputs the first intersection in the bounds with the given ray.</param>
			/// <returns>True if there is an intersection with the ray, false otherwise.</returns>
			virtual bool get_intersection(const Ray& ray, vec3i& intersection) const;

			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D raycast.</summary>
			/// <param name="pos">The raycast to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given raycast.</param>
			virtual void get_closest_point(const Ray& ray, vec3i& closest) const;
		};

		// A bounds object modeled by an axis-aligned rectangular prism, with the position as its minimum point.
		class RectangularPrism : public Shape
		{
		protected:
			// The dimensions of the rectangular prism.
			vec3i m_Dimensions;

		public:
			/// <summary>Constructs an orthogonal rectangular prism.</summary>
			/// <param name="pos">The position of the corner with the lowest values.</param>
			/// <param name="dimensions">The dimensions of the rectangular prism.</param>
			RectangularPrism(const vec3i& pos, const vec3i& dimensions);

			/// <summary>Retrieves the first intersection of the bounds with the given raycast.</summary>
			/// <param name="ray">The raycast to retrieve an intersection with.</param>
			/// <param name="intersection">Outputs the first intersection in the bounds with the given ray.</param>
			/// <returns>True if there is an intersection with the ray, false otherwise.</returns>
			virtual bool get_intersection(const Ray& ray, vec3i& intersection) const;

			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D raycast.</summary>
			/// <param name="pos">The raycast to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given raycast.</param>
			virtual void get_closest_point(const Ray& ray, vec3i& closest) const;
		};

	}
}