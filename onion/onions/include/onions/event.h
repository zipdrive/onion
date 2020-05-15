#pragma once

// Return value to continue processing events.
#define EVENT_CONTINUE 0
// Return value to stop processing events.
#define EVENT_STOP 1


// An interface that responds to an event.
template <typename EventType>
class EventListener
{
public:
	virtual ~EventListener() {}

	/// <summary>Freezes the listener.</summary>
	virtual void freeze() = 0;

	/// <summary>Unfreezes the listener.</summary>
	virtual void unfreeze() = 0;

	/// <summary>Responds to an event.</summary>
	/// <param name="event_data">The data for the event.</param>
	virtual int trigger(const EventType& event_data) = 0;
};



struct UpdateEvent
{
	// The current frame
	static int frame;

	// The number of frames per second.
	static int frames_per_second;
};

class UpdateListener
{
private:
	// The last frame that the listener updated.
	int m_LastFrameUpdated;

protected:
	/// <summary>Updates the listener. This is the function that should be overridden by subclasses.</summary>
	virtual void __update() = 0;

public:
	/// <summary>Destroys the listener.</summary>
	virtual ~UpdateListener();

	/// <summary>Checks whether the listener has been updated this frame.</summary>
	bool has_updated();

	/// <summary>Stops the listener from updating.</summary>
	virtual void freeze();

	/// <summary>Causes the listener to update.</summary>
	virtual void unfreeze();

	/// <summary>Updates the listener, including frame data.</summary>
	void update();
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
class KeyboardListener : public EventListener<KeyEvent>, public EventListener<UnicodeEvent>
{
public:
	/// <summary>Destroys the listener.</summary>
	virtual ~KeyboardListener();

	/// <summary>Freezes the listener.</summary>
	virtual void freeze();

	/// <summary>Unfreezes the listener.</summary>
	virtual void unfreeze();

	/// <summary>Responds to a keyboard control being pressed.</summary>
	/// <param name="event_data">The data for the event.</param>
	virtual int trigger(const KeyEvent& event_data);

	/// <summary>Responds to a Unicode character being received.</summary>
	/// <param name="event_data">The data for the event.</param>
	virtual int trigger(const UnicodeEvent& event_data);
};



/// <summary>Registers a keyboard control to be recognized by the application.</summary>
/// <param name="control">The unique ID of the control.</param>
void register_keyboard_control(int control);

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
class MouseMoveListener : public EventListener<MouseMoveEvent>
{
public:
	/// <summary>Destroys the listener.</summary>
	virtual ~MouseMoveListener();

	/// <summary>Freezes the listener.</summary>
	virtual void freeze();

	/// <summary>Unfreezes the listener.</summary>
	virtual void unfreeze();
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
class MousePressListener : public EventListener<MousePressEvent>
{
public:
	/// <summary>Destroys the listener.</summary>
	virtual ~MousePressListener();

	/// <summary>Freezes the listener.</summary>
	virtual void freeze();

	/// <summary>Unfreezes the listener.</summary>
	virtual void unfreeze();
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
class MouseReleaseListener : public EventListener<MouseReleaseEvent>
{
public:
	/// <summary>Destroys the listener.</summary>
	virtual ~MouseReleaseListener();

	/// <summary>Freezes the listener.</summary>
	virtual void freeze();

	/// <summary>Unfreezes the listener.</summary>
	virtual void unfreeze();
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
	virtual void unfreeze();
};