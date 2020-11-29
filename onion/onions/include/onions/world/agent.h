#pragma once
#include "object.h"
#include "../event.h"

namespace onion
{
	namespace world
	{

		// Controls the actions of an actor.
		class Agent
		{
		public:
			/// <summary>Updates the actor.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			/// <param name="frames_passed">The number of frames since the last update.</param>
			/// <returns>The desired translation of the actor, in subpixel units.</returns>
			virtual vec3i update(const WorldCamera* view, int frames_passed) = 0;
		};

		// An object that can move around and do stuff.
		class Actor : public Object
		{
		protected:
			// The agent that controls the actor movement.
			Agent* m_Agent;

			// Wrapper for the shape that handles subpixel translations.
			SubpixelHandler m_SubpixelHandler;

		public:
			/// <summary>Constructs an actor object.</summary>
			/// <param name="bounds">The bounds of the object. Should be constructed with new specifically for this object.</param>
			/// <param name="agent">The agent that controls the object. Should be constructed with new specifically for this object.</param>
			/// <param name="graphic">The graphic used to display the object. Should be constructed with new specifically for this object.</param>
			Actor(Shape* bounds, Agent* agent, Graphic3D* graphic);

			/// <summary>Deletes the agent.</summary>
			virtual ~Actor();


			/// <summary>Translates the actor by a number of subpixel units.</summary>
			/// <param name="trans">The vector of translation, in subpixel units.</returns>
			void translate(const vec3i& trans);
			
			
			/// <summary>Updates the actor.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			/// <param name="frames_passed">The number of frames since the last update.</param>
			/// <returns>The desired translation of the actor, in subpixel units.</returns>
			virtual vec3i update(const WorldCamera* view, int frames_passed);
		};


		// An agent whose movement is controlled by the player's inputs.
		class PlayerMovementControlledAgent : public Agent, public KeyboardListener
		{
		protected:
			// The direction of the agent's movement.
			vec2i m_Direction;

			// The movement speed of the actor.
			Int m_MovementSpeed;

		public:
			/// <summary>Registers the directional keyboard controls.</summary>
			PlayerMovementControlledAgent(Int movement_speed);


			/// <summary>Moves the actor.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			/// <param name="frames_passed">The number of frames since the last update.</param>
			virtual vec3i update(const WorldCamera* view, int frames_passed);

			/// <summary>Responds to a keyboard control being pressed.</summary>
			/// <param name="event_data">The data for the event.</param>
			virtual int trigger(const KeyEvent& event_data);
		};

	}
}