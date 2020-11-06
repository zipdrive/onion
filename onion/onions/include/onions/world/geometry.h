#pragma once
#include <vector>
#include "../matrix.h"

namespace onion
{
	namespace world
	{

		// An infinite line in three-dimensional space.
		struct Line
		{
			// The point of origin for the ray.
			vec3i origin;

			// The direction of the ray.
			vec3i direction;


			/// <summary>Retrieves the point on the line with the given coefficient for the direction vector.</summary>
			/// <param name="t">The coefficient of the direction vector.</param>
			/// <param name="point">Outputs the point along the line, where the direction vector has the given coefficient.</param>
			virtual void get(Int t, vec3i& point) const;

			/// <summary>Retrieves the point on the line with the given coefficient for the direction vector.</summary>
			/// <param name="t">The coefficient of the direction vector.</param>
			/// <param name="point">Outputs the point along the line, where the direction vector has the given coefficient.</param>
			virtual void get(const Frac& t, vec3i& point) const;


			/// <summary>Calculates the closest point on the ray to the given point.</summary>
			/// <param name="point">The given point.</param>
			/// <param name="closest">Outputs the closest point on the ray to the given point.</param>
			void get_closest_point(const vec3i& point, vec3i& closest) const;
		};

		// A raycast in three-dimensional space, whose range is 0 <= t < INFINITY.
		struct Ray : public Line
		{
			/// <summary>Retrieves the point on the line with the given coefficient for the direction vector.</summary>
			/// <param name="t">The coefficient of the direction vector.</param>
			/// <param name="point">Outputs the point along the line, where the direction vector has the given coefficient.</param>
			void get(Int t, vec3i& point) const;

			/// <summary>Retrieves the point on the line with the given coefficient for the direction vector.</summary>
			/// <param name="t">The coefficient of the direction vector.</param>
			/// <param name="point">Outputs the point along the line, where the direction vector has the given coefficient.</param>
			void get(const Frac& t, vec3i& point) const;
		};

		// A line segment in three-dimensional space, whose range is 0 <= t <= 1.
		struct Segment : public Line
		{
			/// <summary>Retrieves the point on the line with the given coefficient for the direction vector.</summary>
			/// <param name="t">The coefficient of the direction vector.</param>
			/// <param name="point">Outputs the point along the line, where the direction vector has the given coefficient.</param>
			void get(Int t, vec3i& point) const;

			/// <summary>Retrieves the point on the line with the given coefficient for the direction vector.</summary>
			/// <param name="t">The coefficient of the direction vector.</param>
			/// <param name="point">Outputs the point along the line, where the direction vector has the given coefficient.</param>
			void get(const Frac& t, vec3i& point) const;
		};



		// A two-dimensional plane in three-dimensional space.
		struct Plane
		{
			// The normal vector from the plane.
			vec3i normal;

			// The dot product of any point on the plane with the normal vector.
			Int dot;

			
			/// <summary>Calculates the intersection between two planes.</summary>
			/// <param name="other">The other plane.</param>
			/// <param name="intersection">The line that is the intersection between the two planes, if the planes intersect.</param>
			/// <returns>False if the two planes are parallel, true otherwise.</returns>
			bool get_intersection(const Plane& other, Line& intersection) const;

			/// <summary>Calculates the intersection of this plane with a line.</summary>
			/// <param name="line">The line to test for intersection with.</param>
			/// <param name="intersection">Outputs the intersection of the line with the plane. The direction of this line will be (0,0,0) if the intersection is a point.</param>
			/// <returns>True if the line intersects the plane, false otherwise.</returns>
			bool get_intersection(const Line& line, Line& intersection) const;


			/// <summary>Calculates the closest point on the plane to the given point.</summary>
			/// <param name="point">The given point.</param>
			/// <param name="closest">Outputs the closest point on the plane to the given point.</param>
			void get_closest_point(const vec3i& point, vec3i& closest) const;
		};

		// A two-dimensional parallelogram in three-dimensional space.
		struct Parallelogram
		{
			// The position of one corner of the parallelogram.
			vec3i pos;

			// The vectors indicating the length of each side of the parallelogram.
			vec3i sides[2];


			/// <summary>Clamps the point to the bounds of the parallelogram.</summary>
			/// <param name="point">The point to clamp.</param>
			void clamp(vec3i& point) const;

			
			/// <summary>Calculates the closest point on the parallelogram to the given point.</summary>
			/// <param name="point">The given point.</param>
			/// <param name="closest">Outputs the closest point on the parallelogram to the given point.</param>
			void get_closest_point(const vec3i& point, vec3i& closest) const;

			/// <summary>Calculates the closest point on the parallelogram to the given line.</summary>
			/// <param name="line">The given line.</param>
			/// <param name="closest">Outputs the closest point on the parallelogram to the given line.</param>
			void get_closest_point(const Line& line, vec3i& closest) const;

			/// <summary>Calculates the closest point on the parallelogram to the given parallelogram.</summary>
			/// <param name="quad">The given parallelogram.</param>
			/// <param name="closest">Outputs the closest point on the parallelogram to the given parallelogram.</param>
			void get_closest_point(const Parallelogram& quad, vec3i& closest) const;
		};




		/// <summary>Row-reduces the bottom-right corner of a matrix and vector.</summary>
		/// <param name="A">The matrix to row-reduce.</param>
		/// <param name="B">The vector to row-reduce.</param>
		/// <param name="r">The row to row-reduce below.</param>
		/// <param name="c">The column to row-reduce to the right of.</param>
		template <int _Columns, int _Rows>
		bool __reduce(matrix<Int, _Columns, _Rows>& A, matrix<Int, 1, _Rows>& B, int r, int c)
		{
			if (r < _Rows && c < _Columns)
			{
				if (A.get(r, c) == 0)
				{
					// Swap with the first row below this one that has a nonzero element in the c-th column
					for (int i = r + 1; r < _Rows; ++r)
					{
						if (A.get(i, c) != 0)
						{
							Int temp = B.get(i);
							B(i) = B.get(r);
							B(r) = temp;

							for (int j = c; j < _Columns; ++j)
							{
								temp = A.get(i, j);
								A(i, j) = A.get(r, j);
								A(r, j) = temp;
							}

							break;
						}
					}
				}

				int r1;
				if (A.get(r, c) == 0)
				{
					// If there were no rows below this one with a nonzero element in the c-th column, move on to the next column
					r1 = r;
				}
				else
				{
					// Subtract a multiple of this row from all rows below it, so that no row below it has a nonzero element in the c-th column
					for (int i = r + 1; i < _Rows; ++i)
					{
						if (Int cr = A.get(i, c))
						{
							Int ci = A.get(r, c);

							Int divisor = gcd(ci, cr);
							if (divisor > 1)
							{
								ci /= divisor;
								cr /= divisor;
							}

							B(i) = (B.get(i) * ci) - (B.get(r) * cr);

							for (int j = c; j < _Columns; ++j)
							{
								A(i, j) = (A.get(i, j) * ci) - (A.get(r, j) * cr);
							}
						}
					}

					// Recursively reduce the remainder of the matrix
					r1 = r + 1;
				}

				int c1 = c + 1;
				if (__reduce(A, B, r1, c1))
				{
					// Subtract the reduced row from the above rows, so that no row above it has a nonzero element in the c1-th column
					for (int i = 0; i < r1; ++i)
					{
						if (Int cr = A.get(i, c1))
						{
							Int ci = A.get(r1, c1);

							Int divisor = gcd(ci, cr);
							if (divisor > 1)
							{
								ci /= divisor;
								cr /= divisor;
							}

							B(i) = (B.get(i) * ci) - (B.get(r1) * cr);

							for (int j = c1; j < _Columns; ++j)
							{
								A(i, j) = (A.get(i, j) * ci) - (A.get(r1, j) * cr);
							}
						}
					}
				}

				return true;
			}

			return false;
		}
		
		template <int _Columns, int _Rows>
		bool solve(const matrix<Int, _Columns, _Rows>& A, const matrix<Int, 1, _Rows>& B, matrix<Frac, 1, _Columns>& x0, std::vector<matrix<Int, 1, _Columns>>& x)
		{
			// Reduce the given matrices
			matrix<Int, _Columns, _Rows> Ar = A;
			matrix<Int, 1, _Rows> Br = B;

			__reduce(Ar, Br, 0, 0);

			// Solve for each element of the hyperplane x
			x.clear();
			int freevar[_Columns];
			for (int j = _Columns - 1; j >= 0; --j)
				freevar[j] = -1;

			for (int i = 0; i < _Rows; ++i)
			{
				int j_first = _Columns, j_last = _Columns;
				for (int j = 0; j < _Columns; ++j)
				{
					if (Ar.get(i, j) != 0)
					{
						// j is a constant or a dependant variable
						if (j_first > j)
						{
							j_first = j;
						}
						
						// j hasn't already been declared a free variable
						else if (freevar[j] < 0)
						{
							freevar[j] = x.size();
							x.emplace_back();

							Int divisor = gcd(abs(Ar.get(i, j)), abs(Ar.get(i, j_first)));
							x.back()(j_first) = -Ar.get(i, j) / divisor;
							x.back()(j) = Ar.get(i, j_first) / divisor;
						}

						// j has already been declared a free variable
						else
						{
							Int divisor = gcd(abs(Ar.get(i, j)), abs(Ar.get(i, j_first)));
							x[freevar[j]] *= Ar.get(i, j_first) / divisor;
							x[freevar[j]](j_first) = Ar.get(i, j) / divisor;
						}

						j_last = j;
					}
				}

				// There is no nonzero coefficient in this row
				if (j_first == _Columns)
				{
					if (B.get(i) != 0)
						return false; // No solution
				}

				// j_first is a constant or dependant variable
				else 
				{
					x0(j_first) = Frac(B.get(i), A.get(i, j_first));
				}
			}

			// There is a solution
			return true;
		}


		/// <summary>Solves the equation (t1 * a1) + (t2 * a2) = b.</summary>
		/// <returns>True if there is a solution, false otherwise.</returns>
		bool solve(const vec3i& a1, const vec3i& a2, const vec3i& b, Frac& t1, Frac& t2);


		

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


			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const = 0;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D line.</summary>
			/// <param name="pos">The line to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given line.</param>
			virtual void get_closest_point(const Line& ray, vec3i& closest) const = 0;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D parallelogram.</summary>
			/// <param name="quad">The parallelogram to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given parallelogram.</param>
			virtual void get_closest_point(const Parallelogram& quad, vec3i& closest) const = 0;


			/// <summary>Calculates the minimum distance (squared) between the two shapes.</summary>
			/// <param name="shape">The shape to calculate the distance from.</param>
			/// <returns>The minimum distance (squared) to the other shape.</returns>
			virtual Int get_distance(const Shape* other) const = 0;
		};

		// A shape modeled by a single point in three-dimensional space.
		class Point : public Shape
		{
		public:
			/// <summary>Constructs a point in three-dimensional space.</summary>
			/// <param name="pos">The point.</param>
			Point(const vec3i& pos);


			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D line.</summary>
			/// <param name="line">The line to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given line.</param>
			virtual void get_closest_point(const Line& line, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D parallelogram.</summary>
			/// <param name="quad">The parallelogram to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given parallelogram.</param>
			virtual void get_closest_point(const Parallelogram& quad, vec3i& closest) const;


			/// <summary>Calculates the minimum distance (squared) between the two shapes.</summary>
			/// <param name="shape">The shape to calculate the distance from.</param>
			/// <returns>The minimum distance (squared) to the other shape.</returns>
			virtual Int get_distance(const Shape* shape) const;
		};

		// A shape modeled by a sphere, with the position as its center.
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


			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D line.</summary>
			/// <param name="line">The line to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given line.</param>
			virtual void get_closest_point(const Line& line, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D parallelogram.</summary>
			/// <param name="quad">The parallelogram to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given parallelogram.</param>
			virtual void get_closest_point(const Parallelogram& quad, vec3i& closest) const;


			/// <summary>Calculates the minimum distance (squared) between the two shapes.</summary>
			/// <param name="shape">The shape to calculate the distance from.</param>
			/// <returns>The minimum distance (squared) to the other shape.</returns>
			virtual Int get_distance(const Shape* shape) const;
		};



		// A shape modeled by an axis-aligned rectangular prism, with the position as its minimum point.
		class RectangularPrism : public Shape
		{
		protected:
			// The dimensions of the rectangular prism.
			vec3i m_Dimensions;

			/// <summary>Clamps the point to the bounds of the prism.</summary>
			/// <param name="point">The point to clamp.</param>
			void clamp(vec3i& point) const;

		public:
			/// <summary>Constructs an orthogonal rectangular prism.</summary>
			/// <param name="pos">The position of the corner with the lowest values.</param>
			/// <param name="dimensions">The dimensions of the rectangular prism.</param>
			RectangularPrism(const vec3i& pos, const vec3i& dimensions);


			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D line.</summary>
			/// <param name="line">The line to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given line.</param>
			virtual void get_closest_point(const Line& line, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D parallelogram.</summary>
			/// <param name="quad">The parallelogram to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given parallelogram.</param>
			virtual void get_closest_point(const Parallelogram& quad, vec3i& closest) const;


			/// <summary>Calculates the minimum distance (squared) between the two shapes.</summary>
			/// <param name="shape">The shape to calculate the distance from.</param>
			/// <returns>The minimum distance (squared) to the other shape.</returns>
			virtual Int get_distance(const Shape* shape) const;
		};

		// A shape modeled by an upright two-dimensional rectangle in three-dimensional space.
		class Rectangle : public RectangularPrism
		{
		public:
			/// <summary>Constructs an orthogonal rectangular prism.</summary>
			/// <param name="pos">The position of the corner with the lowest values.</param>
			/// <param name="dimensions">The dimensions of the rectangular prism.</param>
			Rectangle(const vec3i& pos, const vec3i& dimensions);


			/// <summary>Retrieves the closest point within the sphere to an arbitrary 3D point.</summary>
			/// <param name="pos">The position to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given position.</param>
			virtual void get_closest_point(const vec3i& pos, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D line.</summary>
			/// <param name="line">The line to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given line.</param>
			virtual void get_closest_point(const Line& line, vec3i& closest) const;

			/// <summary>Retrieves the closest point within the bounds to an arbitrary 3D parallelogram.</summary>
			/// <param name="quad">The parallelogram to retrieve the closest point to.</param>
			/// <param name="closest">Outputs the closest point in the bounds to the given parallelogram.</param>
			virtual void get_closest_point(const Parallelogram& quad, vec3i& closest) const;


			/// <summary>Calculates the minimum distance (squared) between the two shapes.</summary>
			/// <param name="shape">The shape to calculate the distance from.</param>
			/// <returns>The minimum distance (squared) to the other shape.</returns>
			virtual Int get_distance(const Shape* shape) const;
		};

	}
}