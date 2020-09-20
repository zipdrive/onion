#pragma once
#include "object.h"
#include "../event.h"

namespace onion
{
	namespace world
	{

		class Agent
		{

		};


		// An agent controlled by the player's inputs.
		class PlayerControlledAgent : public Agent, public KeyboardListener
		{
		protected:
			// The direction of the agent's movement.
			vec2i m_Direction;
		};

	}
}