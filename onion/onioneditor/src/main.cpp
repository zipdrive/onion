#include "../include/editorchunk.h"


mat4x4f g_UITransform;

float g_EditorHeight;
Graphic* g_EditorTabsBackground;

Editor* g_Editor = nullptr;
ChunkEditor* g_ChunkEditor = nullptr;


void display_editor()
{
	// Set up the transformation
	mat_push();
	mat_custom_transform(g_UITransform);

	// Display the editor
	//g_Editor->display();

	// Display the editor tabs
	mat_translate(0.f, g_EditorHeight, 0.f);
	//g_EditorTabsBackground->display();

	get_gui_font()->display_line("FUCK_YOU.PNG", generate_palette_matrix(255, 255, 255, 255, 255, 255, 255, 96, 0, 0, 0, 0, 0, 0, 0, 0));
	
	// Clean up the transformation
	mat_pop();
}

int main()
{
	// Initialize Onion
	if (onion_init("settings_editor.ini") != 0)
		return 1;

	// Construct the UI transform
	Application*& app = get_application_settings();

	g_UITransform.set(0, 0, 2.f / app->width);
	g_UITransform.set(1, 1, 2.f / app->height);
	g_UITransform.set(2, 2, 0.5f);
	g_UITransform.set(0, 3, -1.f);
	g_UITransform.set(1, 3, -1.f);
	g_UITransform.set(2, 3, -0.5f);

	// Initialize data
	Data::load();

	// Initialize editors
	//g_ChunkEditor = new ChunkEditor();

	//g_Editor = g_ChunkEditor;
	//g_Editor->unfreeze();

	g_EditorHeight = app->height - 24;

	// Initialize editor tabs
	g_EditorTabsBackground = SolidColorGraphic::generate(183, 227, 244, 255, app->width, 24);

	// Run the core loop
	onion_main(display_editor);

	// Clean up
	delete g_ChunkEditor;
	return 0;
}