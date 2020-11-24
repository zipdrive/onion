#pragma once
#include <vector>
#include "../matrix.h"

#define ONION_WORLD_GEOMETRY_SCALE		0.01f

namespace onion
{
	namespace world
	{

		typedef std::vector<vec3f> Simplex;


		// A three-dimensional shape.
		class Shape
		{
		protected:
			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			virtual vec3f support(const vec3f& dir) const = 0;

			/// <summary>Calculates the distance from this shape to the given shape, using the GJK algorithm.</summary>
			/// <param name="other">The shape to calculate the distance from.</param>
			/// <param name="s">Inputs the initial guess for s, outputs the last value of s.</param>
			/// <param name="d">Inputs the initial guess for d (as used in the GJK algorithm), outputs the last value of d.</param>
			/// <returns>The distance from this shape to the given shape.</returns>
			Int __get_distance(const Shape* other, Simplex& s, vec3f& d) const;

		public:
			/// <summary>Retrieves the position of the shape.</summary>
			/// <returns>A reference to the position of the shape.</returns>
			virtual vec3i get_position() const = 0;

			/// <summary>Sets the position of the shape.</summary>
			/// <param name="pos">The new position of the shape.</param>
			virtual void set_position(const vec3i& pos) = 0;

			/// <summary>Translates the shape by a given vector.</summary>
			/// <param name="trans">The vector of translation.</param>
			virtual void translate(const vec3i& trans) = 0;


			/// <summary>Calculates the distance from this shape to the given shape.</summary>
			/// <param name="other">The shape to calculate the distance from.</param>
			/// <returns>The distance from this shape to the given shape.</returns>
			virtual Int get_distance(const Shape* other) const;
		};



		// A single point in three-dimensional space.
		class Point : public Shape
		{
		protected:
			// The position of the shape.
			vec3i m_Position;


			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			virtual vec3f support(const vec3f& dir) const;

		public:
			/// <summary>Constructs a single point in three-dimensional space.</summary>
			/// <param name="pos">The point.</param>
			Point(const vec3i& pos);


			/// <summary>Retrieves the position of the shape.</summary>
			/// <returns>A reference to the position of the shape.</returns>
			vec3i get_position() const;

			/// <summary>Sets the position of the shape.</summary>
			/// <param name="pos">The new position of the shape.</param>
			void set_position(const vec3i& pos);

			/// <summary>Translates the shape by a given vector.</summary>
			/// <param name="trans">The vector of translation.</param>
			virtual void translate(const vec3i& trans);
		};

		// A line segment in three-dimensional space.
		class Segment : public Point
		{
		protected:
			// The end point of the line segment.
			vec3i m_EndPoint;


			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			virtual vec3f support(const vec3f& dir) const;

		public:
			/// <summary>Constructs a line segment.</summary>
			/// <param name="origin">The origin of the line segment.</param>
			/// <param name="direction">The direction of the line segment.</param>
			Segment(const vec3i& origin, const vec3i& direction);


			/// <summary>Translates the shape by a given vector.</summary>
			/// <param name="trans">The vector of translation.</param>
			virtual void translate(const vec3i& trans);
		};


		// A rectangular prism whose faces are aligned with the Cartesian planes.
		class OrthogonalPrism : public Point
		{
		protected:
			// The dimensions of the prism.
			vec3i m_Dimensions;


			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			vec3f support(const vec3f& dir) const;

		public:
			/// <summary>Constructs a rectangular prism.</summary>
			/// <param name="pos">The corner of the prism with minimum values.</param>
			/// <param name="dimensions">The dimensions of the prism.</param>
			OrthogonalPrism(const vec3i& pos, const vec3i& dimensions);
		};


		// A rectangle perpendicular to the xy-plane.
		class UprightRectangle : public Point
		{
		protected:
			// The z-coordinate is the height of the rectangle, and the xy-coordinates are the line segment where the rectangle meets the xy-plane.
			vec3i m_Dimensions;


			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			vec3f support(const vec3f& dir) const;

		public:
			/// <summary>Constructs an upright rectangle.</summary>
			/// <param name="pos">The corner of the rectangle with minimum values.</param>
			/// <param name="dimensions">The z-coordinate is the height of the rectangle, and the xy-coordinates are the line segment where the rectangle meets the xy-plane.</param>
			UprightRectangle(const vec3i& pos, const vec3i& dimensions);
		};


		class Parallelogram : public Point
		{
		protected:
			// The radial vectors of the parallelogram.
			vec3i m_Radii[2];


			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			vec3f support(const vec3f& dir) const;

		public:
			/// <summary>Constructs a parallelogram.</summary>
			/// <param name="pos">The corner of the rectangle with minimum values.</param>
			/// <param name="dimensions">The z-coordinate is the height of the rectangle, and the xy-coordinates are the line segment where the rectangle meets the xy-plane.</param>
			Parallelogram(const vec3i& pos, const vec3i& dir1, const vec3i& dir2);
		};




		// A wrapper for a shape that handles subpixel translations.
		struct SubpixelHandler
		{
		protected:
			// The shape being handled.
			Shape* m_Shape;

			// The current subpixel position within the current pixel.
			vec3i m_Subpixels;

		public:
			// The number of subpixels contained in a single pixel.
			static constexpr Int num_subpixels = 256;


			/// <summary>Constructs a wrapper for a shape that handles subpixel translations.</summary>
			/// <param name="shape">The shape to handle subpixel translations for.</param>
			SubpixelHandler(Shape* shape);

			/// <summary>Translates the shape by a given vector, in subpixel units.</summary>
			/// <param name="trans">A vector of translation, in subpixel units.</param>
			void translate(const vec3i& trans);
		};

	}
}