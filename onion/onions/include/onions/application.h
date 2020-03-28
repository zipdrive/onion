#pragma once

#include <string>


// Contains all settings for the application window.
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
Application*& get_application_settings();

/// <summary>Sets the main Application object.</summary>
/// <param name="app">The Application object to set.</param>
void set_application_settings(Application* app);


typedef void(*onion_display_func)(void);
typedef void(*onion_event_func)(void);


/// <summary>Initializes the Onion library.</summary>
/// <param name="settings_file">The path to the settings file.</param>
/// <returns>1 if an error was encountered, 0 otherwise.</returns>
int onion_init(const char* settings_file);

/// <summary>Runs the main loop of the Onion library.</summary>
/// <param name="display_callback">The callback function for displaying.</param>
void onion_main(onion_display_func display_callback);