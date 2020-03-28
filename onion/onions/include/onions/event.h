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
	/// <summary>Freezes the listener.</summary>
	virtual void freeze() = 0;

	/// <summary>Unfreezes the listener.</summary>
	virtual void unfreeze() = 0;

	/// <summary>Responds to an event.</summary>
	/// <param name="event_data">The data for the event.</param>
	virtual int trigger(const EventType& event_data) = 0;
};


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



/// <summary>Pushes a listener for mouse movement to the global event listener stack.</summary>
/// <param name="listener">The listener to push.</param>
void push_mouse_move_listener(MouseMoveListener* listener);

/// <summary>Pops the listener for mouse movement that is on top of the global event listener stack.</summary>
void pop_mouse_move_listener();

/// <summary>Pushes a listener for mouse button pressing to the global event listener stack.</summary>
/// <param name="listener">The listener to push.</param>
void push_mouse_press_listener(MousePressListener* listener);

/// <summary>Pops the listener for mouse button pressing that is on top of the global event listener stack.</summary>
void pop_mouse_press_listener();

/// <summary>Pushes a listener for mouse button releasing to the global event listener stack.</summary>
/// <param name="listener">The listener to push.</param>
void push_mouse_release_listener(MouseReleaseListener* listener);

/// <summary>Pops the listener for mouse button releasing that is on top of the global event listener stack.</summary>
void pop_mouse_release_listener();