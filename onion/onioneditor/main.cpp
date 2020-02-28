#include "editor.h"


Editor* g_Editor = nullptr;

WorldOrthographicFrame* g_WorldFrame = nullptr;


void display_editor()
{
	g_Editor->display();
	//g_WorldFrame->display();
}

int main()
{
	// Initialize Onion
	if (onion_init("settings_editor.ini") != 0)
		return 1;

	// Initialize stuff
	g_Editor = new ChunkEditor();
	//g_WorldFrame = new WorldOrthographicFrame(0, 0, 640, 360);

	// Run the core loop
	onion_main(display_editor);
	return 0;
}