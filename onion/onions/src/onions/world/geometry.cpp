#include "../../../include/onions/world/geometry.h"

namespace onion
{
	namespace world
	{

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
		
		bool Point::get_intersection(const Ray& ray, vec3i& intersection) const
		{
			vec3i diff = m_Position - ray.origin;
			for (int k = 2; k >= 0; --k)
			{
				if (diff.get(k) != 0)
				{
					// Check to make sure that the difference vector (between the point and the ray origin) is parallel to the ray direction
					if (ray.direction.get(k) * diff == diff.get(k) * ray.direction)
					{
						intersection = m_Position;
						return true;
					}
					return false;
				}
			}

			intersection = m_Position;
			return true;
		}

		void Point::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			closest = m_Position;
		}

		void Point::get_closest_point(const Ray& ray, vec3i& closest) const
		{
			closest = m_Position;
		}


		Sphere::Sphere(const vec3i& pos, Int radius)
		{
			m_Position = pos;
			m_RadiusSquared = radius * radius;
		}
		
		bool Sphere::get_intersection(const Ray& ray, vec3i& intersection) const
		{
			// Check if the origin is inside or on the sphere
			vec3i diff = ray.origin - m_Position;
			Int dist = diff.square_sum();

			if (dist < m_RadiusSquared)
			{
				intersection = m_Position;
				return true;
			}
			else
			{
				Int aa = diff.square_sum();
				Int ab = diff.dot(ray.direction);
				Int bb = ray.direction.square_sum();

				Int disc = (ab * ab) - (bb * (aa - m_RadiusSquared));
				if (disc >= 0)
				{
					Int t = (Int)ceil((sqrt(disc) - ab) / bb);
					intersection = ray.origin + ((INT_VEC3)ray.direction * t);
					return true;
				}
				return false;
			}
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
				diff *= (Int)round(sqrt(m_RadiusSquared));
				Int denominator = (Int)ceil(sqrt(dist));
				closest.set(0, 0, diff.get(0) / denominator);
				closest.set(1, 0, diff.get(1) / denominator);
				closest.set(2, 0, diff.get(2) / denominator);

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

			Int ab = diff.dot(ray.direction);
			Int bb = ray.direction.square_sum();
			vec3i to_closest_on_line = diff;
			for (int k = 2; k >= 0; --k)
				to_closest_on_line(k) += ray.direction.get(k) * ab / bb;

			vec3f dir;
			to_closest_on_line.normalize(dir);
			dir = (FLOAT_VEC3)dir * sqrtf(m_RadiusSquared);

			closest = m_Position;
			for (int k = 2; k >= 0; --k)
				closest(k) += (Int)round(dir.get(k));
		}


		RectangularPrism::RectangularPrism(const vec3i& pos, const vec3i& dimensions)
		{
			m_Position = pos;
			m_Dimensions = dimensions;
		}

		bool RectangularPrism::get_intersection(const Ray& ray, vec3i& intersection) const
		{
			// Check if the ray's origin is in the bounds
			for (int k = 0; k <= 3; ++k)
			{
				if (k == 3)
				{
					intersection = ray.origin;
					return true;
				}

				if (ray.origin.get(k) < m_Position.get(k) || ray.origin.get(k) > m_Position.get(k) + m_Dimensions.get(k))
					break;
			}

			// Check the intersection of the ray with each planar face of the prism
			for (int k = 0; k < 3; ++k)
			{
				if (ray.origin.get(k) <= m_Position.get(k) && ray.direction.get(k) > 0)
				{
					Int diff = m_Position.get(k) - ray.origin.get(k);
					Int t = (diff / ray.direction.get(k)) + (diff % ray.direction.get(k) == 0 ? 0 : 1);
					vec3i planeIntersect = ray.origin + ((INT_VEC3)ray.direction * t);

					for (int m = 0; m <= 3; ++m)
					{
						if (m == 3)
						{
							intersection = planeIntersect;
							return true;
						}

						if (planeIntersect.get(m) < m_Position.get(m) || planeIntersect.get(m) > m_Position.get(m) + m_Dimensions.get(m))
							break;
					}
				}
				else if (ray.origin.get(k) >= m_Position.get(k) + m_Dimensions.get(k) && ray.direction.get(k) < 0)
				{
					Int diff = m_Position.get(k) + m_Dimensions.get(k) - ray.origin.get(k);
					Int t = (diff / ray.direction.get(k)) + (diff % ray.direction.get(k) == 0 ? 0 : 1);
					vec3i planeIntersect = ray.origin + ((INT_VEC3)ray.direction * t);

					for (int m = 0; m <= 3; ++m)
					{
						if (m == 3)
						{
							intersection = planeIntersect;
							return true;
						}

						if (planeIntersect.get(m) < m_Position.get(m) || planeIntersect.get(m) > m_Position.get(m) + m_Dimensions.get(m))
							break;
					}
				}
			}

			return false;
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
			Int smallest_dist = INT_MAX;

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
						Int numer = p[m].dot - p[m].normal.dot(ray.origin);
						Int denom = p[m].normal.dot(ray.direction);

						if (denom != 0) // Make sure plane isn't parallel to the ray
						{
							vec3i intersect = ray.origin;
							vec3i closest_on_prism;
							for (int n = 2; n >= 0; --n)
							{
								intersect(n) += ray.direction.get(n) * numer / denom;

								if (intersect.get(n) < m_Position.get(n))
									closest_on_prism(n) = m_Position.get(n);
								else if (intersect.get(n) > m_Position.get(n) + m_Dimensions.get(n))
									closest_on_prism(n) = m_Position.get(n) + m_Dimensions.get(n);
								else
									closest_on_prism(n) = intersect.get(n);
							}

							vec3i diff = intersect - closest_on_prism;
							Int dist = diff.square_sum();
							if (dist < smallest_dist)
							{
								closest = closest_on_prism;
								smallest_dist = dist;
							}
						}
					}
				}
			}
		}


		Rectangle::Rectangle(const vec3i& pos, const vec3i& dimensions)
		{
			m_Position = pos;
			m_Dimensions = dimensions;
		}

		bool Rectangle::get_intersection(const Ray& ray, vec3i& intersection) const
		{
			Int numer = (m_Dimensions.get(1) * (m_Position.get(0) - ray.origin.get(0))) - (m_Dimensions.get(0) * (m_Position.get(1) - ray.origin.get(1)));
			Int denom = (m_Dimensions.get(1) * ray.direction.get(0)) - (m_Dimensions.get(0) * ray.direction.get(1));

			if (denom == 0)
			{
				// Ray is parallel to the plane that the rectangle is on
				return false;
			}
			else
			{
				// Find the intersection between the ray and the plane that the rectangle is on
				intersection = ray.origin;
				for (int k = 2; k >= 0; --k)
				{
					intersection(k) += ray.direction.get(k) * numer / denom;

					// Check if the intersection lies within the rectangle
					if (intersection.get(k) < m_Position.get(k) || intersection.get(k) > m_Position.get(k) + m_Dimensions.get(k))
						return false;
				}

				return true;
			}
		}

		void Rectangle::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			// Find the closest point on the plane that the rectangle is on to the given point
			Int numer = (m_Dimensions.get(1) * (m_Position.get(0) - pos.get(0))) - (m_Dimensions.get(0) * (m_Position.get(1) - pos.get(1)));
			Int denom = (m_Dimensions.get(0) * m_Dimensions.get(0)) + (m_Dimensions.get(1) * m_Dimensions.get(1));

			closest = pos;
			closest(0) += (numer * m_Dimensions.get(1)) / denom;
			closest(1) -= (numer * m_Dimensions.get(0)) / denom;

			// Clamp the closest point to the bounds of the rectangle
			for (int k = 2; k >= 0; --k)
			{
				if (closest.get(k) < m_Position.get(k))
					closest(k) = m_Position.get(k);
				else if (closest.get(k) > m_Position.get(k) + m_Dimensions.get(k))
					closest(k) = m_Position.get(k) + m_Dimensions.get(k);
			}
		}

		void Rectangle::get_closest_point(const Ray& ray, vec3i& closest) const
		{
			// Find the intersection between the ray and the plane that the rectangle is on
			Int numer = (m_Dimensions.get(1) * (m_Position.get(0) - ray.origin.get(0))) - (m_Dimensions.get(0) * (m_Position.get(1) - ray.origin.get(1)));
			Int denom = (m_Dimensions.get(1) * ray.direction.get(0)) - (m_Dimensions.get(0) * ray.direction.get(1));

			if (denom == 0)
			{
				// Ray is parallel to the plane that the rectangle is on, so just return the closest point to the origin
				get_closest_point(ray.origin, closest);
			}
			else
			{
				closest = ray.origin;
				for (int k = 2; k >= 0; --k)
				{
					closest(k) += ray.direction.get(k) * numer / denom;

					// Clamp the closest point to the bounds of the rectangle
					if (closest.get(k) < m_Position.get(k))
						closest(k) = m_Position.get(k);
					else if (closest.get(k) > m_Position.get(k) + m_Dimensions.get(k))
						closest(k) = m_Position.get(k) + m_Dimensions.get(k);
				}
			}
		}

	}
}