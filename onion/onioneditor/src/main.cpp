#include <iostream>
#include <list>
#include "../include/dialog.h"
#include "../include/editorchunk.h"
#include "../include/editorsprite.h"


mat4x4f g_UITransform;

float g_EditorHeight;
Graphic* g_EditorTabsBackground;

Editor* g_Editor = nullptr;
ChunksEditor* g_ChunksEditor = nullptr;
SpriteSheetsEditor* g_SpriteSheetsEditor = nullptr;


std::list<Dialog*> g_Dialogs;

void push_dialog(Dialog* dialog)
{
	if (g_Dialogs.empty())
		g_Editor->freeze();
	else 
		g_Dialogs.front()->freeze();

	g_Dialogs.push_front(dialog);
	dialog->unfreeze();
}

Dialog* pop_dialog()
{
	if (g_Dialogs.empty()) return nullptr;

	Dialog* dialog = g_Dialogs.front();
	g_Dialogs.pop_front();

	if (g_Dialogs.empty())
		g_Editor->unfreeze();
	else
		g_Dialogs.front()->unfreeze();

	return dialog;
}


void display_editor()
{
	// Set up the transformation
	mat_push();
	mat_custom_transform(g_UITransform);

	// Display the editor
	g_Editor->display();

	// Display the dialogs
	for (auto iter = g_Dialogs.rbegin(); iter != g_Dialogs.rend(); ++iter)
	{
		(*iter)->display();
	}

	// Display the editor tabs
	mat_translate(0.f, g_EditorHeight, 0.f);
	g_EditorTabsBackground->display();

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
	g_ChunksEditor = new ChunksEditor();
	g_SpriteSheetsEditor = new SpriteSheetsEditor();

	g_Editor = g_SpriteSheetsEditor;
	g_Editor->unfreeze();

	g_EditorHeight = app->height - 24;

	// Initialize editor tabs
	g_EditorTabsBackground = SolidColorGraphic::generate(183, 227, 244, 255, app->width, 24);

	// Run the core loop
	onion_main(display_editor);

	// Clean up
	delete g_ChunksEditor;
	return 0;
}