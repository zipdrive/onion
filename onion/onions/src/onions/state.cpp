#include "../../include/onion.h"

using namespace onion;

State::~State() {}


State* g_State = nullptr;


State* onion::get_state()
{
	return g_State;
}

void onion::set_state(State* state)
{
	if (g_State)
		delete g_State;
	g_State = state;
}


void onion_state_display_func()
{
	if (g_State)
		g_State->display();
}

void onion::state_main()
{
	main(onion_state_display_func);
}