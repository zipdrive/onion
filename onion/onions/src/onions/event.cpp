#include <iostream>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <thread>
#include <ctime>
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
	/// <summary>Pops a listener from the stack.</summary>
	/// <param name="listener">The listener to remove from the stack.</param>
	void pop(EventListener<EventType>* listener)
	{
		for (auto iter = m_Stack.begin(); iter != m_Stack.end(); ++iter)
		{
			if (*iter == listener)
			{
				m_Stack.erase(iter);
				break;
			}
		}
	}

	/// <summary>Pushes a listener on top of the stack.</summary>
	/// <param name="listener">The listener to add to the stack.</param>
	void push(EventListener<EventType>* listener)
	{
		pop(listener);
		m_Stack.push_back(listener);
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



int UpdateEvent::frame{ 1 };
int UpdateEvent::frames_per_second{ 60 };

class StackUpdateListener : public UpdateListener
{
private:
	// Whether the listener is updating or not.
	bool m_Frozen;

	// The stack of event listeners to update.
	vector<UpdateListener*> m_Stack;

protected:
	void __update()
	{
		for (auto iter = m_Stack.rbegin(); iter != m_Stack.rend(); ++iter)
		{
			(*iter)->update();
		}
	}

public:
	/// <summary>Freezes input to the global listener.</summary>
	void freeze()
	{
		m_Frozen = true;
	}

	/// <summary>Unfreezes input to the global listener.</summary>
	void unfreeze()
	{
		m_Frozen = false;
	}

	/// <summary>Pops a listener from the stack.</summary>
	/// <param name="listener">The listener to remove from the stack.</param>
	void pop(UpdateListener* listener)
	{
		for (auto iter = m_Stack.begin(); iter != m_Stack.end(); ++iter)
		{
			if (*iter == listener)
			{
				m_Stack.erase(iter);
				break;
			}
		}
	}

	/// <summary>Pushes a listener on top of the stack.</summary>
	/// <param name="listener">The listener to add to the stack.</param>
	void push(UpdateListener* listener)
	{
		pop(listener);
		m_Stack.push_back(listener);
	}

} g_UpdateManager;



UpdateListener::~UpdateListener()
{
	g_UpdateManager.pop(this);
}

void UpdateListener::update()
{
	__update();
	m_LastFrameUpdated = UpdateEvent::frame;
}

bool UpdateListener::has_updated()
{
	return UpdateEvent::frame == m_LastFrameUpdated;
}

void UpdateListener::freeze()
{
	g_UpdateManager.pop(this);
}

void UpdateListener::unfreeze()
{
	g_UpdateManager.push(this);
}




class StackKeyboardListener : public EventListener<UnicodeEvent>
{
private:
	// Whether the manager is receiving keyboard inputs or not
	bool m_Frozen;

	// The map from keys to keyboard controls
	std::unordered_map<int, int> m_KeyboardControls;

	// The stack of listeners
	std::vector<KeyboardListener*> m_Stack;

public:
	/// <summary>Converts a key input to the associated keyboard control. Returns -1 if no control is assigned.</summary>
	/// <param name="key">The key received.</param>
	/// <returns>The unique ID of the keyboard control associated with the key.</returns>
	int convert_key_to_control(int key)
	{
		auto iter = m_KeyboardControls.find(key);
		return iter != m_KeyboardControls.end() ? iter->second : -1;
	}

	/// <summary>Retrieves the name of the key assigned to the keyboard control.</summary>
	/// <param name="control">The unique ID of the keyboard control.</param>
	string get_key_from_control(int control)
	{
		for (auto iter = m_KeyboardControls.begin(); iter != m_KeyboardControls.end(); ++iter)
		{
			if (iter->second == control)
			{
				const char* key_name = glfwGetKeyName(iter->first, 0);
				return string(key_name ? key_name : "UNKNOWN");
			}
		}
	}
	
	/// <summary>Assigns a key input with a keyboard control.</summary>
	/// <param name="control">The unique ID of the keyboard control to assign.</param>
	/// <param name="key">The key to assign.</param>
	void assign_key_to_control(int control, int key = -1)
	{
		if (key >= 0)
		{
			auto prev = m_KeyboardControls.find(key);
			if (prev == m_KeyboardControls.end())
			{
				for (auto iter = m_KeyboardControls.begin(); iter != m_KeyboardControls.end(); ++iter)
				{
					if (iter->second == control)
					{
						m_KeyboardControls.erase(iter);
					}
				}

				m_KeyboardControls.emplace(key, control);
			}
		}
		else
		{
			int k = 0;
			while (true)
			{
				auto prev = m_KeyboardControls.find(k);
				if (prev != m_KeyboardControls.end())
				{
					m_KeyboardControls.emplace(k, control);
					break;
				}

				++k;
			}
		}
	}


	/// <summary>Pushes a listener on top of the stack.</summary>
	/// <param name="listener">The listener to add to the stack.</param>
	void push(KeyboardListener* listener)
	{
		m_Stack.push_back(listener);
	}

	/// <summary>Pops a listener from the stack.</summary>
	/// <param name="listener">The listener to remove from the stack.</param>
	void pop(KeyboardListener* listener)
	{
		for (auto iter = m_Stack.begin(); iter != m_Stack.end(); ++iter)
		{
			if (*iter == listener)
			{
				m_Stack.erase(iter);
				break;
			}
		}
	}


	/// <summary>Responds to an event.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const KeyEvent& event_data)
	{
		if (!m_Frozen)
		{
			for (auto iter = m_Stack.rbegin(); iter != m_Stack.rend(); ++iter)
			{
				if ((*iter)->trigger(event_data) == EVENT_STOP)
				{
					return EVENT_STOP;
				}
			}
		}

		return EVENT_CONTINUE;
	}
	
	/// <summary>Responds to an event.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const UnicodeEvent& event_data)
	{
		if (!m_Frozen)
		{
			for (auto iter = m_Stack.rbegin(); iter != m_Stack.rend(); ++iter)
			{
				if ((*iter)->trigger(event_data) == EVENT_STOP)
				{
					return EVENT_STOP;
				}
			}
		}

		return EVENT_CONTINUE;
	}


	void freeze()
	{
		m_Frozen = true;
	}

	void unfreeze()
	{
		m_Frozen = false;
	}

} g_KeyboardManager;



KeyboardListener::~KeyboardListener()
{
	g_KeyboardManager.pop(this);
}

void KeyboardListener::freeze()
{
	g_KeyboardManager.pop(this);
}

void KeyboardListener::unfreeze()
{
	g_KeyboardManager.push(this);
}

int KeyboardListener::trigger(const KeyEvent& event_data)
{
	return EVENT_STOP;
}

int KeyboardListener::trigger(const UnicodeEvent& event_data)
{
	return EVENT_STOP;
}



void register_keyboard_control(int control)
{
	g_KeyboardManager.assign_key_to_control(control);
}

string get_assigned_key(int control)
{
	return g_KeyboardManager.get_key_from_control(control);
}

void assign_key(int control)
{
	// TODO
}



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


	void pop(MouseMoveListener* listener)
	{
		StackMouseMoveListener::pop(listener);
	}

	void pop(MousePressListener* listener)
	{
		StackMousePressListener::pop(listener);
	}

	void pop(MouseReleaseListener* listener)
	{
		StackMouseReleaseListener::pop(listener);
	}

} g_MouseManager;



MouseMoveListener::~MouseMoveListener()
{
	g_MouseManager.pop(this);
}

void MouseMoveListener::freeze()
{
	g_MouseManager.pop(this);
}

void MouseMoveListener::unfreeze()
{
	g_MouseManager.push(this);
}


MousePressListener::~MousePressListener()
{
	g_MouseManager.pop(this);
}

void MousePressListener::freeze()
{
	g_MouseManager.pop(this);
}

void MousePressListener::unfreeze()
{
	g_MouseManager.push(this);
}


MouseReleaseListener::~MouseReleaseListener()
{
	g_MouseManager.pop(this);
}

void MouseReleaseListener::freeze()
{
	g_MouseManager.pop(this);
}

void MouseReleaseListener::unfreeze()
{
	g_MouseManager.push(this);
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



/// <summary>The callback function for when input for a Unicode character is received.</summary>
/// <param name="window">The window where the event was triggered.</param>
/// <param name="codepoint">The native endian UTF-32 codepoint received.</param>
void onion_unicode_callback(GLFWwindow* window, unsigned int codepoint)
{
	// Construct the data object
	UnicodeEvent event_data = { codepoint };

	// Trigger the global listener
	g_KeyboardManager.trigger(event_data);
}

/// <summary>The callback function for when a physical key is pressed, released, or repeated.</summary>
/// <param name="window">The window that the event triggered from.</param>
/// <param name="key">The keyboard key that triggered the event.</param>
/// <param name="scancode">The scancode of the key.</param>
/// <param name="action">Whether the key was pressed, released, or repeated.</param>
/// <param name="mods">Bit field of which modifier keys were held down.</param>
void onion_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Call key

	// Call unicode callback
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_BACKSPACE)
		{
			onion_unicode_callback(window, 0x08);
		}
		else if (key == GLFW_KEY_DELETE)
		{
			onion_unicode_callback(window, 0x7f);
		}
	}
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
	g_MouseManager.trigger(event_data);
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
		MousePressEvent event_data = { g_MouseManager.x, g_MouseManager.y, button, mods };

		// Trigger the global listener
		g_MouseManager.trigger(event_data);
	}
	else
	{
		// Construct the data object
		MouseReleaseEvent event_data = { g_MouseManager.x, g_MouseManager.y, button };

		// Trigger the global listener
		g_MouseManager.trigger(event_data);
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
	glfwSetCharCallback(g_Window, onion_unicode_callback);
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
	// Set the blend function
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// TODO glBlendFuncSeparate to do rgb and alpha separately?

	// TEMP? Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glClearColor(1.f, 1.f, 1.f, 1.f);

	// Core loop
	while (!glfwWindowShouldClose(g_Window))
	{
		// Calculate next frame
		int new_frame = clock() * UpdateEvent::frames_per_second / CLOCKS_PER_SEC;

		// Update if it is a new frame
		if (new_frame != UpdateEvent::frame)
		{
			// Update everything
			UpdateEvent::frame = new_frame;
			g_UpdateManager.update();

			// Clear the screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Draw everything
			display_callback();

			// Swap buffers
			glfwSwapBuffers(g_Window);
		}

		// TODO wait until frame interval has passed
		glfwPollEvents();// temporary
	}

	// Close everything down.
	glfwDestroyWindow(g_Window);
	glfwTerminate();
}