#include <iostream>
#include <fstream>
#include <regex>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <ctime>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../include/onions/state.h"
#include "../../include/onions/event.h"
#include "../../include/onions/graphics/transform.h"
#include "../../include/onions/world/lighting.h"


using namespace std;

namespace onion
{


	// A stack of event listeners to call in order.
	template <typename _Key, typename _Listener>
	class StackEventManager
	{
	protected:
		struct ListenerPriority
		{
			// The listener.
			_Listener* listener;

			// The priority of the listener.
			_Key priority;

			/// <summary>Constructs an object that stores a listener and a priority.</summary>
			/// <param name="listener">The event listener.</param>
			/// <param name="priority">The priority of the listener.</param>
			ListenerPriority(_Listener* listener, _Key priority) : listener(listener), priority(priority) {}
		};

		struct ListenerPriorityComparer
		{
			bool operator()(shared_ptr<ListenerPriority> left, shared_ptr<ListenerPriority> right)
			{
				return left->priority < right->priority;
			}
		};


		// The stack of event listeners to call.
		vector<shared_ptr<ListenerPriority>> m_Listeners;


		// A queue of event listeners to add before the next trigger pass.
		unordered_set<shared_ptr<ListenerPriority>> m_QueuedToAdd;

		// An array of event listeners to remove before the next trigger pass.
		unordered_set<_Listener*> m_QueuedToRemove;


		/// <summary>Removes any listeners that have been queued for removal, and adds any that have been queued for addition.</summary>
		void flush()
		{
			// Remove any listeners queued for removal
			auto iter = m_Listeners.begin();
			while (iter != m_Listeners.end())
			{
				if (m_QueuedToRemove.count((*iter)->listener) > 0)
				{
					if (iter == m_Listeners.begin())
					{
						m_Listeners.erase(iter);
						iter = m_Listeners.begin();
					}
					else
					{
						m_Listeners.erase(iter--);
						++iter;
					}
				}
				else
				{
					++iter;
				}
			}
			m_QueuedToRemove.clear();

			// Add any listeners queued to add
			m_Listeners.insert(m_Listeners.end(), m_QueuedToAdd.begin(), m_QueuedToAdd.end());
			std::sort(m_Listeners.begin(), m_Listeners.end(), ListenerPriorityComparer());
			m_QueuedToAdd.clear();
		}

	public:
		/// <summary>Clean up the event listeners in the stack.</summary>
		virtual ~StackEventManager()
		{
			// Deconstruct every listener in the stack, so they aren't popping to an already deconstructed event manager
			for (auto iter = m_Listeners.begin(); iter != m_Listeners.end(); ++iter)
			{
				_Listener* listener = (*iter)->listener;
				if (m_QueuedToRemove.count(listener) > 0) // If it hasn't been queued for removal, then it's still under the domain of the manager
				{
					delete listener;
				}
			}
		}

		/// <summary>Pops a listener from the stack.</summary>
		/// <param name="listener">The listener to remove from the stack.</param>
		void pop(_Listener* listener)
		{
			m_QueuedToRemove.insert(listener);
		}

		/// <summary>Pushes a listener on top of the stack.</summary>
		/// <param name="listener">The listener to add to the stack.</param>
		/// <param name="priority">The priority of the listener.</param>
		void push(_Listener* listener, _Key priority)
		{
			m_QueuedToRemove.insert(listener);
			m_QueuedToAdd.emplace(new ListenerPriority(listener, priority));
		}
	};

	template <typename _Key, typename _Listener, typename... _Args>
	class StackEventListener : public StackEventManager<_Key, _Listener>
	{
	public:
		/// <summary>Responds to an event.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual void trigger(_Args... event_data)
		{
			// Flush the queue
			flush();

			// Trigger all listeners
			for (auto iter = m_Listeners.begin(); iter != m_Listeners.end(); ++iter)
			{
				if ((*iter)->listener->trigger(event_data...) == EVENT_STOP)
				{
					break;
				}
			}
		}
	};


	typedef StackEventListener<EVENT_PRIORITY, UpdateListener> StackUpdateListener;

	typedef StackEventListener<EVENT_PRIORITY, MouseMoveListener, const MouseMoveEvent&> StackMouseMoveListener;
	typedef StackEventListener<EVENT_PRIORITY, MousePressListener, const MousePressEvent&> StackMousePressListener;
	typedef StackEventListener<EVENT_PRIORITY, MouseReleaseListener, const MouseReleaseEvent&> StackMouseReleaseListener;



	int UpdateEvent::frame{ 1 };
	int UpdateEvent::frames_per_second{ 60 };

	StackUpdateListener g_UpdateManager;



	UpdateListener::~UpdateListener()
	{
		g_UpdateManager.pop(this);
	}

	int UpdateListener::trigger()
	{
		update(UpdateEvent::frame - m_LastFrameUpdated);
		m_LastFrameUpdated = UpdateEvent::frame;

		return EVENT_CONTINUE;
	}

	bool UpdateListener::has_updated()
	{
		return UpdateEvent::frame == m_LastFrameUpdated;
	}

	void UpdateListener::freeze()
	{
		g_UpdateManager.pop(this);
	}

	void UpdateListener::unfreeze(EVENT_PRIORITY priority)
	{
		m_LastFrameUpdated = UpdateEvent::frame;
		g_UpdateManager.push(this, priority);
	}




	class StackKeyboardListener : public StackEventManager<EVENT_PRIORITY, KeyboardListener>
	{
	private:
		// The map from keys to keyboard controls
		std::unordered_map<int, int> m_KeyboardControls;

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
					auto iter = m_KeyboardControls.begin();
					while (iter != m_KeyboardControls.end())
					{
						if (iter->second == control)
						{
							if (iter == m_KeyboardControls.begin())
							{
								m_KeyboardControls.erase(iter);
								iter = m_KeyboardControls.begin();
							}
							else
							{
								m_KeyboardControls.erase(iter++);
							}
						}
						else
						{
							++iter;
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
					if (prev == m_KeyboardControls.end())
					{
						m_KeyboardControls.emplace(k, control);
						break;
					}

					++k;
				}
			}
		}


		/// <summary>Responds to an event.</summary>
		/// <param name="event_data">The data for the event.</param>
		void trigger(const KeyEvent& event_data)
		{
			flush();

			// Trigger all listeners
			for (auto iter = m_Listeners.rbegin(); iter != m_Listeners.rend(); ++iter)
			{
				if ((*iter)->listener->trigger(event_data) == EVENT_STOP)
				{
					break;
				}
			}
		}

		/// <summary>Responds to an event.</summary>
		/// <param name="event_data">The data for the event.</param>
		void trigger(const UnicodeEvent& event_data)
		{
			flush();

			for (auto iter = m_Listeners.rbegin(); iter != m_Listeners.rend(); ++iter)
			{
				if ((*iter)->listener->trigger(event_data) == EVENT_STOP)
				{
					break;
				}
			}
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

	void KeyboardListener::unfreeze(EVENT_PRIORITY priority)
	{
		g_KeyboardManager.push(this, priority);
	}

	int KeyboardListener::trigger(const KeyEvent& event_data)
	{
		return EVENT_STOP;
	}

	int KeyboardListener::trigger(const UnicodeEvent& event_data)
	{
		return EVENT_STOP;
	}



	void register_keyboard_control(int control, int key)
	{
		g_KeyboardManager.assign_key_to_control(control, key);
	}

	string get_assigned_key(int control)
	{
		return g_KeyboardManager.get_key_from_control(control);
	}

	int g_AssigningKeyToControl = -1;

	void assign_key(int control)
	{
		g_AssigningKeyToControl = control;
	}



	// A listener that keeps track of the mouse state.
	class StackMouseListener : public StackMouseMoveListener, public StackMousePressListener, public StackMouseReleaseListener
	{
	public:
		// The x-coordinate of the mouse.
		int x;

		// The y-coordinate of the mouse.
		int y;

		/// <summary>Triggers against a mouse move event.</summary>
		/// <param name="event_data">The data for the event.</param>
		void trigger(const MouseMoveEvent& event_data)
		{
			x = event_data.x;
			y = event_data.y;

			return StackMouseMoveListener::trigger(event_data);
		}

		using StackMousePressListener::trigger;
		using StackMouseReleaseListener::trigger;


		using StackMouseMoveListener::push;
		using StackMousePressListener::push;
		using StackMouseReleaseListener::push;

		using StackMouseMoveListener::pop;
		using StackMousePressListener::pop;
		using StackMouseReleaseListener::pop;

	} g_MouseManager;



	MouseMoveListener::~MouseMoveListener()
	{
		g_MouseManager.pop(this);
	}

	void MouseMoveListener::freeze()
	{
		g_MouseManager.pop(this);
	}

	void MouseMoveListener::unfreeze(EVENT_PRIORITY priority)
	{
		g_MouseManager.push(this, priority);
	}


	MousePressListener::~MousePressListener()
	{
		g_MouseManager.pop(this);
	}

	void MousePressListener::freeze()
	{
		g_MouseManager.pop(this);
	}

	void MousePressListener::unfreeze(EVENT_PRIORITY priority)
	{
		g_MouseManager.push(this, priority);
	}


	MouseReleaseListener::~MouseReleaseListener()
	{
		g_MouseManager.pop(this);
	}

	void MouseReleaseListener::freeze()
	{
		g_MouseManager.pop(this);
	}

	void MouseReleaseListener::unfreeze(EVENT_PRIORITY priority)
	{
		g_MouseManager.push(this, priority);
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

	void MouseDraggableListener::unfreeze(EVENT_PRIORITY priority)
	{
		MousePressListener::unfreeze(priority);
		MouseMoveListener::unfreeze(priority);
		MouseReleaseListener::unfreeze(priority);
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

		// Resize the state, if it exists
		if (State* state = get_state())
			state->set_bounds(width, height);

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
		// Check if currently assigning a key to a control
		if (g_AssigningKeyToControl >= 0)
		{
			g_KeyboardManager.assign_key_to_control(g_AssigningKeyToControl, key);
			g_AssigningKeyToControl = -1;
			return;
		}

		// Call key
		int control = g_KeyboardManager.convert_key_to_control(key);
		if (control != -1 && action != GLFW_REPEAT)
		{
			KeyEvent event_data = { control, action == GLFW_PRESS };
			g_KeyboardManager.trigger(event_data);
		}

		// Call unicode callback
		if (action == GLFW_PRESS || action == GLFW_REPEAT)
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



	int onion::init(const char* settings_file)
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

		// Set up the transformation matrices
		Transform::init();
		Lighting::init();

		return 0;
	}

	void onion::main(display_func display_callback)
	{
		// Set the blend function
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// TODO glBlendFuncSeparate to do rgb and alpha separately?

		// TEMP? Enable depth testing
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(0.f, 0.f, 0.f, 1.f);

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
				g_UpdateManager.trigger();

				// Clear the screen
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Draw everything
				display_callback();

				// Synchronize the CPU with the GPU
				GLsync synchro = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
				glClientWaitSync(synchro, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);
				glDeleteSync(synchro);

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

}