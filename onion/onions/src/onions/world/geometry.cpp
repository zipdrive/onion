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
			} 
			while (--a_mod >= 0);
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



		bool Plane::get_intersection(const Plane& other, Ray& intersection) const
		{
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

		void Plane::get_closest_point(const vec3i& point, vec3i& closest) const
		{
			Int denom = normal.square_sum();
			closest = (normal * (dot - normal.dot(point))) / denom;
			closest += point;
		}


		bool Ray::get_intersection(const Plane& plane, vec3i& intersection) const
		{
			if (Int denom = plane.normal.dot(direction))
			{
				Int t = (plane.dot - (plane.normal.dot(origin))) / denom;
				intersection = origin + (t * direction);
				return true;
			}
			else if (plane.normal.dot(origin) == plane.dot)
			{
				intersection = origin;
				return true;
			}
			else
			{
				return false;
			}
		}

		void Ray::get_closest_point(const vec3i& point, vec3i& closest) const
		{
			closest = origin - point;
			Int ab = closest.dot(direction);
			Int bb = direction.square_sum();
			
			for (int k = 2; k >= 0; --k)
				closest(k) += direction.get(k) * ab / bb;
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
		
		bool Point::get_intersection(const Ray& ray) const
		{
			vec3i closest;
			ray.get_closest_point(m_Position, closest);

			return closest == m_Position;
		}

		bool Point::get_intersection(const Shape* shape) const
		{
			vec3i p;
			shape->get_closest_point(m_Position, p);
			return p == m_Position;
		}

		void Point::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			closest = m_Position;
		}

		void Point::get_closest_point(const Ray& ray, vec3i& closest) const
		{
			closest = m_Position;
		}

		void Point::get_closest_point(const Plane& plane, vec3i& closest) const
		{
			closest = m_Position;
		}


		Sphere::Sphere(const vec3i& pos, Int radius)
		{
			m_Position = pos;
			m_RadiusSquared = radius * radius;
		}
		
		bool Sphere::get_intersection(const Ray& ray) const
		{
			// Check if the origin is inside or on the sphere
			vec3i diff = ray.origin - m_Position;

			if (diff.square_sum() <= m_RadiusSquared)
			{
				return true;
			}
			else
			{
				ray.get_closest_point(m_Position, diff);
				
				diff -= m_Position;
				return diff.square_sum() <= m_RadiusSquared;
			}
		}

		bool Sphere::get_intersection(const Shape* shape) const
		{
			vec3i p;
			shape->get_closest_point(m_Position, p);
			p -= m_Position;

			return p.square_sum() <= m_RadiusSquared;
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

		void Sphere::get_closest_point(const Ray& ray, vec3i& closest) const
		{
			vec3i diff = ray.origin - m_Position;
			if (diff.square_sum() < m_RadiusSquared)
			{
				closest = ray.origin;
				return;
			}

			Frac t(diff.dot(ray.direction), ray.direction.square_sum());
			vec3i to_closest_on_line = diff + (ray.direction * t);

			vec3f dir;
			to_closest_on_line.normalize(dir);
			dir *= sqrtf(m_RadiusSquared);

			closest = m_Position;
			for (int k = 2; k >= 0; --k)
				closest(k) += (Int)round(dir.get(k));
		}

		void Sphere::get_closest_point(const Plane& plane, vec3i& closest) const
		{
			// Calculate the closest point on the plane to the center of the sphere
			vec3i dir;
			plane.get_closest_point(m_Position, dir);

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
				dir_f *= sqrtf(m_RadiusSquared);

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


		RectangularPrism::RectangularPrism(const vec3i& pos, const vec3i& dimensions)
		{
			m_Position = pos;
			m_Dimensions = dimensions;
		}

		bool RectangularPrism::get_intersection(const Ray& ray) const
		{
			// Check if the ray's origin is in the bounds
			for (int k = 0; k <= 3; ++k)
			{
				if (k == 3)
				{
					return true;
				}

				if (ray.origin.get(k) < m_Position.get(k) || ray.origin.get(k) > m_Position.get(k) + m_Dimensions.get(k))
					break;
			}

			// Check the intersection of the ray with each planar face of the prism
			for (int k = 0; k < 3; ++k)
			{
				Plane plane;
				plane.normal(k) = 1;

				if (ray.origin.get(k) <= m_Position.get(k) && ray.direction.get(k) > 0)
				{
					plane.dot = m_Position.get(k);
				}
				else if (ray.origin.get(k) >= m_Position.get(k) + m_Dimensions.get(k) && ray.direction.get(k) < 0)
				{
					plane.dot = m_Position.get(k) + m_Dimensions.get(k);
				}
				else
				{
					continue;
				}

				vec3i intersection;
				if (ray.get_intersection(plane, intersection))
				{
					int k1 = (k + 1) % 3, k2 = (k + 2) % 3;
					if (intersection.get(k1) >= m_Position.get(k1)
						&& intersection.get(k1) <= m_Position.get(k1) + m_Dimensions.get(k1)
						&& intersection.get(k2) >= m_Position.get(k2)
						&& intersection.get(k2) <= m_Position.get(k2) + m_Dimensions.get(k2))
						return true;
				}
			}

			return false;
		}

		bool RectangularPrism::get_intersection(const Shape* shape) const
		{
			for (int k = 2; k >= 0; --k)
			{
				// Construct a plane aligned with the k-th axis that cuts through the center of the prism
				vec3i normal;
				normal(k) = 1;

				Plane plane = {
					normal,
					m_Position.get(k) + (m_Dimensions.get(k) / 2)
				};

				// Find the closest point on the shape to the midplane
				vec3i closest;
				shape->get_closest_point(plane, closest);

				// 
				if (closest.get(k) < m_Position.get(k)
					|| closest.get(k) > m_Position.get(k) + m_Dimensions.get(k))
					return false;
			}

			// This is a heuristic; there are edge cases (like with a sphere) where you can get to this point with a shape that doesn't intersect the prism
			return true;
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

		void RectangularPrism::get_closest_point(const Ray& ray, vec3i& closest) const
		{
			// TODO do this more efficiently?
			Int smallest_dist = std::numeric_limits<Int>::max();

			// Check the intersection of the ray with every planar edge, and see which intersection is the closest
			for (int k = 2; k >= 0; --k)
			{
				if (ray.direction.get(k) != 0)
				{
					Plane p[2];
					p[0].normal(k) = 1;
					p[0].dot = m_Position.get(k) + m_Dimensions.get(k);
					p[1].normal(k) = -1;
					p[1].dot = -m_Position.get(k);

					for (int m = 1; m >= 0; --m)
					{
						vec3i closest_on_face;
						if (ray.get_intersection(p[m], closest_on_face))
						{
							for (int n = 2; n >= 0; --n)
							{
								if (closest_on_face.get(n) < m_Position.get(n))
									closest_on_face(n) = m_Position.get(n);
								else if (closest_on_face.get(n) > m_Position.get(n) + m_Dimensions.get(n))
									closest_on_face(n) = m_Position.get(n) + m_Dimensions.get(n);
							}

							vec3i diff;
							ray.get_closest_point(closest_on_face, diff);
							diff -= closest_on_face;
							
							Int dist = diff.square_sum();
							if (dist < smallest_dist)
							{
								closest = closest_on_face;
								smallest_dist = dist;
							}
						}
					}
				}
			}
		}

		void RectangularPrism::get_closest_point(const Plane& plane, vec3i& closest) const
		{
			Int smallest_dist = std::numeric_limits<Int>::max();

			for (int k = 2; k >= 0; --k)
			{
				for (int m = 1; m >= 0; --m)
				{
					Plane p;
					p.normal(k) = 1;
					p.dot = m_Position.get(k) + (m * m_Dimensions.get(k));

					Ray intersection;
					plane.get_intersection(p, intersection);

					// TODO this next bit is a heuristic, figure out a better way to do it!
					vec3i closest_on_ray;
					intersection.get_closest_point(m_Position + (m_Dimensions / 2), closest_on_ray);

					vec3i diff;
					for (int n = 2; n >= 0; --n)
					{
						if (closest_on_ray.get(n) < m_Position.get(n))
							diff(n) = m_Position.get(n) - closest_on_ray.get(n);
						else if (closest_on_ray.get(n) > m_Position.get(n) + m_Dimensions.get(n))
							diff(n) = m_Position.get(n) + m_Dimensions.get(n) - closest_on_ray.get(n);
						else
							diff(n) = 0;
					}

					Int dist = diff.square_sum();
					if (dist < smallest_dist)
					{
						closest = diff + closest_on_ray;
						smallest_dist = dist;
					}
				}
			}
		}


		Rectangle::Rectangle(const vec3i& pos, const vec3i& dimensions)
		{
			m_Position = pos;
			m_Dimensions = dimensions;
		}

		bool Rectangle::get_intersection(const Ray& ray) const
		{
			Plane plane = {
				vec3i(-m_Dimensions.get(1), m_Dimensions.get(0), 0),
				(m_Dimensions.get(0) * m_Position.get(1)) - (m_Dimensions.get(1) * m_Position.get(0))
			};

			vec3i intersection;
			if (ray.get_intersection(plane, intersection))
			{
				// Check if the intersection lies within the rectangle
				for (int k = 2; k >= 0; --k)
					if (intersection.get(k) < m_Position.get(k) || intersection.get(k) > m_Position.get(k) + m_Dimensions.get(k))
						return false;
				return true;
			}

			return false;
		}

		bool Rectangle::get_intersection(const Shape* shape) const
		{
			Plane plane = {
				vec3i(-m_Dimensions.get(1), m_Dimensions.get(0), 0),
				(m_Position.get(1) * m_Dimensions.get(0)) - (m_Position.get(0) * m_Dimensions.get(1))
			};

			vec3i closest;
			shape->get_closest_point(plane, closest);

			if (closest.dot(plane.normal) == plane.dot) // TODO there are cases where the shape might intersect but this returns false just due to dealing with integers
			{
				// Ensure closest point lies within rectangle
				for (int k = 2; k >= 0; --k)
					if (closest.get(k) < m_Position.get(k) || closest.get(k) > m_Position.get(k) + m_Dimensions.get(k))
						return false;
				return true;
			}
		}

		void Rectangle::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			// Find the closest point on the plane that the rectangle is on to the given point
			Frac t(
				(m_Dimensions.get(0) * (m_Position.get(0) - pos.get(0))) + (m_Dimensions.get(1) * (m_Position.get(1) - pos.get(1))),
				(m_Dimensions.get(0) * m_Dimensions.get(0)) + (m_Dimensions.get(1) * m_Dimensions.get(1))
			);

			closest = m_Position - (m_Dimensions * t);

			// Clamp the closest point to the bounds of the rectangle
			for (int k = 1; k >= 0; --k)
			{
				if (closest.get(k) < m_Position.get(k))
					closest(k) = m_Position.get(k);
				else if (closest.get(k) > m_Position.get(k) + m_Dimensions.get(k))
					closest(k) = m_Position.get(k) + m_Dimensions.get(k);
			}
			
			// Determine the z-coordinate of the closest point
			if (pos.get(2) < m_Position.get(2))
				closest(2) = m_Position.get(2);
			else if (pos.get(2) > m_Position.get(2) + m_Dimensions.get(2))
				closest(2) = m_Position.get(2) + m_Dimensions.get(2);
			else
				closest(2) = pos.get(2);
		}

		void Rectangle::get_closest_point(const Ray& ray, vec3i& closest) const
		{
			// Find the intersection between the ray and the plane that the rectangle is on
			Frac t(
				(m_Dimensions.get(1) * (m_Position.get(0) - ray.origin.get(0))) - (m_Dimensions.get(0) * (m_Position.get(1) - ray.origin.get(1))),
				(m_Dimensions.get(1) * ray.direction.get(0)) - (m_Dimensions.get(0) * ray.direction.get(1))
			);

			if (t.denominator == 0)
			{
				// Ray is parallel to the plane that the rectangle is on, so just return the closest point to the origin
				get_closest_point(ray.origin, closest);
			}
			else
			{
				closest = ray.origin + (ray.direction * t);
				for (int k = 2; k >= 0; --k)
				{
					// Clamp the closest point to the bounds of the rectangle
					if (closest.get(k) < m_Position.get(k))
						closest(k) = m_Position.get(k);
					else if (closest.get(k) > m_Position.get(k) + m_Dimensions.get(k))
						closest(k) = m_Position.get(k) + m_Dimensions.get(k);
				}
			}
		}

		void Rectangle::get_closest_point(const Plane& plane, vec3i& closest) const
		{
			// Calculate the line that forms the intersection between the given plane and the plane this rectangle is on
			// If there is a free variable, set it equal to the midpoint of the rectangle
			vec3i normal(-m_Dimensions.get(1), m_Dimensions.get(0), 0);

			if (normal.get(0) == 0)
			{
				closest(1) = m_Position.dot(normal) / normal.get(1);

				Int Z = normal.get(1) * plane.normal.get(2);
				Int D = (normal.get(1) * plane.dot) - (m_Position.dot(normal) * plane.normal.get(1));

				if (plane.normal.get(0) == 0)
				{
					closest(0) = m_Position.get(0) + (m_Dimensions.get(0) / 2);
					closest(2) = D / Z;
				}
				else
				{
					closest(2) = m_Position.get(2) + (m_Dimensions.get(2) / 2);
					closest(0) = (D - (closest.get(2) * Z)) / (plane.normal.get(0) * normal.get(1));
				}
			}
			else if (Int Y = (normal.get(0) * plane.normal.get(1)) - (normal.get(1) * plane.normal.get(0)))
			{
				closest(2) = m_Position.get(2) + (m_Dimensions.get(2) / 2);

				Int Z = normal.get(0) * plane.normal.get(2);
				Int D = (normal.get(0) * plane.dot) - (m_Position.dot(normal) * plane.normal.get(0));

				closest(1) = (D - (closest.get(2) * Z)) / Y;
				
				Z = -Z * normal.get(1);
				D = (m_Position.dot(normal) * Y) - (D * normal.get(1));
				Int X = normal.get(0) * Y;

				closest(0) = (D - (closest.get(2) * Z)) / X;
			}
			else
			{
				Int Z = normal.get(0) * plane.normal.get(2);
				Int D = (normal.get(0) * plane.dot) - (m_Position.dot(normal) * plane.normal.get(0));

				closest(2) = D / Z;
				closest(1) = m_Position.get(1) + (m_Dimensions.get(1) / 2);
				closest(0) = (m_Position.dot(normal) - (closest.get(1) * normal.get(1))) / normal.get(0);
			}

			// Clamp closest point to inside of rectangle
			for (int k = 2; k >= 0; --k)
			{
				if (closest.get(k) < m_Position.get(k))
					closest(k) = m_Position.get(k);
				else if (closest.get(k) > m_Position.get(k) + m_Dimensions.get(k))
					closest(k) = m_Position.get(k) + m_Dimensions.get(k);
			}
		}

	}
}