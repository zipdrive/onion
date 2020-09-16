#pragma once
#include "graphics/frame.h"


namespace onion
{


	class State
	{
	public:
		virtual ~State();

		/// <summary>Displays the state.</summary>
		virtual void display() const = 0;
	};


	/// <summary>Retrieves the current state.</summary>
	/// <returns>The current state. NULL if no state is currently set.</returns>
	State* get_state();

	/// <summary>Deletes the previous state and sets a new state.</summary>
	/// <param name="state">The new state.</param>
	void set_state(State* state);


	/// <summary>A main function that displays the current state.</summary>
	void state_main();



}