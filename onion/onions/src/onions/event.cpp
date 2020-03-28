#include <iostream>
#include <fstream>
#include <regex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../include/onions/application.h"
#include "../../include/onions/event.h"


using namespace std;


// A stack of event listeners to call in order.
template <typename EventType>
class StackEventListener : public EventListener<EventType>
{
private:
	// The stack of event listeners to call.
	std::vector<EventListener<EventType>*> m_Stack;

public:
	/// <summary>Pushes a listener on top of the stack.</summary>
	/// <param name="listener">The listener to add to the stack.</param>
	void push(EventListener<EventType>* listener)
	{
		m_Stack.push_back(listener);
	}

	/// <summary>Pops the listener on top of the stack.</summary>
	void pop()
	{
		if (!m_Stack.empty())
		{
			m_Stack.pop_back();
		}
	}

	/// <summary>Responds to an event.</summary>
	/// <param name="event_data">The data for the event.</param>
	virtual int trigger(const EventType& event_data)
	{
		for (auto iter = m_Stack.rbegin(); iter != m_Stack.rend(); ++iter)
		{
			if ((*iter)->trigger(event_data) == EVENT_STOP)
			{
				return EVENT_STOP;
			}
		}

		return EVENT_CONTINUE;
	}
};

typedef StackEventListener<MouseMoveEvent> StackMouseMoveListener;
typedef StackEventListener<MousePressEvent> StackMousePressListener;
typedef StackEventListener<MouseReleaseEvent> StackMouseReleaseListener;


// A listener that keeps track of the mouse state.
class StackMouseListener : public StackMouseMoveListener, public StackMousePressListener, public StackMouseReleaseListener
{
private:
	// Whether the listener is frozen or not.
	bool m_Frozen = false;

public:
	// The x-coordinate of the mouse.
	int x;

	// The y-coordinate of the mouse.
	int y;

	/// <summary>Triggers against a mouse move event.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MouseMoveEvent& event_data)
	{
		x = event_data.x;
		y = event_data.y;

		if (m_Frozen) return EVENT_CONTINUE;
		return StackMouseMoveListener::trigger(event_data);
	}

	/// <summary>Triggers against a mouse press event.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MousePressEvent& event_data)
	{
		if (m_Frozen) return EVENT_CONTINUE;
		return StackMousePressListener::trigger(event_data);
	}

	/// <summary>Triggers against a mouse release event.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MouseReleaseEvent& event_data)
	{
		if (m_Frozen) return EVENT_CONTINUE;
		return StackMouseReleaseListener::trigger(event_data);
	}


	void freeze()
	{
		m_Frozen = true;
	}

	void unfreeze()
	{
		m_Frozen = false;
	}


	void push(MouseMoveListener* listener)
	{
		StackMouseMoveListener::push(listener);
	}

	void push(MousePressListener* listener)
	{
		StackMousePressListener::push(listener);
	}

	void push(MouseReleaseListener* listener)
	{
		StackMouseReleaseListener::push(listener);
	}


	void pop_move_listener()
	{
		StackMouseMoveListener::pop();
	}

	void pop_press_listener()
	{
		StackMousePressListener::pop();
	}

	void pop_release_listener()
	{
		StackMouseReleaseListener::pop();
	}

} g_MouseListener;


void push_mouse_move_listener(MouseMoveListener* listener)
{
	g_MouseListener.push(listener);
}

void pop_mouse_move_listener()
{
	g_MouseListener.pop_move_listener();
}

void push_mouse_press_listener(MousePressListener* listener)
{
	g_MouseListener.push(listener);
}

void pop_mouse_press_listener()
{
	g_MouseListener.pop_press_listener();
}

void push_mouse_release_listener(MouseReleaseListener* listener)
{
	g_MouseListener.push(listener);
}

void pop_mouse_release_listener()
{
	g_MouseListener.pop_release_listener();
}


void MouseMoveListener::freeze()
{
	//g_MouseListener.pop_move_listener(this);
}

void MouseMoveListener::unfreeze()
{
	g_MouseListener.push(this);
}


void MousePressListener::freeze()
{
	//
}

void MousePressListener::unfreeze()
{
	g_MouseListener.push(this);
}


void MouseReleaseListener::freeze()
{
	//
}

void MouseReleaseListener::unfreeze()
{
	g_MouseListener.push(this);
}


MouseDraggableListener* MouseDraggableListener::dragged{ nullptr };

int MouseDraggableListener::trigger(const MouseReleaseEvent& event_data)
{
	if (dragged == this)
	{
		dragged = nullptr;
		return EVENT_STOP;
	}

	return EVENT_CONTINUE;
}

void MouseDraggableListener::freeze()
{
	MousePressListener::freeze();
	MouseMoveListener::freeze();
	MouseReleaseListener::freeze();
}

void MouseDraggableListener::unfreeze()
{
	MousePressListener::unfreeze();
	MouseMoveListener::unfreeze();
	MouseReleaseListener::unfreeze();
}



// The main Application object.
Application* g_Application = NULL;

// The settings file.
const char* g_SettingsFile = NULL;

// The application window.
GLFWwindow* g_Window = NULL;


void load_settings()
{
	g_Application = new Application();

	string line;
	ifstream settings(g_SettingsFile);

	if (settings.is_open())
	{
		regex eqmatcher("(\\S+)\\s*=\\s*(.*\\S)\\s*(//.*)?");
		smatch m;

		while (getline(settings, line))
		{
			if (regex_match(line, m, eqmatcher))
			{
				if (m[1].compare("title") == 0)
				{
					g_Application->title = m[2].str();
				}
				else if (m[1].compare("width") == 0)
				{
					g_Application->width = stoi(m[2].str());
				}
				else if (m[1].compare("height") == 0)
				{
					g_Application->height = stoi(m[2].str());
				}
				else if (m[1].compare("fullscreen") == 0)
				{
					g_Application->fullscreen = (m[2].compare("true") == 0);
				}
			}
		}

		settings.close();
	}
}

void save_settings()
{
	ofstream settings(g_SettingsFile, ios::out | ios::trunc);

	if (settings.is_open())
	{
		settings << "title = " << g_Application->title;
		settings << "\nwidth = " << g_Application->width;
		settings << "\nheight = " << g_Application->height;
		settings << "\nfullscreen = " << (g_Application->fullscreen ? "true" : "false");

		settings.close();
	}
}


Application*& get_application_settings()
{
	if (!g_Application)
	{
		load_settings();
	}

	return g_Application;
}

void set_application_settings(Application* app)
{
	g_Application = app;
	g_Application->display();

	save_settings();
}

Application::Application() : title("NO_TITLE_SET")
{
	width = 640;
	height = 400;
	fullscreen = false;
}

Application::Application(Application* other) : title(other->title)
{
	width = other->width;
	height = other->height;
	fullscreen = other->fullscreen;
}

int Application::display()
{
	if (!g_Window)
	{
		// If the application window has not been initialized, initialize it.
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		g_Window = glfwCreateWindow(width, height, title.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
		if (!g_Window)
		{
			glfwTerminate();
			return 1;
		}
	}
	else
	{
		// If the application window has been initialized, alter its size.
		if (fullscreen)
		{
			// TODO fix fullscreen
			glfwSetWindowMonitor(g_Window, glfwGetPrimaryMonitor(), 0, 0, width, height, GLFW_DONT_CARE);
		}
		else
		{
			glfwSetWindowMonitor(g_Window, NULL, 100, 100, width, height, GLFW_DONT_CARE);
		}
	}

	// Set the viewport
	glViewport(0, 0, width, height);
	return 0;
}



/// <summary>The callback function for when a physical key is pressed, released, or repeated.</summary>
/// <param name="window">The window that the event triggered from.</param>
/// <param name="key">The keyboard key that triggered the event.</param>
/// <param name="scancode">The scancode of the key.</param>
/// <param name="action">Whether the key was pressed, released, or repeated.</param>
/// <param name="mods">Bit field of which modifier keys were held down.</param>
void onion_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

/// <summary>The callback function for when the mouse moves.</summary>
/// <param name="window">The window that the event triggered from.</param>
/// <param name="xpos">The x-coordinate of the mouse cursor.</param>
/// <param name="ypos">The y-coordinate of the mouse cursor.</param>
void onion_mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	// Construct the data object
	MouseMoveEvent event_data = { round(xpos), g_Application->height - (int)round(ypos) };

	// Trigger the global listener
	g_MouseListener.trigger(event_data);
}

/// <summary>The callback function for when a mouse button is pressed or released.</summary>
/// <param name="window">The window that the event triggered from.</param>
/// <param name="button">The mouse button that triggered the event.</param>
/// <param name="action">Whether the mouse button was pressed or released.</param>
/// <param name="mods">Bit field of which modifier keys were held down.</param>
void onion_mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		// Construct the data object
		MousePressEvent event_data = { g_MouseListener.x, g_MouseListener.y, button, mods };

		// Trigger the global listener
		g_MouseListener.trigger(event_data);
	}
	else
	{
		// Construct the data object
		MouseReleaseEvent event_data = { g_MouseListener.x, g_MouseListener.y, button };

		// Trigger the global listener
		g_MouseListener.trigger(event_data);
	}
}



int onion_init(const char* settings_file)
{
	// Initialize the GLFW library.
	if (!glfwInit())
	{
		return 1;
	}

	// Load the application settings.
	g_SettingsFile = settings_file;

	Application* app = get_application_settings();
	if (!app)
	{
		glfwTerminate();
		return 1;
	}

	// Initialize the application window.
	app->display();
	if (!g_Window)
	{
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(g_Window);

	glfwSetKeyCallback(g_Window, onion_key_callback);
	glfwSetCursorPosCallback(g_Window, onion_mouse_move_callback);
	glfwSetMouseButtonCallback(g_Window, onion_mouse_click_callback);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(g_Window);
		glfwTerminate();
		return 1;
	}

	return 0;
}

void onion_main(onion_display_func display_callback)
{
	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Set the blend function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Core loop
	while (!glfwWindowShouldClose(g_Window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw everything
		display_callback();

		// Swap buffers
		glfwSwapBuffers(g_Window);

		// Poll events
		glfwPollEvents();
	}

	// Close everything down.
	glfwDestroyWindow(g_Window);
	glfwTerminate();
}