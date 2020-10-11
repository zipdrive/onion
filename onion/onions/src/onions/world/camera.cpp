#include "../../../include/onions/graphics/transform.h"
#include "../../../include/onions/world/camera.h"

namespace onion
{
	namespace world
	{

		bool WorldCamera::View::is_visible(const vec3i& pos) const
		{
			for (int k = 3; k >= 0; --k)
				if (edges[k].normal.dot(pos) < edges[k].dot)
					return false;
			return true;
		}


		WorldCamera::WorldCamera(const mat2x3i& frame_bounds) : m_FrameBounds(frame_bounds) {}
		
		const vec3i& WorldCamera::get_position() const
		{
			return m_Position;
		}

		const WorldCamera::View& WorldCamera::get_view() const
		{
			return m_View;
		}
		
		void WorldCamera::set_position(const vec3i& position)
		{
			translate(position - m_Position);
		}
		
		void WorldCamera::translate(const vec3i& trans)
		{
			vec3i prior = m_Position;
			vec3i current = prior + trans;
			for (int k = 2; k >= 0; --k)
			{
				prior(k) /= UNITS_PER_PIXEL;
				current(k) /= UNITS_PER_PIXEL;
			}

			vec3i diff = current - prior;
			if (diff.square_sum() > 0 && is_active())
			{
				Transform::view.translate(-diff.get(0), -diff.get(1), -diff.get(2));
				Transform::set_view();
			}

			m_Position += trans;
		}



		StaticTopDownWorldCamera::StaticTopDownWorldCamera(const mat2x3i& frame_bounds) : WorldCamera(frame_bounds) {}

		void StaticTopDownWorldCamera::reset_view()
		{
			vec3i halves(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0)) / 2
			);

			// Construct a raycast through the center of the screen
			m_View.center.origin = UNITS_PER_PIXEL * m_Position;
			m_View.center.direction = vec3i(0, -1, 1);

			// Construct a position for the bottom-left and top-right corners
			vec3i bottom_left = m_Position - halves;
			bottom_left(2) = m_Position.get(2);
			vec3i top_right = m_Position + halves;
			top_right(2) = m_Position.get(2);

			// Construct the plane for the left edge
			m_View.edges[LEFT_VIEW_EDGE].normal = vec3i(1, 0, 0);
			m_View.edges[LEFT_VIEW_EDGE].dot = UNITS_PER_PIXEL * m_View.edges[LEFT_VIEW_EDGE].normal.dot(bottom_left);

			// Construct the plane for the right edge
			m_View.edges[RIGHT_VIEW_EDGE].normal = vec3i(-1, 0, 0);
			m_View.edges[RIGHT_VIEW_EDGE].dot = UNITS_PER_PIXEL * m_View.edges[RIGHT_VIEW_EDGE].normal.dot(top_right);

			// Construct the plane for the top edge
			m_View.edges[TOP_VIEW_EDGE].normal = vec3i(0, -1, -1);
			m_View.edges[TOP_VIEW_EDGE].dot = UNITS_PER_PIXEL * m_View.edges[TOP_VIEW_EDGE].normal.dot(top_right);

			// Construct the plane for the bottom edge
			m_View.edges[BOTTOM_VIEW_EDGE].normal = vec3i(0, 1, 1);
			m_View.edges[BOTTOM_VIEW_EDGE].dot = UNITS_PER_PIXEL * m_View.edges[BOTTOM_VIEW_EDGE].normal.dot(bottom_left);
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
				0.f, 1.f, -1.f, 0.f
			);
			
			// Center the camera at its position in model space
			Transform::view.translate(-m_Position.get(0), -m_Position.get(1), -m_Position.get(2));


			// Reset the visible space
			reset_view();
		}

		void StaticTopDownWorldCamera::get_view(vec2i& direction) const
		{
			direction = vec2i(0, -1);
		}


		DynamicAxonometricWorldCamera::DynamicAxonometricWorldCamera(const mat2x3i& frame_bounds, float top_view_angle, float side_view_angle) : m_TopViewAngle(top_view_angle), m_SideViewAngle(side_view_angle), WorldCamera(frame_bounds) {}

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