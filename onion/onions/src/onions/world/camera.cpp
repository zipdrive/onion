#include "../../../include/onions/graphics/transform.h"
#include "../../../include/onions/world/camera.h"

namespace onion
{
	namespace world
	{

		WorldCamera::View::View(const mat2x3i& frame_bounds) : m_FrameBounds(frame_bounds) {}
		
		
		WorldCamera::WorldCamera(const mat2x3i& frame_bounds) : m_FrameBounds(frame_bounds) {}
		
		const vec3i& WorldCamera::get_position() const
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



		StaticTopDownWorldCamera::StaticTopDownView::StaticTopDownView(const mat2x3i& frame_bounds) : View(frame_bounds) {}
		
		vec3i StaticTopDownWorldCamera::StaticTopDownView::get_position() const
		{
			return m_Position;
		}

		void StaticTopDownWorldCamera::StaticTopDownView::set_position(const vec3i& pos)
		{
			const vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				0
			);

			m_Position = pos - halves - (get_normal() * pos.get(2));
		}

		void StaticTopDownWorldCamera::StaticTopDownView::translate(const vec3i& trans)
		{
			m_Position += vec2i(trans - (get_normal() * trans.get(2)));
		}

		vec3i StaticTopDownWorldCamera::StaticTopDownView::get_normal() const
		{
			static const vec3i normal(0, -1, 1);
			return normal;
		}

		vec3i StaticTopDownWorldCamera::StaticTopDownView::support(const vec3i& dir) const
		{
			const Int near = m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0);
			const vec3i n(0, -near, near);

			vec3i res;
			Int d_max = std::numeric_limits<Int>::min();

			for (int c = 7; c >= 0; --c)
			{
				vec3i p(m_Position, 0);

				if (c % 2 == 0)
					p(0) += m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0);
				if ((c / 2) % 2 == 0)
					p(1) += m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0);
				if (c / 4 == 0)
					p += n;

				Int d = p.dot(dir);
				if (d > d_max)
				{
					res = p;
					d_max = d;
				}
			}

			return res;
		}

		bool StaticTopDownWorldCamera::StaticTopDownView::compare(const Shape* lhs, const Shape* rhs) const
		{
			const Int near = m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0);
			const vec3i n(0, -near, near);
			const vec3i dx(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0), 0, 0);
			const vec3i dy(0, m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0), 0);

			// Check which object is closer to the screen
			Parallelogram rect(vec3i(m_Position, 0) + n, dx, dy);
			Int dl = rect.get_distance(lhs);
			Int dr = rect.get_distance(rhs);
			if (dr < dl)
				return true;
			else if (dl < dr)
				return false;

			// Check which object is closer to the bottom of the screen
			rect = Parallelogram(m_Position, dx, n);
			dl = rect.get_distance(lhs);
			dr = rect.get_distance(rhs);
			if (dr < dl)
				return true;
			else if (dl < dr)
				return false;

			// Check which object is closer to the left side of the screen
			rect = Parallelogram(m_Position, dy, n);
			dl = rect.get_distance(lhs);
			dr = rect.get_distance(rhs);
			if (dr < dl)
				return true;
			return false;

			/*

			// Check if the highest z-coordinate of lhs is below the lowest z-coordinate of rhs
			Parallelogram r(m_Position, dx, dy);
			Parallelogram t(vec3i(m_Position, 0) + n, dx, dy);
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
			r = Parallelogram(m_Position, dx, n);
			dl = r.get_distance(lhs);
			dr = r.get_distance(rhs);
			if (dr < dl)
				return true;
			else if (dl < dr)
				return false;

			// Check if rhs is closer to the left side of the screen than lhs
			r = Parallelogram(m_Position, dy, n);
			dl = r.get_distance(lhs);
			dr = r.get_distance(rhs);
			if (dr < dl)
				return true;
			return false;

			*/
		}
		
		
		StaticTopDownWorldCamera::StaticTopDownWorldCamera(const mat2x3i& frame_bounds) : WorldCamera(frame_bounds), m_View(frame_bounds) {}

		void StaticTopDownWorldCamera::translate(const vec3i& trans)
		{
			WorldCamera::translate(trans);
			m_View.translate(trans);
		}

		const WorldCamera::View* StaticTopDownWorldCamera::get_view() const
		{
			return &m_View;
		}

		void StaticTopDownWorldCamera::reset_view()
		{
			m_View.set_position(m_Position);
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


			// Reset the visible space
			reset_view();
		}




		DynamicAxonometricWorldCamera::DynamicAxonometricView::DynamicAxonometricView(const mat2x3i& frame_bounds) : View(frame_bounds) {}

		vec3i DynamicAxonometricWorldCamera::DynamicAxonometricView::get_position() const
		{
			return m_Position;
		}

		void DynamicAxonometricWorldCamera::DynamicAxonometricView::set_position(const vec3i& pos)
		{
			// TODO
			const vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				0
			);

			m_Position = pos - halves - (get_normal() * pos.get(2));
		}

		void DynamicAxonometricWorldCamera::DynamicAxonometricView::translate(const vec3i& trans)
		{
			m_Position += vec2i(trans - (get_normal() * trans.get(2)));
		}

		vec3i DynamicAxonometricWorldCamera::DynamicAxonometricView::get_normal() const
		{
			return m_Normal;
		}

		vec3i DynamicAxonometricWorldCamera::DynamicAxonometricView::support(const vec3i& dir) const
		{
			const vec3i n = m_Normal * Frac(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0), m_Normal.get(2));

			vec3i res;
			Int d_max = std::numeric_limits<Int>::min();

			for (int c = 7; c >= 0; --c)
			{
				vec3i p(m_Position, 0);

				if (c % 2 == 0)
					p += vec3i(m_Radii[0], 0);
				if ((c / 2) % 2 == 0)
					p += vec3i(m_Radii[1], 0);
				if (c / 4 == 0)
					p += m_Normal;

				Int d = p.dot(dir);
				if (d > d_max)
				{
					res = p;
					d_max = d;
				}
			}

			return res;
		}

		bool DynamicAxonometricWorldCamera::DynamicAxonometricView::compare(const Shape* lhs, const Shape* rhs) const
		{
			// TODO

			const Int near = m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0);
			const vec3i n(0, -near, near);
			const vec3i dx(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0), 0, 0);
			const vec3i dy(0, m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0), 0);

			// Check if the highest z-coordinate of lhs is below the lowest z-coordinate of rhs
			Parallelogram r(m_Position, dx, dy);
			Parallelogram t(vec3i(m_Position, 0) + n, dx, dy);
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
			r = Parallelogram(m_Position, dx, n);
			dl = r.get_distance(lhs);
			dr = r.get_distance(rhs);
			if (dr < dl)
				return true;
			else if (dl < dr)
				return false;

			// Check if rhs is closer to the left side of the screen than lhs
			r = Parallelogram(m_Position, dy, n);
			dl = r.get_distance(lhs);
			dr = r.get_distance(rhs);
			if (dr < dl)
				return true;
			return false;
		}


		DynamicAxonometricWorldCamera::DynamicAxonometricWorldCamera(const mat2x3i& frame_bounds, float top_view_angle, float side_view_angle) : m_TopViewAngle(top_view_angle), m_SideViewAngle(side_view_angle), m_View(frame_bounds), WorldCamera(frame_bounds) {}

		void DynamicAxonometricWorldCamera::reset_view()
		{
			// TODO
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


			// Reset the visible space
			reset_view();
		}

		void DynamicAxonometricWorldCamera::set_top(float angle)
		{
			m_TopViewAngle = angle;
		}

		void DynamicAxonometricWorldCamera::rotate_top(float angle)
		{
			// TODO maybe do this more efficiently?
			m_TopViewAngle += angle;
		}

		void DynamicAxonometricWorldCamera::set_side(float angle)
		{
			m_SideViewAngle = angle;
		}

		void DynamicAxonometricWorldCamera::rotate_side(float angle)
		{
			m_SideViewAngle += angle;
		}

		void DynamicAxonometricWorldCamera::get_view(vec2i& direction) const
		{
			float c = cos(m_SideViewAngle);
			float s = sin(m_SideViewAngle);
			direction = vec2i(
				s == 0.f ? 0 : (s > 0.f ? -1 : 1),
				c == 0.f ? 0 : (c > 0.f ? -1 : 1)
			);
		}



	}
}