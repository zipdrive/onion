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



		bool solve(const vec3i& a1, const vec3i& a2, const vec3i& b, Frac& t1, Frac& t2)
		{
			INT_MAT2X3 A;
			for (int r = 2; r >= 0; --r)
			{
				A.set(r, 0, a1.get(r));
				A.set(r, 1, a2.get(r));
			}

			FRAC_VEC2 x0;
			std::vector<INT_VEC2> x;

			if (solve(A, b, x0, x))
			{
				t1 = x0.get(0);
				t2 = x0.get(1);

				return true;
			}
			else
			{
				return false;
			}
		}



		bool Plane::get_intersection(const Plane& other, Line& intersection) const
		{
			// TODO replace with solve function for readability?

			if (normal.get(0) == 0)
			{
				if (normal.get(1) == 0)
				{
					if (normal.get(2) == 0)
						return false;

					if (other.normal.get(0) == 0)
					{
						if (other.normal.get(1) == 0)
						{
							return false;
						}
						else
						{
							intersection.origin(0) = 0;
							intersection.origin(1) = other.dot / other.normal.get(1);
						}
					}
					else
					{
						Int t = minimize_modulo_difference(other.dot, other.normal.get(1), other.normal.get(0));

						intersection.origin(0) = (other.dot - (t * other.normal.get(1))) / other.normal.get(0);
						intersection.origin(1) = t;
					}

					intersection.origin(2) = dot / normal.get(2);
				}
				else
				{
					Int a = (other.normal.get(0) * normal.get(1)) - (other.normal.get(1) * normal.get(0));
					Int c = (other.normal.get(2) * normal.get(1)) - (other.normal.get(1) * normal.get(2));
					Int d = (other.dot * normal.get(1)) - (other.normal.get(1) * dot);

					if (a == 0)
					{
						if (c == 0)
						{
							return false;
						}
						else
						{
							intersection.origin(0) = 0;
							intersection.origin(1) = (dot - d) / normal.get(1);
							intersection.origin(2) = d / c;
						}
					}
					else
					{
						Int t = minimize_double_modulo_difference(d, c, a, dot, normal.get(2), normal.get(1));

						intersection.origin(0) = (d - (t * c)) / a;
						intersection.origin(1) = (dot - (t * normal.get(2))) / normal.get(1);
						intersection.origin(2) = t;
					}
				}
			}
			else if (Int b = (other.normal.get(1) * normal.get(0)) - (other.normal.get(0) * normal.get(1)))
			{
				Int c = (other.normal.get(2) * normal.get(0)) - (other.normal.get(0) * normal.get(2));
				Int d = (other.dot * normal.get(0)) - (other.normal.get(0) * dot);

				Int mod = normal.get(0) * b;
				Int lhs = (dot * b) - (normal.get(1) * d);
				Int rhs = (normal.get(2) * b) - (normal.get(1) * c);

				Int t = minimize_modulo_difference(lhs, rhs, mod);

				intersection.origin(0) = (lhs - (t * rhs)) / mod;
				intersection.origin(1) = (d - (t * c)) / b;
				intersection.origin(2) = t;
			}
			else if (Int c = (other.normal.get(2) * normal.get(0)) - (other.normal.get(0) * normal.get(2)))
			{
				Int d = (other.dot * normal.get(0)) - (other.normal.get(0) * dot);

				Int mod = normal.get(0) * c;
				Int lhs = (dot * c) - (normal.get(2) * d);
				Int rhs = normal.get(1) * c;

				Int t = minimize_modulo_difference(lhs, rhs, mod);

				intersection.origin(0) = (lhs - (t * rhs)) / mod;
				intersection.origin(1) = t;
				intersection.origin(2) = d / c;
			}
			else
			{
				return false;
			}

			normal.cross(other.normal, intersection.direction);
			return true;
		}

		bool Plane::get_intersection(const Line& line, Line& intersection) const
		{
			// Check if line is not parallel to the plane
			if (Int denom = normal.dot(line.direction))
			{
				line.get(Frac(dot - (normal.dot(line.origin)), denom), intersection.origin);
				return normal.dot(intersection.origin) == dot;
			}

			// If the line is parallel to the plane, check if the line lies on the plane
			else if (normal.dot(line.origin) == dot)
			{
				intersection = line;
				return true;
			}

			return false;
		}

		void Plane::get_closest_point(const vec3i& point, vec3i& closest) const
		{
			closest = point + (normal * Frac(dot - normal.dot(point), normal.square_sum()));
		}


		void Parallelogram::clamp(vec3i& point) const
		{
			// The coefficients for each of the radius vectors
			Frac t[2];

			// Solve for the coefficients
			solve(sides[0], sides[1], point, t[0], t[1]);

			// Clamp each coefficient to the range [0, 1]
			for (int k = 1; k >= 0; --k)
			{
				if (t[k] < 0)
					t[k] = 0;
				else if (t[k] > 1)
					t[k] = 1;
			}

			// Solve for the clamped point
			point = pos + (sides[0] * t[0]) + (sides[1] * t[1]);
		}
		
		void Parallelogram::get_closest_point(const vec3i& point, vec3i& closest) const
		{
			// Calculate the closest point on the parallelogram's plane to the given point
			vec3i normal;
			sides[0].cross(sides[1], normal);

			closest = point + (normal * Frac(normal.dot(pos - point), normal.square_sum()));

			// Clamp the closest point to the bounds of the parallelogram
			clamp(closest);
		}

		void Parallelogram::get_closest_point(const Line& line, vec3i& closest) const
		{
			// TODO see if this can be done more efficiently?

			// Construct five planes: one representing the plane that the parallelogram is on, and the others representing the planes parallel to one of the sides and perpendicular to the parallelogram itself
			Plane p[5];
			sides[0].cross(sides[1], p[4].normal);
			p[4].dot = p[4].normal.dot(pos);

			for (int k = 3; k >= 0; --k)
			{
				sides[k / 2].cross(p[4].normal, p[k].normal);
				p[k].dot = p[k].normal.dot(k % 2 == 0 ? pos : (pos + sides[k / 2]));
			}

			Int smallest_dist = std::numeric_limits<Int>::max();
			for (int k = 4; k >= 0; --k)
			{
				// Calculate the intersection between the line and each of the planes
				Line plane_intersection;
				if (p[k].get_intersection(line, plane_intersection))
				{
					// Determine the closest point on the parallelogram to the intersection of the line and the plane
					vec3i diff;
					if (plane_intersection.direction.square_sum() > 0)
					{
						// Plane is parallel to line
						diff = plane_intersection.origin; // TODO pick point closest to center of parallelogram
					}
					else
					{
						// Plane is not parallel to line
						diff = plane_intersection.origin;
					}

					vec3i closest_on_parallelogram;
					get_closest_point(diff, closest_on_parallelogram);
					diff -= closest_on_parallelogram;

					// If this point is closer than the prior closest, replace it
					Int dist = diff.square_sum();
					if (dist < smallest_dist)
					{
						closest = closest_on_parallelogram;
						smallest_dist = dist;
					}
				}
			}
		}

		void Parallelogram::get_closest_point(const Parallelogram& quad, vec3i& closest) const
		{
			// Construct the planes that each parallelogram occupies
			Plane p[2];

			sides[0].cross(sides[1], p[0].normal);
			p[0].dot = pos.dot(p[0].normal);

			quad.sides[0].cross(quad.sides[1], p[1].normal);
			p[1].dot = quad.pos.dot(p[1].normal);

			// Calculate the intersection between the two planes
			Line plane_intersection;
			if (p[0].get_intersection(p[1], plane_intersection))
			{
				// The two parallelograms are not parallel or occupy the same plane
				vec3i closest_on_quad;
				quad.get_closest_point(plane_intersection, closest_on_quad);

				get_closest_point(closest_on_quad, closest);
			}
			else
			{
				// The two parallelograms exist on parallel planes that do not intersect
			}
		}


		
		void Line::get(Int t, vec3i& point) const
		{
			point = origin + (direction * t);
		}

		void Line::get(const Frac& t, vec3i& point) const
		{
			point = origin + (direction * t);
		}

		void Line::get_closest_point(const vec3i& point, vec3i& closest) const
		{
			get(Frac(direction.dot(origin - point), direction.square_sum()), closest);
		}


		void Ray::get(Int t, vec3i& point) const
		{
			point = t <= 0 ? origin : (origin + (direction * t));
		}

		void Ray::get(const Frac& t, vec3i& point) const
		{
			if ((t.numerator < 0 && t.denominator > 0) || (t.numerator > 0 && t.denominator < 0))
				point = origin;
			else
				point = origin + (direction * t);
		}


		void Segment::get(Int t, vec3i& point) const
		{
			point = t <= 0 ? origin : (origin + direction);
		}

		void Segment::get(const Frac& t, vec3i& point) const
		{
			// t < 0
			if ((t.numerator < 0 && t.denominator > 0) || (t.numerator > 0 && t.denominator < 0))
				point = origin;

			// t > 1
			else if (abs(t.numerator) > abs(t.denominator))
				point = origin + direction;

			// 0 <= t <= 1
			else
				point = origin + (direction * t);
		}



		const vec3i& Shape::get_position() const
		{
			return m_Position;
		}

		void Shape::set_position(const vec3i& pos)
		{
			m_Position = pos;
		}

		void Shape::translate(const vec3i& trans)
		{
			set_position(m_Position + trans);
		}


		Point::Point(const vec3i& pos)
		{
			m_Position = pos;
		}

		void Point::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			closest = m_Position;
		}

		void Point::get_closest_point(const Line& line, vec3i& closest) const
		{
			closest = m_Position;
		}

		void Point::get_closest_point(const Parallelogram& quad, vec3i& closest) const
		{
			closest = m_Position;
		}

		Int Point::get_distance(const Shape* other) const
		{
			vec3i diff;
			other->get_closest_point(m_Position, diff);
			diff -= m_Position;

			return diff.square_sum();
		}


		Sphere::Sphere(const vec3i& pos, Int radius)
		{
			m_Position = pos;
			m_RadiusSquared = radius * radius;
		}

		void Sphere::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			vec3i diff = pos - m_Position;
			Int dist = diff.square_sum();

			if (dist <= m_RadiusSquared)
			{
				closest = pos;
			}
			else
			{
				closest = (diff * (Int)floor(sqrt(m_RadiusSquared))) / (Int)ceil(sqrt(dist));
				closest += m_Position;
			}
		}

		void Sphere::get_closest_point(const Line& line, vec3i& closest) const
		{
			vec3i diff;
			line.get_closest_point(m_Position, diff);
			diff -= m_Position;

			vec3f dir;
			diff.normalize(dir);
			dir *= (Float)sqrtf(m_RadiusSquared);

			closest = m_Position;
			for (int k = 2; k >= 0; --k)
				closest(k) += (Int)round(dir.get(k));
		}

		void Sphere::get_closest_point(const Parallelogram& quad, vec3i& closest) const
		{
			// Calculate the closest point on the plane to the center of the sphere
			vec3i dir;
			quad.get_closest_point(m_Position, dir);

			// Calculate the direction from the sphere to the plane
			dir -= m_Position;
			Int len_squared = dir.square_sum();
			if (len_squared <= m_RadiusSquared)
			{
				closest = m_Position + dir;
			}
			else
			{
				// Scale the direction to have length equal to the radius of the sphere
				vec3f dir_f;
				dir.normalize(dir_f);
				dir_f *= (Float)sqrtf(m_RadiusSquared);

				// Convert to integers
				closest = m_Position;
				for (int k = 2; k >= 0; --k)
				{
					closest(k) += (Int)(dir.get(k) < 0
						? ceil(dir_f.get(k))
						: floor(dir_f.get(k))
						);
				}
			}
		}

		Int Sphere::get_distance(const Shape* other) const
		{
			vec3i diff;
			other->get_closest_point(m_Position, diff);
			diff -= m_Position;

			Int dist = diff.square_sum() - m_RadiusSquared;
			return dist < 0 ? 0 : dist;
		}


		RectangularPrism::RectangularPrism(const vec3i& pos, const vec3i& dimensions)
		{
			m_Position = pos;
			m_Dimensions = dimensions;
		}

		void RectangularPrism::clamp(vec3i& point) const
		{
			for (int k = 2; k >= 0; --k)
			{
				if (point.get(k) < m_Position.get(k))
					point(k) = m_Position.get(k);
				else if (point.get(k) > m_Position.get(k) + m_Dimensions.get(k))
					point(k) = m_Position.get(k) + m_Dimensions.get(k);
			}
		}

		void RectangularPrism::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			for (int k = 2; k >= 0; --k)
			{
				int n = pos.get(k);

				if (n <= m_Position.get(k))
					closest.set(k, 0, m_Position.get(k));
				else if (n >= m_Position.get(k) + m_Dimensions.get(k))
					closest.set(k, 0, m_Position.get(k) + m_Dimensions.get(k));
				else
					closest.set(k, 0, n);
			}
		}

		void RectangularPrism::get_closest_point(const Line& line, vec3i& closest) const
		{
			// TODO do this more efficiently?
			Int smallest_dist = std::numeric_limits<Int>::max();

			// Check the intersection of the ray with every planar edge, and see which intersection is the closest
			for (int k = 2; k >= 0; --k)
			{
				for (int m = 1; m >= 0; --m)
				{
					// Construct a parallelogram for the face
					Parallelogram p;

					p.pos = m_Position;
					if (m == 1)
						p.pos(k) += m_Dimensions.get(k);

					p.sides[0]((k + 1) % 3) = m_Dimensions.get((k + 1) % 3);
					p.sides[1]((k + 2) % 3) = m_Dimensions.get((k + 2) % 3);

					// Calculate the closest point to the line on that face
					vec3i closest_on_face;
					p.get_closest_point(line, closest_on_face);

					// Calculate the difference between the face and the line
					vec3i diff;
					line.get_closest_point(closest_on_face, diff);
					diff -= closest_on_face;

					// Calculate the distance from the line
					Int dist = diff.square_sum();
					if (dist < smallest_dist)
						smallest_dist = dist;
				}
			}
		}

		void RectangularPrism::get_closest_point(const Parallelogram& quad, vec3i& closest) const
		{
			Int smallest_dist = std::numeric_limits<Int>::max();

			for (int k = 2; k >= 0; --k)
			{
				for (int m = 1; m >= 0; --m)
				{
					// Construct the parallelogram for the face
					Parallelogram p =
					{
						m_Position,
						{
							vec3i(0, 0, 0),
							vec3i(0, 0, 0)
						}
					};
					if (m > 0)
						p.pos(k) += m_Dimensions.get(k);
					p.sides[0]((k + 1) % 3) = m_Dimensions.get((k + 1) % 3);
					p.sides[1]((k + 2) % 3) = m_Dimensions.get((k + 2) % 3);

					// Calculate the closest point on the face
					vec3i closest_on_face;
					p.get_closest_point(quad, closest_on_face);
					vec3i diff;
					quad.get_closest_point(closest_on_face, diff);
					diff -= closest_on_face;

					// Calculate distance between the face and the parallelogram
					Int dist = diff.square_sum();
					if (dist < smallest_dist)
					{
						closest = closest_on_face;
						smallest_dist = dist;
					}
				}
			}
		}

		Int RectangularPrism::get_distance(const Shape* other) const
		{
			Int smallest_dist = std::numeric_limits<Int>::max();

			for (int k = 2; k >= 0; --k)
			{
				for (int m = 1; m >= 0; --m)
				{
					Parallelogram p = 
					{
						m_Position,
						{
							vec3i(0, 0, 0),
							vec3i(0, 0, 0)
						}
					};
					if (m > 0)
						p.pos(k) += m_Dimensions.get(k);
					p.sides[0]((k + 1) % 3) = m_Dimensions.get((k + 1) % 3);
					p.sides[1]((k + 2) % 3) = m_Dimensions.get((k + 2) % 3);

					vec3i diff;
					other->get_closest_point(p, diff);
					for (int n = 2; n >= 0; --n)
					{
						if (diff.get(n) < m_Position.get(n))
							diff(n) -= m_Position.get(n);
						else if (diff.get(n) > m_Position.get(n) + m_Dimensions.get(n))
							diff(n) -= m_Position.get(n) + m_Dimensions.get(n);
						else
							diff(n) = 0;
					}

					Int dist = diff.square_sum();
					if (dist < smallest_dist)
						smallest_dist = dist;
				}
			}

			return smallest_dist;
		}


		Rectangle::Rectangle(const vec3i& pos, const vec3i& dimensions) : RectangularPrism(pos, dimensions) {}

		void Rectangle::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			// Find the closest point on the plane that the rectangle is on to the given point
			Frac t(
				(m_Dimensions.get(0) * (m_Position.get(0) - pos.get(0))) + (m_Dimensions.get(1) * (m_Position.get(1) - pos.get(1))),
				(m_Dimensions.get(0) * m_Dimensions.get(0)) + (m_Dimensions.get(1) * m_Dimensions.get(1))
			);

			closest = m_Position - (m_Dimensions * t);
			closest(2) = pos.get(2);

			// Clamp the closest point to the bounds of the rectangle
			clamp(closest);
		}

		void Rectangle::get_closest_point(const Line& line, vec3i& closest) const
		{
			// Find the intersection between the line and the plane that the rectangle is on
			if (Int denom = (m_Dimensions.get(1) * line.direction.get(0)) - (m_Dimensions.get(0) * line.direction.get(1)))
			{
				Frac t(
					(m_Dimensions.get(1) * (m_Position.get(0) - line.origin.get(0))) - (m_Dimensions.get(0) * (m_Position.get(1) - line.origin.get(1))),
					denom
				);

				line.get(t, closest);
				clamp(closest);
			}
			else
			{
				// Line is parallel to the plane that the rectangle is on, so just return the closest point to the origin
				get_closest_point(line.origin, closest);
			}
		}

		void Rectangle::get_closest_point(const Parallelogram& quad, vec3i& closest) const
		{
			// Construct the parallelogram that constitutes the bounds of the rectangle
			Parallelogram p =
			{
				m_Position,
				{
					vec3i(m_Dimensions.get(0), m_Dimensions.get(1), 0),
					vec3i(0, 0, m_Dimensions.get(2))
				}
			};

			// Use the parallelogram to get the closest point
			p.get_closest_point(quad, closest);
		}

		Int Rectangle::get_distance(const Shape* other) const
		{
			// Calculate the closest point to the rectangle on the other shape
			Parallelogram quad = {
				m_Position,
				{
					vec3i(m_Dimensions.get(0), m_Dimensions.get(1), 0),
					vec3i(0, 0, m_Dimensions.get(2))
				}
			};

			vec3i closest_on_other;
			other->get_closest_point(quad, closest_on_other);

			// Calculate the closest point to that point
			vec3i closest_on_rectangle;
			get_closest_point(closest_on_other, closest_on_rectangle);

			// Calculate the (squared) distance between the two points
			vec3i diff = closest_on_rectangle - closest_on_other;
			return diff.square_sum();
		}

	}
}