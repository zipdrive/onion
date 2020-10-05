#pragma once
#include "graphics/frame.h"
#include "application.h"


namespace onion
{


	class State
	{
	public:
		/// <summary>Virtual deconstructor.</summary>
		virtual ~State() = default;

		/// <summary>Is called when the state is made or when the window is resized.</summary>
		/// <param name="width">The width of the window.</param>
		/// <param name="height">The height of the window.</param>
		virtual void set_bounds(int width, int height);

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