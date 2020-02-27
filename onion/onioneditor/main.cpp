#include "editor.h"


WorldOrthographicFrame* g_WorldFrame = nullptr;


void display_editor()
{
	g_WorldFrame->display();
}

int main()
{
	// Initialize Onion
	if (onion_init("settings_editor.ini") != 0)
		return 1;

	// Initialize stuff
	g_WorldFrame = new WorldOrthographicFrame(0, 24, 640, 352, 0);

	// Run the core loop
	onion_main(display_editor);
	return 0;
}