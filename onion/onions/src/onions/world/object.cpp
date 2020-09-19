#include "../../../include/onions/world/object.h"

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
		
		
		void PointBounds::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			closest = m_Position;
		}

		
		void SphereBounds::get_closest_point(const vec3i& pos, vec3i& closest) const
		{
			vec3i diff = pos - m_Position;
			int dist = diff.square_sum();

			if (dist <= m_RadiusSquared)
			{
				closest = pos;
			}
			else
			{
				diff *= (int)round(sqrt(m_RadiusSquared));
				int denominator = ceil(sqrt(dist));
				closest.set(0, 0, diff.get(0) / denominator);
				closest.set(1, 0, diff.get(1) / denominator);
				closest.set(2, 0, diff.get(2) / denominator);

				closest += m_Position;
			}
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

		void Object::display() const {}

	}
}