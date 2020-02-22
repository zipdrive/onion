#pragma once
#include <string>


// Contains all settings for the application.
struct Application
{
	// The title of the application window.
	std::string title;

	// The width of the application window.
	int width;

	// The height of the application window.
	int height;

	// True if the application window should be fullscreen.
	bool fullscreen;


	/// <summary>Initializes the Application object.</summary>
	Application();

	/// <summary>Initializes the Application object.</summary>
	/// <param name="other">The Application object to copy settings from.</param>
	Application(Application* other);

	/// <summary>Displays the Application object.</summary>
	/// <returns>1 if the window displayed unsuccessfully, 0 otherwise.</returns>
	int display();
};

/// <summary>Gets the main Application object.</summary>
/// <returns>The main Application object.</returns>
Application* get_application_settings();

/// <summary>Sets the main Application object.</summary>
/// <param name="app">The Application object to set.</param>
void set_application_settings(Application* app);



// Contains some sort of content for the game.
class Layer
{
public:
	/// <summary>Displays the layer to the screen.</summary>
	virtual void display();
};