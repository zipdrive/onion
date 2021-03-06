#include <algorithm>
#include "../../include/onion.h"

namespace onion
{

	void State::set_bounds(int width, int height) {}


	State* g_State = nullptr;


	State* get_state()
	{
		return g_State;
	}

	void set_state(State* state)
	{
		if (g_State)
			delete g_State;
		g_State = state;

		Application* app = get_application_settings();
		g_State->set_bounds(app->width, app->height);
	}


	void state_display_func()
	{
		if (g_State)
			g_State->display();
	}

	void state_main()
	{
		main(state_display_func);
	}

}