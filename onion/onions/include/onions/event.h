#pragma once
#include <string>

// Return value to continue processing events.
#define EVENT_CONTINUE 0
// Return value to stop processing events.
#define EVENT_STOP 1


#define EVENT_PRIORITY int



// Default key controls

#define ONION_KEY_LEFT		0x100
#define ONION_KEY_RIGHT		0x101
#define ONION_KEY_DOWN		0x102
#define ONION_KEY_UP		0x103

#define ONION_KEY_SELECT	0x200
#define ONION_KEY_CANCEL	0x201



namespace onion
{


	// An interface that responds to an event.
	template <typename... _EventType>
	class EventListener
	{
	public:
		/// <summary>Destroys the listener.</summary>
		virtual ~EventListener() {}

		/// <summary>Freezes the listener.</summary>
		virtual void freeze() = 0;

		/// <summary>Unfreezes the listener.</summary>
		/// <param name="priority">The priority for the listener. High numbers trigger in response to events before low numbers.</param>
		virtual void unfreeze(EVENT_PRIORITY priority) = 0;

		/// <summary>Responds to an event.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(_EventType... event_data) = 0;
	};



	struct UpdateEvent
	{
		// The current frame
		static int frame;

		// The number of frames per second.
		static int frames_per_second;
	};

	class UpdateListener : public EventListener<>
	{
	private:
		// The last frame that the listener updated.
		int m_LastFrameUpdated;

	protected:
		/// <summary>Updates the listener. This is the function that should be overridden by subclasses.</summary>
		/// <param name="frames_passed">The number of frames that have passed since the last update.</param>
		virtual void update(int frames_passed) = 0;

	public:
		/// <summary>Destroys the listener.</summary>
		virtual ~UpdateListener();

		/// <summary>Checks whether the listener has been updated this frame.</summary>
		bool has_updated();

		/// <summary>Stops the listener from updating.</summary>
		virtual void freeze();

		/// <summary>Causes the listener to update.</summary>
		/// <param name="priority">The priority for the listener. High numbers trigger in response to events before low numbers.</param>
		virtual void unfreeze(EVENT_PRIORITY priority);

		/// <summary>Updates the listener, including frame data.</summary>
		int trigger();
	};



	struct KeyEvent
	{
		// The keyboard input that was processed.
		int control;

		// True if the key was pressed, false if it was released.
		bool pressed;
	};

	struct UnicodeEvent
	{
		// The Unicode character that was processed.
		unsigned int character;
	};

	// A listener that responds to key presses and text input.
	class KeyboardListener : public EventListener<const KeyEvent&>, public EventListener<const UnicodeEvent&>
	{
	public:
		/// <summary>Destroys the listener.</summary>
		virtual ~KeyboardListener();

		/// <summary>Freezes the listener.</summary>
		virtual void freeze();

		/// <summary>Unfreezes the listener.</summary>
		/// <param name="priority">The priority for the listener. High numbers trigger in response to events before low numbers.</param>
		virtual void unfreeze(EVENT_PRIORITY priority);

		/// <summary>Responds to a keyboard control being pressed.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(const KeyEvent& event_data);

		/// <summary>Responds to a Unicode character being received.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(const UnicodeEvent& event_data);
	};


	/// <summary>Registers a keyboard control to be recognized by the application.</summary>
	/// <param name="control">The unique ID of the control.</param>
	/// <param name="key">(Optional) The ID of the key to be assigned to the control.</param>
	void register_keyboard_control(int control, int key = -1);

	/// <summary>Retrieves the name of the key assigned to a keyboard control.</summary>
	/// <param name="control">The unique ID of the control.</param>
	/// <returns>The name of the key assigned to the keyboard control.</returns>
	std::string get_assigned_key(int control);

	/// <summary>Assigns the next key pressed to the keyboard control passed to this function.</summary>
	/// <param name="control">The unique ID of the control.</param>
	void assign_key(int control);




	struct MouseMoveEvent
	{
		// The x-coordinate of the mouse.
		int x;

		// The y-coordinate of the mouse.
		int y;
	};

	// A listener that responds to mouse movements.
	class MouseMoveListener : public EventListener<const MouseMoveEvent&>
	{
	public:
		/// <summary>Destroys the listener.</summary>
		virtual ~MouseMoveListener();

		/// <summary>Freezes the listener.</summary>
		virtual void freeze();

		/// <summary>Unfreezes the listener.</summary>
		/// <param name="priority">The priority for the listener. High numbers trigger in response to events before low numbers.</param>
		virtual void unfreeze(EVENT_PRIORITY priority);
	};


	struct MousePressEvent
	{
		// The x-coordinate of the mouse.
		int x;

		// The y-coordinate of the mouse.
		int y;

		// The button pressed.
		int button;

		// Bit field of modifier keys.
		int mods;
	};

	// A listener that responds to mouse button pressing.
	class MousePressListener : public EventListener<const MousePressEvent&>
	{
	public:
		/// <summary>Destroys the listener.</summary>
		virtual ~MousePressListener();

		/// <summary>Freezes the listener.</summary>
		virtual void freeze();

		/// <summary>Unfreezes the listener.</summary>
		/// <param name="priority">The priority for the listener. High numbers trigger in response to events before low numbers.</param>
		virtual void unfreeze(EVENT_PRIORITY priority);
	};


	struct MouseReleaseEvent
	{
		// The x-coordinate of the mouse.
		int x;

		// The y-coordinate of the mouse.
		int y;

		// The button released.
		int button;
	};

	// A listener that responds to mouse movements.
	class MouseReleaseListener : public EventListener<const MouseReleaseEvent&>
	{
	public:
		/// <summary>Destroys the listener.</summary>
		virtual ~MouseReleaseListener();

		/// <summary>Freezes the listener.</summary>
		virtual void freeze();

		/// <summary>Unfreezes the listener.</summary>
		/// <param name="priority">The priority for the listener. High numbers trigger in response to events before low numbers.</param>
		virtual void unfreeze(EVENT_PRIORITY priority);
	};



	class MouseDraggableListener : public MousePressListener, public MouseMoveListener, public MouseReleaseListener
	{
	public:
		// The dragged object.
		static MouseDraggableListener* dragged;

		/// <summary>Responds to the mouse being released.</summary>
		/// <param name="event_data">The data for the event.</param>
		virtual int trigger(const MouseReleaseEvent& event_data);

		/// <summary>Freezes the listener.</summary>
		virtual void freeze();

		/// <summary>Unfreezes the listener.</summary>
		/// <param name="priority">The priority for the listener. High numbers trigger in response to events before low numbers.</param>
		virtual void unfreeze(EVENT_PRIORITY priority);
	};



}