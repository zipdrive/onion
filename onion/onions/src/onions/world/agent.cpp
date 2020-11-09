#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../../include/onions/world/agent.h"

namespace onion
{
	namespace world
	{
		
		SubpixelHandler::SubpixelHandler(Shape* shape) : m_Subpixels(0, 0, 0)
		{
			m_Shape = shape;
		}

		void SubpixelHandler::translate(const vec3i& trans)
		{
			m_Subpixels += trans;

			// This might give inaccurate results if the C++ implementation doesn't truncate division of negative integers towards 0
			vec3i t = trans / num_subpixels;
			m_Subpixels -= t * num_subpixels;

			m_Shape->translate(t);
		}


		
		Actor::Actor(Shape* bounds, Agent* agent, Graphic3D* graphic) : Object(bounds, graphic), m_SubpixelHandler(bounds)
		{
			m_Agent = agent;
		}

		Actor::~Actor()
		{
			if (m_Agent)
				delete m_Agent;
		}

		SubpixelHandler& Actor::get_translator()
		{
			return m_SubpixelHandler;
		}

		vec3i Actor::update(const WorldCamera::View* view, int frames_passed)
		{
			return m_Agent->update(view, frames_passed);
		}


		PlayerMovementControlledAgent::PlayerMovementControlledAgent(Int movement_speed)
		{
			m_Direction = vec2i(0, 0);
			m_MovementSpeed = movement_speed;

			unfreeze(INT_MIN);
		}
		
		vec3i PlayerMovementControlledAgent::update(const WorldCamera::View* view, int frames_passed)
		{
			// TODO

			vec2i d = view->get_normal();
			Int d_len = d.square_sum();
			vec2i t = (d * d) 
				* (m_MovementSpeed * m_MovementSpeed * frames_passed * frames_passed) 
				/ (d_len * UpdateEvent::frames_per_second * UpdateEvent::frames_per_second);
			for (int k = 0; k < 2; ++k)
				d(k) = (d.get(k) < 0 ? -1 : 1) * (Int)round(sqrt(t.get(k)));

			return vec3i(
				(d.get(1) * m_Direction.get(0)) + (d.get(0) * m_Direction.get(1)),
				(d.get(1) * m_Direction.get(1)) - (d.get(0) * m_Direction.get(0)),
				0
			);
		}

		int PlayerMovementControlledAgent::trigger(const KeyEvent& event_data)
		{
			if (event_data.control == ONION_KEY_LEFT)
			{
				m_Direction(0) += (event_data.pressed ? -1 : 1);
			}
			else if (event_data.control == ONION_KEY_RIGHT)
			{
				m_Direction(0) += (event_data.pressed ? 1 : -1);
			}
			else if (event_data.control == ONION_KEY_DOWN)
			{
				m_Direction(1) += (event_data.pressed ? -1 : 1);
			}
			else if (event_data.control == ONION_KEY_UP)
			{
				m_Direction(1) += (event_data.pressed ? 1 : -1);
			}
			else
			{
				return EVENT_CONTINUE;
			}

			return EVENT_STOP;
		}

	}
}