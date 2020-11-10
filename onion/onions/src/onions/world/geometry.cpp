#include "../../../include/onions/world/geometry.h"

namespace onion
{
	namespace world
	{

		template <int N>
		bool lambda_all_positive(const matrix<Frac, 1, N>& lambda)
		{
			for (int k = N - 1; k >= 0; --k)
				if (lambda.get(k) < 0)
					return false;
			return true;
		}

		template <int N>
		vec3i lambda_mult(const matrix<Frac, 1, N>& lambda, const Simplex& s)
		{
			// TODO current implementation may produce rounding errors?
			vec3i total(0, 0, 0);
			for (int k = N - 1; k >= 0; --k)
				total += s[k] * lambda.get(k);
			return total;
		}


		void reduce(vec3i& a)
		{
			Int divisor = gcd(abs(a.get(0)), gcd(abs(a.get(1)), abs(a.get(2))));
			a /= divisor;
		}

		
		void nearest_simplex1d(const Simplex& s_input, Simplex& s_output, vec3i& dir)
		{
			// Project the origin onto the line formed by the vertices
			vec3i s_diff = s_input[1] - s_input[0];

			Int numer[2];
			numer[0] = s_diff.dot(s_input[1]);
			numer[1] = s_diff.dot(s_input[0]);
			Int denom = s_diff.square_sum();

			if (numer[0] > 0 && numer[1] < 0)
			{
				// The projected origin lies between the two vertices
				s_output = s_input;
				dir = (s_input[1] * Frac(numer[1], denom)) - (s_input[0] * Frac(numer[0], denom));
			}
			else
			{
				// The projected origin lies outside the line segment formed by the two vertices, so delete a vertex
				s_output = { s_input[0] };
				dir = -1 * s_input[0];
			}
		}

		void nearest_simplex2d(const Simplex& s_input, Simplex& s_output, vec3i& dir)
		{
			// Calculate the two vectors that constitute the basis for the plane
			vec3i d[2] = { s_input[1] - s_input[0], s_input[2] - s_input[0] };

			// TODO come up with a better solution to prevent integer overflow
			reduce(d[0]);
			reduce(d[1]);
			vec3i d0 = d[0];
			vec3i d1 = d[1];

			Int d00 = d[0].square_sum();
			Int d11 = d[1].square_sum();
			Int d01 = d[0].dot(d[1]);

			// Construct the adjugate matrix
			INT_MAT3X2 adj;
			for (int k = 2; k >= 0; --k)
			{
				adj.set(0, k, (d[0].get(k) * d11) - (d[1].get(k) * d01));
				adj.set(1, k, (d[1].get(k) * d00) - (d[0].get(k) * d01));
			}

			// Calculate the normal vector of the plane formed by the vertices
			vec3i normal;
			d[0].cross(d[1], normal);
			reduce(normal);

			// Calculate the projection of the origin onto the plane formed by the simplex
			vec3i o = normal * Frac(normal.dot(s_input[0]), normal.square_sum());

			// Calculate the barycentric coordinates of the origin projection, with lambda_0 = 1
			vec2i numer = adj * (o - s_input[0]);
			Int denom = (d00 * d11) - (d01 * d01); // TODO make sure nonzero!

			FRAC_VEC3 lambda;
			lambda(0) = 1;
			lambda(1) = Frac(numer.get(0), denom);
			lambda(2) = Frac(numer.get(1), denom);

			if (lambda_all_positive(lambda))
			{
				// The projected origin lies within the triangle
				s_output = s_input;
				dir = -1 * o;
			}
			else
			{
				// Delete a vertex
				Int d_min = std::numeric_limits<Int>::max();

				for (int c = 2; c > 0; --c)
				{
					if (lambda.get(c) < 0) // Discard the c-th vertex
					{
						// Construct a simplex with the c-th vertex excluded
						Simplex w = { s_input[0], s_input[c == 1 ? 2 : 1] };

						// Run the sub-routine for a triangle
						Simplex s_output_temp;
						vec3i dir_temp;
						nearest_simplex1d(w, s_output_temp, dir_temp);

						// In case there are multiple negative lambda values, check which has the better minimum norm
						Int d = dir_temp.square_sum();
						if (d < d_min)
						{
							s_output = s_output_temp;
							dir = dir_temp;
							d_min = d;
						}
					}
				}
			}
		}

		void nearest_simplex3d(const Simplex& s_input, Simplex& s_output, vec3i& dir)
		{
			// Calculate the determinant of M = [ (s_0, 1)  (s_1, 1)  (s_2, 1)  (s_3, 1) ]
			vec4i cof; // The cofactors C_{4,c} of M
			Int det = 0; // The determinant of M
			for (int c = 3; c >= 0; --c) // The index of the column being removed from the minor
			{
				// Calculate the c-th cofactor of M
				vec3i a;
				s_input[c > 1 ? 1 : 2].cross(s_input[c > 2 ? 2 : 3], a);
				cof(c) = (c % 2 == 0 ? -1 : 1) * a.dot(s_input[c > 0 ? 0 : 1]);

				// Sum the cofactors to get the determinant of M
				det += cof.get(c);
			}

			// Calculate the potential lambdas
			FRAC_VEC4 lambda;
			for (int c = 3; c >= 0; --c)
				lambda(c) = Frac(cof.get(c), det);

			if (lambda_all_positive(lambda))
			{
				// The origin lies within the tetrahedron
				s_output = s_input;
				dir = vec3i(0, 0, 0);
			}
			else
			{
				// Delete a vertex
				Int d_min = std::numeric_limits<Int>::max();

				for (int c = 3; c > 0; --c)
				{
					if (lambda.get(c) < 0) // Discard the c-th vertex
					{
						// Construct a simplex with the c-th vertex excluded
						Simplex w;
						for (int n = 0; n < 4; ++n)
							if (n != c)
								w.push_back(s_input[n]);

						// Run the sub-routine for a triangle
						Simplex s_output_temp;
						vec3i dir_temp;
						nearest_simplex2d(w, s_output_temp, dir_temp);

						// In case there are multiple negative lambda values, check which has the better minimum norm
						Int d = dir_temp.square_sum();
						if (d < d_min)
						{
							s_output = s_output_temp;
							dir = dir_temp;
							d_min = d;
						}
					}
				}
			}
		}
		
		/// <summary></summary>
		/// <returns>True if the generated simplex contains the origin, false otherwise.</returns>
		bool nearest_simplex(const Simplex& s_input, Simplex& s_output, vec3i& dir)
		{
			switch (s_input.size())
			{
			case 2: // Line
			{
				nearest_simplex1d(s_input, s_output, dir);
				break;
			}
			case 3: // Triangle
			{
				nearest_simplex2d(s_input, s_output, dir);
				break;
			}
			case 4: // Tetrahedron
			{
				nearest_simplex3d(s_input, s_output, dir);
				break;
			}
			default: // Point
			{
				s_output = s_input;
				dir = -1 * s_input[0];
			}
			}

			// TODO i don't know for sure that this works
			return dir.square_sum() == 0;
		}



		/*Int Shape::__get_distance(const Shape* other, Simplex& s, vec3i& d) const
		{
			// TODO figure out a way to ensure there are no infinite loops

			vec3i a;
			Simplex temp;

			do
			{
				a = support(d) - other->support(-1 * d);

				if (a.dot(d) < 0)
				{
					// No intersection between the two shapes
					break;
				}

				// Add the new vertex to the simplex
				temp = s;
				temp.push_back(a);
				
				// Calculate the simplex on the current simplex that is nearest to the origin
				if (nearest_simplex(temp, s, d))
				{
					// The two shapes intersect, so the distance is 0
					return 0;
				}
			} 
			while (s.size() < 4);

			return d.square_sum();
		}*/

		Int Shape::__get_distance(const Shape* other, Simplex& s, vec3i& d) const
		{
			// Calculate the next point on the Minkowski difference
			vec3i d0 = d;
			vec3i a1 = support(d0);
			vec3i a2 = other->support(-1 * d0);
			vec3i a = a1 - a2;

			if (a.dot(d) < 0)
			{
				// No intersection
				return d.square_sum();
			}

			// Add a to the simplex, making sure there are no duplicates
			Simplex temp = { a };
			for (auto iter = s.begin(); iter != s.end(); ++iter)
			{
				if (a == *iter)
				{
					// The algorithm has stalled, so stop running
					return d.square_sum();
				}
				else
				{
					// Add the point to the new simplex
					temp.push_back(*iter);
				}
			}

			// Calculate the simplex on the current simplex that is nearest to the origin
			if (nearest_simplex(temp, s, d))
			{
				// The two shapes intersect
				return 0;
			}
			else if (s.size() < 4)
			{
				// Recurse
				return __get_distance(other, s, d);
			}
			else
			{
				// All points on the simplex are equally close to the origin
				return d.square_sum();
			}
		}

		Int Shape::get_distance(const Shape* other) const
		{
			// Generate an initial guess
			vec3i d = other->get_position() - get_position();
			vec3i a = support(d) - other->support(-1 * d);
			d = -1 * a;
			
			Simplex s = { a };

			// Calculate the distance between the two shapes using GJK algorithm
			return __get_distance(other, s, d);
		}



		Point::Point(const vec3i& pos)
		{
			m_Position = pos;
		}

		vec3i Point::get_position() const
		{
			return m_Position;
		}

		void Point::set_position(const vec3i& pos)
		{
			translate(pos - m_Position);
		}

		void Point::translate(const vec3i& trans)
		{
			m_Position += trans;
		}
		
		vec3i Point::support(const vec3i& dir) const
		{
			return m_Position;
		}
		
		
		OrthogonalPrism::OrthogonalPrism(const vec3i& pos, const vec3i& dimensions) : Point(pos)
		{
			m_Dimensions = dimensions;
		}

		vec3i OrthogonalPrism::support(const vec3i& dir) const
		{
			vec3i res = m_Position;
			for (int k = 2; k >= 0; --k)
				if (dir.get(k) > 0)
					res(k) += m_Dimensions.get(k);
			return res;
		}


		UprightRectangle::UprightRectangle(const vec3i& pos, const vec3i& dimensions) : Point(pos) 
		{
			m_Dimensions = dimensions;
		}

		vec3i UprightRectangle::support(const vec3i& dir) const
		{
			vec3i res = m_Position;

			if ((m_Dimensions.get(0) * dir.get(0)) + (m_Dimensions.get(1) * dir.get(1)) > 0)
			{
				res(0) += m_Dimensions.get(0);
				res(1) += m_Dimensions.get(1);
			}

			if (dir.get(2) > 0)
			{
				res(2) += m_Dimensions.get(2);
			}

			return res;
		}


		Parallelogram::Parallelogram(const vec3i& pos, const vec3i& dir1, const vec3i& dir2) : Point(pos)
		{
			m_Radii[0] = dir1;
			m_Radii[1] = dir2;
		}

		vec3i Parallelogram::support(const vec3i& dir) const
		{
			vec3i res;
			Int d_max = std::numeric_limits<Int>::min();

			for (int c = 3; c >= 0; --c)
			{
				vec3i p = m_Position;
				if (c % 2 > 0)
					p += m_Radii[0];
				if (c / 2 > 0)
					p += m_Radii[1];

				Int d = dir.dot(p);
				if (d > d_max)
				{
					res = p;
					d_max = d;
				}
			}

			return res;
		}

	}
}