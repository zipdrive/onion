#include "../../../include/onions/world/object.h"
#include "../../../include/onions/world/lighting.h"

namespace onion
{
	namespace world
	{

		const vec3i& Bounds::get_position() const
		{
			return m_Position;
		}

		void Bounds::set_position(const vec3i& pos)
		{
			m_Position = pos;
		}

		void Bounds::translate(const vec3i& trans)
		{
			set_position(m_Position + trans);
		}
		
		
		bool PointBounds::get_intersection(const Ray& ray, vec3i& intersection) const
		{
			vec3i diff = m_Position - ray.origin;
			if (diff.get(0) < ray.direction.get(0))
			{
				if ((ray.direction.get(0) / diff.get(0)) * diff == ray.direction)
				{
					intersection = m_Position;
					return true;
				}
				return false;
			}
			else
			{
				if ((diff.get(0) / ray.direction.get(0)) * ray.direction == diff)
				{
					intersection = m_Position;
					return true;
				}
				return false;
			}
		}
		
		void PointBounds::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			closest = m_Position;
		}

		
		bool SphereBounds::get_intersection(const Ray& ray, vec3i& intersection) const
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
				Int aa = diff * diff;
				Int ab = diff * ray.direction;
				Int bb = ray.direction * ray.direction;

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
		
		void SphereBounds::get_closest_point(const vec3i& pos, vec3i& closest) const
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
				Int denominator = ceil(sqrt(dist));
				closest.set(0, 0, diff.get(0) / denominator);
				closest.set(1, 0, diff.get(1) / denominator);
				closest.set(2, 0, diff.get(2) / denominator);

				closest += m_Position;
			}
		}


		RectangleBounds::RectangleBounds(const vec3i& pos, const vec3i& dimensions)
		{
			m_Position = pos;
			m_Dimensions = dimensions;
		}

		bool RectangleBounds::get_intersection(const Ray& ray, vec3i& intersection) const
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
		
		void RectangleBounds::get_closest_point(const vec3i& pos, vec3i& closest) const
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



		Object::Object(Bounds* bounds)
		{
			m_Bounds = bounds;
		}

		Object::~Object()
		{
			if (m_Bounds)
				delete m_Bounds;
		}

		const Bounds* Object::get_bounds() const
		{
			return m_Bounds;
		}

		void Object::display(const vec2i& direction) const {}



		std::unordered_map<std::string, ObjectGenerator*> ObjectGenerator::m_Generators{};
		std::unordered_map<std::string, ObjectGenerator::Generator*> ObjectGenerator::m_GeneratorFinder{};

		ObjectGenerator::ObjectGenerator(std::string id)
		{
			m_Generators.emplace(id, this);
		}

		void ObjectGenerator::init()
		{
			// Register all types included as part of the Onion library
			set<CubeLightObjectGenerator>("light, cube");

			// Load all objects from file
			LoadFile file("res/data/world/objects.txt");
			while (file.good())
			{
				_StringData line;
				std::string id = file.load_data(line);

				std::string type;
				if (line.get("type", type))
				{
					auto iter = m_GeneratorFinder.find(type);
					if (iter != m_GeneratorFinder.end())
					{
						iter->second->generate(id, line);
					}
				}
			}
		}

		Object* ObjectGenerator::generate(std::string id, const _StringData& params)
		{
			auto iter = m_Generators.find(id);
			if (iter != m_Generators.end())
				return iter->second->generate(params);
			
			// Check if generators haven't been loaded yet
			if (m_Generators.empty())
			{
				init();

				iter = m_Generators.find(id);
				if (iter != m_Generators.end())
					return iter->second->generate(params);
			}

			return nullptr;
		}



		bool ObjectManager::compare(Bounds* lhs, Bounds* rhs)
		{
			// Get closest points in each bound to the screen
			vec3i distant = vec3i((INT_MAX - 1) * (INT_VEC2)m_SortDirection, INT_MIN);
			vec3i lhs_closest, rhs_closest;
			lhs->get_closest_point(distant, lhs_closest);
			rhs->get_closest_point(distant, rhs_closest);

			// Cast a ray from each closest point in the direction away from the screen, and see if it intersects the other bound
			Ray ray = { lhs_closest, vec3i(-1 * (INT_VEC2)m_SortDirection, 0) };
			vec3i intersect;
			if (rhs->get_intersection(ray, intersect))
				return true;
			ray.origin = rhs_closest;
			if (lhs->get_intersection(ray, intersect))
				return false;


		}

	}
}