#include "../../../include/onions/world/geometry.h"

namespace onion
{
	namespace world
	{

		/// <summary>Implements the extended Euclidean algorithm to find the greatest common divisor of two integers, as well as integers A, B such that Aa + Bb = gcd(a, b).</summary>
		/// <param name="r0">The first integer.</param>
		/// <param name="r1">The second integer.</param>
		/// <param name="s0">The first integer in the recursive sequence to calculate the coefficient of a.</param>
		/// <param name="s1">The second integer in the recursive sequence to calculate the coefficient of a.</param>
		/// <param name="sfinal">Outputs the coefficient of a.</param>
		/// <param name="t0">The first integer in the recursive sequence to calculate the coefficient of b.</param>
		/// <param name="t1">The second integer in the recursive sequence to calculate the coefficient of b.</param>
		/// <param name="tfinal">Outputs the coefficient of b.</param>
		/// <returns>The greatest common divisor of both integers.</returns>
		template <typename T>
		T __gcd(T r0, T r1, T s0, T s1, T& sfinal, T t0, T t1, T& tfinal)
		{
			T q1 = r0 / r1;
			T r2 = r0 - (q1 * r1);

			if (r2 == 0)
			{
				sfinal = s1;
				tfinal = t1;
				return r1;
			}
			else
			{
				T s2 = s0 - (q1 * s1);
				T t2 = t0 - (q1 * t2);

				return __gcd(r1, r2, s1, s2, sfinal, t1, t2, tfinal);
			}
		}

		/// <summary>Implements the extended Euclidean algorithm to find the greatest common divisor of two integers, as well as integers A, B such that Aa + Bb = gcd(a, b).</summary>
		/// <param name="a">The first integer.</param>
		/// <param name="b">The second integer.</param>
		/// <param name="A">Outputs the coefficient of a.</param>
		/// <param name="B">Outputs the coefficient of b.</param>
		/// <returns>The greatest common divisor of both integers.</returns>
		template <typename T>
		T gcd(T a, T b, T& A, T& B)
		{
			if (a < b)
				return gcd(b, a, B, A);
			else
			{
				if (b == 0)
				{
					A = 1;
					B = 0;
					return a;
				}
				else
				{
					return __gcd(a, b, 1, 0, A, 0, 1, B);
				}
			}
		}


		/// <summary>Calculates x to minimize a - bx % n.</summary>
		/// <returns>The value of x that minimizes a - bx % n.</returns>
		template <typename T>
		T minimize_modulo_difference(T a, T b, T n)
		{
			T a_mod = a % n;
			if (a_mod == 0)
				return 0;
			else if (a_mod < 0)
				a_mod += abs(n);

			T b_mod = b % n;
			if (b_mod < 0)
				b_mod += n;

			T B, N;
			T divisor = gcd(n, b_mod, N, B);

			do
			{
				if (a_mod % divisor == 0)
				{
					// There is a perfect solution to make ax == b mod n
					return (B * a_mod) / divisor;
				}
			} while (--a_mod >= 0);
		}

		/// <summary>Calculates x to minimize both (a1 - (b1 * x) % n1) and (a2 - (b2 * x) % n2).</summary>
		/// <returns>The value of x that minimizes both (a1 - (b1 * x) % n1) and (a2 - (b2 * x) % n2).</returns>
		template <typename T>
		T minimize_double_modulo_difference(T a1, T b1, T n1, T a2, T b2, T n2)
		{
			T n[2] = { abs(n1), abs(n2) };

			T a[2] = { a1 % n[0], a2 % n[1] };
			if (a[0] == 0 && a[1] == 0)
				return 0;

			T b[2] = { b1 % n1, b2 % n2 };
			T divisors[2], B[2], N[2];
			for (int k = 0; k < 2; k++)
			{
				if (a[k] < 0)
					a[k] += n[k];
				if (b[k] < 0)
					b[k] += n[k];

				divisors[k] = gcd(n[k], b[k], N[k], B[k]);
				while (a[k] % divisors[k] != 0)
					--a[k];
			}

			T r = (divisors[0] * B[1] * a[1]) - (divisors[1] * B[0] * a[0]);
			T s[2] = { -divisors[1] * n[0], divisors[0] * n[1] };
			T x = minimize_modulo_difference(r, s[0], s[1]);

			return ((B[0] * a[0]) + (x * n[0])) / divisors[0];
		}



		template <int N>
		bool lambda_all_positive(const matrix<Frac, 1, N>& lambda)
		{
			for (int k = N - 1; k >= 0; --k)
				if (lambda.get(k) <= 0)
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

		
		void nearest_simplex1d(const Simplex& s_input, Simplex& s_output, vec3i& dir)
		{
			// Project the origin onto the line formed by the vertices
			vec3i t = s_input[1] - s_input[0];
			vec3i o = s_input[1] + (t * Frac(t.dot(s_input[1]), t.square_sum()));

			// Project the vertices and the projected origin onto R^1
			Int mu_max = 0;
			int index = -1;
			for (int k = 2; k >= 0; --k)
			{
				Int mu = t.get(k);
				if (abs(mu) > abs(mu_max))
				{
					mu_max = -mu;
					index = k;
				}
			}

			// Calculate the potential lambdas
			FRAC_VEC2 lambda;
			for (int m = 1; m >= 0; --m)
				lambda(m) = Frac((m == 0 ? 1 : -1) * (s_input[m].get(index) - o.get(index)), mu_max);

			if (lambda_all_positive(lambda))
			{
				// The projected origin lies between the two vertices
				s_output = s_input;
				dir = -1 * lambda_mult(lambda, s_output);
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
			// Project the origin onto the plane formed by the vertices
			vec3i normal;
			vec3i(s_input[1] - s_input[0]).cross(s_input[2] - s_input[0], normal);
			vec3i o = normal * Frac(normal.dot(s_input[0]), normal.square_sum());

			// Project the triangle and the origin onto R^2 by selecting the Cartesian plane such that the projected triangle has the greatest area
			vec3i cof[3]; // Vectors whose k-th elements are the cofactors C_{3,c} of M (see below) for the projection onto the Cartesian plane that excludes the k-th dimension
			vec3i mu;
			for (int c = 2; c >= 0; --c)
			{
				s_input[(c + 1) % 3].cross(s_input[(c + 2) % 3], cof[c]);

				// Sum the cross products from each pair of vertices to obtain a vector whose k-th element is the area of the triangle projected onto the Cartesian plane that excludes the k-th dimension
				mu += cof[c];
			}
			
			Int mu_max = 0;
			int index = -1;
			for (int k = 2; k >= 0; --k)
			{
				// Calculate which Cartesian plane has the largest projected area for the triangle
				if (abs(mu.get(k)) > abs(mu_max))
				{
					mu_max = mu.get(k);
					index = k;
				}
			}

			// Calculate the determinant of M = [ (s_0^proj, 1)  (s_1^proj, 1)  (s_2^proj, 1) ]
			Int det = 0; // The determinant of M
			for (int c = 2; c >= 0; --c) // The index of the column being removed from the minor
			{
				// Sum the cofactors to get the determinant of M
				det += cof[c].get(index);
			}

			// Calculate the potential lambdas that serve as the solution to M * lambda = [ o_{k1}  o_{k2}  1 ]
			FRAC_VEC3 lambda;
			int k1 = (index + 1) % 3;
			int k2 = (index + 2) % 3;
			for (int c = 3; c >= 0; --c)
			{
				int c1 = (c + 1) % 3;
				int c2 = (c + 2) % 3;
				lambda(c) = Frac(
					(o.get(k1) * (s_input[c1].get(k2) - s_input[c2].get(k2)))
						+ (o.get(k2) * (s_input[c2].get(k1) - s_input[c1].get(k1)))
						+ cof[c].get(index),
					det
				);
			}

			if (lambda_all_positive(lambda))
			{
				// The origin lies within the triangle
				s_output = s_input;
				dir = -1 * lambda_mult(lambda, s_output);
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
						Simplex w;
						for (int n = 0; n < 3; ++n)
							if (n != c)
								w.push_back(s_input[n]);

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
				dir = -1 * lambda_mult(lambda, s_output);
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



		Int Shape::__get_distance(const Shape* other, Simplex& s, vec3i& d) const
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