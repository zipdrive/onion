#include "../../../include/onions/graphics/transform.h"
#include "../../../include/onions/world/camera.h"

namespace onion
{
	namespace world
	{

		WorldCamera::WorldCamera(const mat2x3i& frame_bounds) : m_FrameBounds(frame_bounds) {}
		
		void WorldCamera::__set_bounds() {}

		vec3i WorldCamera::get_position() const
		{
			return m_Position;
		}
		
		void WorldCamera::set_position(const vec3i& position)
		{
			translate(position - m_Position);
		}
		
		void WorldCamera::translate(const vec3i& trans)
		{
			vec3i prior = m_Position;
			vec3i current = prior + trans;

			vec3i diff = current - prior;
			if (diff.square_sum() > 0 && is_active())
			{
				Transform::view.translate(-diff.get(0), -diff.get(1), -diff.get(2));
				Transform::set_view();
			}

			m_Position += trans;
		}



		
		StaticTopDownWorldCamera::StaticTopDownWorldCamera(const mat2x3i& frame_bounds) : WorldCamera(frame_bounds) 
		{
			m_Position = vec3i(0, 0, 0);
			m_ZeroPosition = vec2i(0, 0);
		}

		void StaticTopDownWorldCamera::__set_bounds()
		{
			const vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				0
			);
			m_ZeroPosition = m_Position - halves - (get_normal() * m_Position.get(2));
		}

		void StaticTopDownWorldCamera::translate(const vec3i& trans)
		{
			WorldCamera::translate(trans);
			
			const vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				0
			);
			m_ZeroPosition = m_Position - halves - (get_normal() * m_Position.get(2));
		}

		vec3i StaticTopDownWorldCamera::get_normal() const
		{
			static const vec3i normal(0, -1, 1);
			return normal;
		}

		vec3f StaticTopDownWorldCamera::support(const vec3f& dir) const
		{
			const Int near = abs(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0));
			const vec3i n(0.f, -near, near);

			vec3i res;
			Float d_max = std::numeric_limits<Float>::min();

			for (int c = 7; c >= 0; --c)
			{
				vec3i p(m_ZeroPosition, 0);

				if (c % 2 == 0)
					p(0) += m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0);
				if ((c / 2) % 2 == 0)
					p(1) += m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0);
				if (c / 4 == 0)
					p += n;

				Float d = p.dot(dir);
				if (d > d_max)
				{
					res = p;
					d_max = d;
				}
			}

			return ONION_WORLD_GEOMETRY_SCALE * res;
		}

		bool StaticTopDownWorldCamera::compare(const Shape* lhs, const Shape* rhs) const
		{
			const Int near = abs(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0));
			const vec3i n(0, -near, near);
			const vec3i dx(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0), 0, 0);
			const vec3i dy(0, m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0), 0);

			// Check which object is closer to the screen
			Parallelogram rect(vec3i(m_ZeroPosition, 0) + n, dx, dy);
			Int dl = rect.get_distance(lhs);
			Int dr = rect.get_distance(rhs);
			if (dr < dl)
				return true;
			else if (dl < dr)
				return false;

			// Check which object is closer to the bottom of the screen
			rect = Parallelogram(m_ZeroPosition, dx, n);
			dl = rect.get_distance(lhs);
			dr = rect.get_distance(rhs);
			if (dr < dl)
				return true;
			else if (dl < dr)
				return false;

			// Check which object is closer to the left side of the screen
			rect = Parallelogram(m_ZeroPosition, dy, n);
			dl = rect.get_distance(lhs);
			dr = rect.get_distance(rhs);
			if (dr < dl)
				return true;
			return false;
		}
		
		void StaticTopDownWorldCamera::__activate()
		{
			// Create a pixel-perfect orthogonal projection centered in the middle of the screen
			vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0)) / 2
			);

			Transform::projection.ortho(
				-halves.get(0), halves.get(0),
				-halves.get(1), halves.get(1),
				-halves.get(2), halves.get(2)
			);

			// A positive y-unit pushes the coordinate farther from the camera.
			// A positive z-unit pulls the coordinate closer to the camera and up by one positive y-unit.
			Transform::projection.get() *= TransformMatrix(
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 1.f, 0.f,
				0.f, 0.f, -1.f, 0.f
			);
			
			// Center the camera at its position in model space
			Transform::view.translate(-m_Position.get(0), -m_Position.get(1), -m_Position.get(2));
		}



		DynamicAxonometricWorldCamera::DynamicAxonometricWorldCamera(const mat2x3i& frame_bounds, float top_view_angle, float side_view_angle) : m_TopViewAngle(0.f), m_SideViewAngle(side_view_angle), WorldCamera(frame_bounds) 
		{
			set_top(top_view_angle);
		}

		void DynamicAxonometricWorldCamera::translate(const vec3i& trans)
		{
			WorldCamera::translate(trans);

			const vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				0
			);
			m_ZeroPosition = m_Position - halves - (m_Normal * m_Position.get(2));
		}

		vec3i DynamicAxonometricWorldCamera::get_normal() const
		{
			return m_Normal;
		}

		vec3f DynamicAxonometricWorldCamera::support(const vec3f& dir) const
		{
			const vec3f n = m_Normal * (ONION_WORLD_GEOMETRY_SCALE * (m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0)) / m_Normal.get(2));

			vec3f res;
			Float d_max = std::numeric_limits<Float>::min();

			for (int c = 7; c >= 0; --c)
			{
				vec3f p(ONION_WORLD_GEOMETRY_SCALE * m_ZeroPosition, 0);

				if (c % 2 == 0)
					p += vec3f(ONION_WORLD_GEOMETRY_SCALE * m_Radii[0], 0);
				if ((c / 2) % 2 == 0)
					p += vec3f(ONION_WORLD_GEOMETRY_SCALE * m_Radii[1], 0);
				if (c / 4 == 0)
					p += ONION_WORLD_GEOMETRY_SCALE * m_Normal;

				Float d = p.dot(dir);
				if (d > d_max)
				{
					res = p;
					d_max = d;
				}
			}

			return res;
		}

		bool DynamicAxonometricWorldCamera::compare(const Shape* lhs, const Shape* rhs) const
		{
			// TODO

			const Int near = m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0);
			const vec3i n(0, -near, near);
			const vec3i dx(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0), 0, 0);
			const vec3i dy(0, m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0), 0);

			// Check if the highest z-coordinate of lhs is below the lowest z-coordinate of rhs
			Parallelogram r(m_ZeroPosition, dx, dy);
			Parallelogram t(vec3i(m_ZeroPosition, 0) + n, dx, dy);
			Int dl = near - t.get_distance(lhs);
			Int dr = r.get_distance(rhs);
			if (dl <= dr)
			{
				return true;
			}

			// Check if the highest z-coordinate of rhs is below the lowest z-coordinate of lhs
			dl = r.get_distance(lhs);
			dr = near - t.get_distance(rhs);
			if (dr <= dl)
			{
				return false;
			}

			// Check if rhs is closer to the bottom of the screen than lhs
			r = Parallelogram(m_ZeroPosition, dx, n);
			dl = r.get_distance(lhs);
			dr = r.get_distance(rhs);
			if (dr < dl)
				return true;
			else if (dl < dr)
				return false;

			// Check if rhs is closer to the left side of the screen than lhs
			r = Parallelogram(m_ZeroPosition, dy, n);
			dl = r.get_distance(lhs);
			dr = r.get_distance(rhs);
			if (dr < dl)
				return true;
			return false;
		}

		void DynamicAxonometricWorldCamera::__activate()
		{
			// Create a pixel-perfect orthogonal projection centered in the middle of the screen
			vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0)) / 2
			);

			Transform::projection.ortho(
				-halves.get(0), halves.get(0),
				-halves.get(1), halves.get(1),
				-halves.get(2), halves.get(2)
			);

			// A positive y-unit pushes the coordinate farther from the camera, and is squished by the cosine of the top view angle.
			// A positive z-unit pulls the coordinate closer to the camera and up by one positive y-unit.
			Transform::projection.get() *= TransformMatrix(
				1.f, 0.f, 0.f, 0.f,
				0.f, cos(m_TopViewAngle), 1.f, 0.f,
				0.f, 1.f, -1.f, 0.f
			);

			// Rotate the model space around the z-axis
			Transform::projection.rotatez(m_SideViewAngle);

			// Center the camera at its position in model space
			Transform::view.translate(-m_Position.get(0), -m_Position.get(1), -m_Position.get(2));
		}

		void DynamicAxonometricWorldCamera::__set_bounds()
		{
			reset_vectors();
		}

		void DynamicAxonometricWorldCamera::reset_vectors()
		{
			// Set the new radial vectors
			Float y_scrunch = cosf(m_TopViewAngle);

			Float c = cosf(m_SideViewAngle);
			Float s = sinf(m_SideViewAngle);

			auto c_func = c < 0 ? &floorf : &ceilf;
			auto s_func = s < 0 ? &floorf : &ceilf;

			Int dx = m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0);
			Int dy = m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0);

			m_Radii[0] = vec2i(c_func(c * dx), s_func(-s * dx));
			m_Radii[1] = vec2i(s_func(y_scrunch * s * dy), c_func(y_scrunch * c * dy));

			// Set the new normal vector
			Int n = m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0);
			Float z_scrunch = n / (y_scrunch + 1.f);
			m_Normal = vec3i(s_func(z_scrunch * s), c_func(z_scrunch * c), ceil(z_scrunch * y_scrunch));

			// Set the zero position
			const vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				0
			);
			m_ZeroPosition = m_Position - halves - (m_Normal * Frac(m_Position.get(2), m_Normal.get(2)));
		}

		void DynamicAxonometricWorldCamera::set_top(float angle)
		{
			m_TopViewAngle = angle;

			reset_vectors();
		}

		void DynamicAxonometricWorldCamera::rotate_top(float angle)
		{
			// TODO maybe do this more efficiently?
			m_TopViewAngle += angle;

			reset_vectors();
		}

		void DynamicAxonometricWorldCamera::set_side(float angle)
		{
			m_SideViewAngle = angle;

			// If active, change the angle of rotation
			if (is_active())
			{
				Transform::projection.rotatez(angle - m_SideViewAngle);
			}

			// Set the new radius vectors and normal vector
			reset_vectors();
		}

		void DynamicAxonometricWorldCamera::rotate_side(float angle)
		{
			m_SideViewAngle += angle;

			// If active, change the angle of rotation
			if (is_active())
			{
				Transform::projection.rotatez(angle);
			}

			// Set the new radius vectors and normal vector
			reset_vectors();
		}



	}
}