#include <algorithm>
#include "../../include/onion.h"

namespace onion
{

	State::~State() {}

	void State::display() const
	{
		static Application*& app = get_application_settings();

		int depth = std::max(app->width, app->height);

		MatrixStack& p = projection();
		p.reset();
		p.ortho(0, app->width, 0, app->height, -depth, depth);

		__display();
	}


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