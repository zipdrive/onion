#include "../../../include/onions/graphics/transform.h"
#include "../../../include/onions/world/camera.h"

namespace onion
{
	namespace world
	{

		WorldCamera::WorldCamera(const mat2x3i& frame_bounds) : m_FrameBounds(frame_bounds) {}
		
		const vec3i& WorldCamera::get_position() const
		{
			return m_Position;
		}
		
		void WorldCamera::set_position(const vec3i& position)
		{
			m_Position = position;
			
			if (is_active())
			{
				Transform::view.identity();
				Transform::view.translate(-position.get(0), -position.get(1), -position.get(2));
				Transform::set_view();
			}
		}
		
		void WorldCamera::translate(const vec3i& trans)
		{
			m_Position += trans;

			if (is_active())
			{
				Transform::view.translate(-trans.get(0), -trans.get(1), -trans.get(2));
				Transform::set_view();
			}
		}



		StaticTopDownWorldCamera::StaticTopDownWorldCamera(const mat2x3i& frame_bounds) : WorldCamera(frame_bounds) {}
		
		void StaticTopDownWorldCamera::__activate() const
		{
			// Scale the camera view
			float ppu = 1.f / UNITS_PER_PIXEL;
			Transform::projection.scale(ppu, ppu, ppu);

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
		}

		void StaticTopDownWorldCamera::get_view(vec2i& direction) const
		{
			direction = vec2i(0, -1);
		}


		DynamicAxonometricWorldCamera::DynamicAxonometricWorldCamera(const mat2x3i& frame_bounds, float top_view_angle, float side_view_angle) : m_TopViewAngle(top_view_angle), m_SideViewAngle(side_view_angle), WorldCamera(frame_bounds) {}

		void DynamicAxonometricWorldCamera::__activate() const
		{
			// Translate the camera to the center of the frame
			Transform::projection.translate(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0)) / 2
			);

			// Scale the view
			float ppu = 0.5f / UNITS_PER_PIXEL;
			Transform::projection.scale(ppu, ppu, ppu);

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