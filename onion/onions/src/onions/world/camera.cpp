#include "../../../include/onions/world/camera.h"

namespace onion
{
	namespace world
	{

		Camera::Camera(const mat3x2i& frame_bounds) : m_FrameBounds(frame_bounds) 
		{
			reset();
		}

		Camera::~Camera() {}
		
		const TransformMatrix& Camera::get_transform() const
		{
			return m_Transform;
		}
		
		const vec3i& Camera::get_position() const
		{
			return m_Position;
		}
		
		void Camera::set_position(const vec3i& position)
		{
			translate(position - m_Position);
		}
		
		void Camera::translate(const vec3i& trans)
		{
			m_Transform.translate(-trans.get(0), -trans.get(1), -trans.get(2));

			m_Position += trans;
		}



		StaticTopDownCamera::StaticTopDownCamera(const mat3x2i& frame_bounds) : Camera(frame_bounds) {}
		
		void StaticTopDownCamera::reset()
		{
			// Reset the transformation matrix
			m_Transform.identity();

			// Translate the camera to the center of the frame
			m_Transform.translate(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0)) / 2
			);

			// Scale the camera view
			float ppu = 0.5f / UNITS_PER_PIXEL;
			m_Transform.scale(ppu, ppu, ppu);

			// A positive y-unit pushes the coordinate farther from the camera.
			// A positive z-unit pulls the coordinate closer to the camera and up by one positive y-unit.
			m_Transform *= TransformMatrix(
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 1.f, 0.f,
				0.f, 1.f, -1.f, 0.f
			);

			// Center the camera at its position in model space
			m_Transform.translate(-m_Position.get(0), -m_Position.get(1), -m_Position.get(2));
		}

		void StaticTopDownCamera::get_view(vec2i& direction) const
		{
			direction = vec2i(0, -1);
		}


		DynamicAxonometricCamera::DynamicAxonometricCamera(const mat3x2i& frame_bounds, float top_view_angle, float side_view_angle) : m_TopViewAngle(top_view_angle), m_SideViewAngle(side_view_angle), Camera(frame_bounds) {}

		void DynamicAxonometricCamera::reset()
		{
			// Reset the identity matrix
			m_Transform.identity();

			// Translate the camera to the center of the frame
			m_Transform.translate(
				(m_FrameBounds.get(0, 1) - m_FrameBounds.get(0, 0)) / 2,
				(m_FrameBounds.get(1, 1) - m_FrameBounds.get(1, 0)) / 2,
				(m_FrameBounds.get(2, 1) - m_FrameBounds.get(2, 0)) / 2
			);

			// Scale the view
			float ppu = 0.5f / UNITS_PER_PIXEL;
			m_Transform.scale(ppu, ppu, ppu);

			// A positive y-unit pushes the coordinate farther from the camera, and is squished by the cosine of the top view angle.
			// A positive z-unit pulls the coordinate closer to the camera and up by one positive y-unit.
			m_Transform *= TransformMatrix(
				1.f, 0.f, 0.f, 0.f,
				0.f, cos(m_TopViewAngle), 1.f, 0.f,
				0.f, 1.f, -1.f, 0.f
			);

			// Rotate the model space around the z-axis
			m_Transform.rotatez(m_SideViewAngle);

			// Center the camera at its position in model space
			m_Transform.translate(-m_Position.get(0), -m_Position.get(1), -m_Position.get(2));
		}

		void DynamicAxonometricCamera::set_top(float angle)
		{
			m_TopViewAngle = angle;
			reset();
		}

		void DynamicAxonometricCamera::rotate_top(float angle)
		{
			// TODO maybe do this more efficiently?
			m_TopViewAngle += angle;
			reset();
		}

		void DynamicAxonometricCamera::set_side(float angle)
		{
			m_Transform.translate(m_Position.get(0), m_Position.get(1), m_Position.get(2));
			m_Transform.rotatez(angle - m_SideViewAngle);
			m_Transform.translate(-m_Position.get(0), -m_Position.get(1), -m_Position.get(2));

			m_SideViewAngle = angle;
		}

		void DynamicAxonometricCamera::rotate_side(float angle)
		{
			m_Transform.translate(m_Position.get(0), m_Position.get(1), m_Position.get(2));
			m_Transform.rotatez(angle);
			m_Transform.translate(-m_Position.get(0), -m_Position.get(1), -m_Position.get(2));

			m_SideViewAngle += angle;
		}

		void DynamicAxonometricCamera::get_view(vec2i& direction) const
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