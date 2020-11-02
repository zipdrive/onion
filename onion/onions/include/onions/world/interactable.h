#pragma once
#include "object.h"
#include "../event.h"

namespace onion
{
	namespace world
	{

		class _Interactable
		{
		protected:
			// The object that interacts with this object.
			static Object* m_Interactor;

		public:
			/// <summary>Sets the object that interacts with interactable objects.</summary>
			/// <param name="obj">The new interactor object.</param>
			static void set_interactor(Object* obj);

			/// <summary>Triggers the interaction.</summary>
			/// <returns>True if there was an interaction, false if there was no interaction.</returns>
			virtual bool interact() = 0;
		};

		// An object that can be interacted with.
		template <typename T>
		class _InteractableObject : public _Interactable, public T, public KeyboardListener
		{
		protected:
			// The maximum (squared) radius that the interactor can be from the object and still interact with it.
			Int m_MaximumRadiusSquared;

		public:
			/// <summary>Constructs an interactable object.</summary>
			/// <param name="maximum_radius">The maximum distance that the interactor can be from this object and still interact with it.</param>
			/// <param name="args">The arguments passed to the parent class.</param>
			template <typename... _Args>
			_InteractableObject(Int maximum_radius, Int priority, const _Args&... args) : T(args...)
			{
				m_MaximumRadiusSquared = maximum_radius * maximum_radius;
				unfreeze(priority);
			}

			/// <summary>Triggers an interaction if ONION_KEY_SELECT was pressed and the interactor object is within a certain distance from this object.</summary>
			/// <param name="event_data">The data for the key event.</param>
			/// <returns>EVENT_STOP if the object was successfully interacted with, EVENT_CONTINUE otherwise.</returns>
			virtual int trigger(const KeyEvent& event_data)
			{
				if (event_data.control == ONION_KEY_SELECT && event_data.pressed)
				{
					if (m_Interactor)
					{
						// Calculate the (squared) distance between the interactor and this object
						const Shape* interactor_bounds = m_Interactor->get_bounds();

						vec3i pos[2];
						interactor_bounds->get_closest_point(m_Bounds->get_position(), pos[0]);
						m_Bounds->get_closest_point(interactor_bounds->get_position(), pos[1]);

						vec3i closest[3];
						closest[0] = pos[0] - m_Bounds->get_position();
						closest[1] = pos[1] - interactor_bounds->get_position();
						closest[2] = pos[0] - pos[1];

						// TODO this is just a heuristic, do this but better?
						Int dist_sq = std::min(std::min(closest[0].square_sum(), closest[1].square_sum()), closest[2].square_sum());

						// Check that the distance is less than the maximum distance possible to still interact from
						if (dist_sq <= m_MaximumRadiusSquared)
						{
							// Interact with the object
							return interact() ? EVENT_STOP : EVENT_CONTINUE;
						}
					}
				}

				return EVENT_CONTINUE;
			}
		};



	}
}