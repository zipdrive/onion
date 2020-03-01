#pragma once

#include <vector>
#include <unordered_map>


/*
*
*	Definitions
*
*/

// The key to an individual sprite on a sprite sheet
#define SPRITE_ID int16_t
#define SPRITE_KEY int

// The key to a chunk of static data
#define CHUNK_KEY int

// The width and height of a tile in the world.
#define TILE_WIDTH 32
#define TILE_HEIGHT 32

// Return value to continue processing events.
#define EVENT_CONTINUE 0
// Return value to stop processing events.
#define EVENT_STOP 1





/*
*
*	Matrices
*
*/

// A matrix of numeric values
template <typename T, int _Rows, int _Columns>
class matrix
{
private:
	T mat[_Rows * _Columns];

public:
	/// <summary>Retrieves a pointer to the matrix value array.</summary>
	/// <returns>A pointer to the matrix value array.</returns>
	const T* matrix_values() const
	{
		// DEBUG: Set breakpoint on this line to view 
		return mat;
	}


	/// <summary>Retrieves a reference to the value at the specified index of the matrix value array.</summary>
	/// <param name="index">The index of the value.</param>
	T& operator()(int index)
	{
		return mat[index];
	}

	/// <summary>Retrieves a reference to the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	T& operator()(int row, int column)
	{
		return mat[row + (_Rows * column)]; // Row-major order
	}


	/// <summary>Retrieves the value at the specified index of the matrix value array.</summary>
	/// <param name="index">The index of the value.</param>
	T get(int index) const
	{
		return mat[index];
	}

	/// <summary>Retrieves the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	T get(int row, int column) const
	{
		return mat[row + (_Rows * column)]; // Row-major order
	}


	/// <summary>Sets the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	void set(int row, int column, T value)
	{
		mat[row + (_Rows * column)] = value;
	}

	matrix()
	{
		for (int r = _Rows - 1; r >= 0; --r)
		{
			for (int c = _Columns - 1; c >= 0; --c)
			{
				if (r == c && _Rows == _Columns)
					set(r, c, 1);
				else
					set(r, c, 0);
			}
		}
	}
};

/// <summary>Multiplies two matrices.</summary>
/// <param name="lhs">The matrix being multiplied on the left.</param>
/// <param name="rhs">The matrix being multiplied on the right.</param>
/// <param name="res">Outputs the resulting matrix.</param>
template <typename T, int _Rows, int _Middle, int _Columns>
void mat_mul(const matrix<T, _Rows, _Middle>& lhs, const matrix<T, _Middle, _Columns>& rhs, matrix<T, _Rows, _Columns>& res)
{
	for (int r = _Rows - 1; r >= 0; --r)
	{
		for (int c = _Columns - 1; c >= 0; --c)
		{
			T val = 0;
			for (int k = _Middle - 1; k >= 0; --k)
				val += lhs.get(r, k) * rhs.get(k, c);
			res.set(r, c, val);
		}
	}
}


// A 4-by-1 vector of float values
class vec4f : public matrix<float, 4, 1>
{
public:
	vec4f();

	vec4f(float a1, float a2, float a3, float a4);
};

// A 4-by-4 matrix of float values
class mat4x4f : public matrix<float, 4, 4>
{
public:
	mat4x4f();

	mat4x4f(float a11, float a12, float a13, float a14,
		float a21, float a22, float a23, float a24,
		float a31, float a32, float a33, float a34,
		float a41 = 0.f, float a42 = 0.f, float a43 = 0.f, float a44 = 1.f);
};


typedef matrix<int, 2, 2> mat2x2i;

typedef matrix<float, 2, 1> vec2f;


/// <summary>Pushes a copy of the current matrix to the top of the stack.</summary>
void mat_push();

/// <summary>Pops the current matrix from the top of the stack.</summary>
void mat_pop();

/// <summary>Gets the current matrix transform.</summary>
mat4x4f& mat_get();

/// <summary>Gets the value array of the current matrix transform.</summary>
/// <returns>The value array of the current matrix transform.</returns>
const float* mat_get_values();

/// <summary>Clears the stack and sets the projection to orthogonal.</summary>
/// <param name="left">The left side of the projection.</param>
/// <param name="right">The right side of the projection.</param>
/// <param name="bottom">The bottom side of the projection.<param>
/// <param name="top">The top side of the projection.</param>
void mat_ortho(float left, float right, float bottom, float top, float near, float far);

/// <summary>Adds a translation to the current transformation.</summary>
/// <param name="dx">The translation along the x-axis.</param>
/// <param name="dy">The translation along the y-axis.</param>
/// <param name="dz">The translation along the z-axis.</param>
void mat_translate(float dx, float dy, float dz);

/// <summary>Adds a scale transform to the current transformation.</summary>
/// <param name="sx">The scaling factor for the x-axis.</param>
/// <param name="sy">The scaling factor for the y-axis.</param>
/// <param name="sz">The scaling factor for the z-axis.</param>
void mat_scale(float sx, float sy, float sz);

/// <summary>Adds a rotation transform around the x-axis to the current transformation.</summary>
/// <param name="angle">The angle of rotation.</param>
void mat_rotatex(float angle);

/// <summary>Adds a rotation transform around the y-axis to the current transformation.</summary>
/// <param name="angle">The angle of rotation.</param>
void mat_rotatey(float angle);

/// <summary>Adds a rotation transform around the z-axis to the current transformation.</summary>
/// <param name="angle">The angle of rotation.</param>
void mat_rotatez(float angle);

/// <summary>Adds a custom transformation to the current transformation.</summary>
/// <param name="transform">The matrix of the transformation.</summary>
void mat_custom_transform(const mat4x4f& transform);




/*
*
*	Graphics
*
*/


/// <summary>Generates a palette matrix for use in graphics.</summary>
/// <param name="rr">The red-value of the color that red maps to.</param>
/// <param name="rg">The green-value of the color that red maps to.</param>
/// <param name="rb">The blue-value of the color that red maps to.</param>
/// <param name="ra">The alpha-value of the color that red maps to.</param>
/// <param name="gr">The red-value of the color that green maps to.</param>
/// <param name="gg">The green-value of the color that green maps to.</param>
/// <param name="gb">The blue-value of the color that green maps to.</param>
/// <param name="ga">The alpha-value of the color that green maps to.</param>
/// <param name="br">The red-value of the color that blue maps to.</param>
/// <param name="bg">The green-value of the color that blue maps to.</param>
/// <param name="bb">The blue-value of the color that blue maps to.</param>
/// <param name="ba">The alpha-value of the color that blue maps to.</param>
/// <param name="ar">The red-value of the color that alpha maps to.</param>
/// <param name="ag">The green-value of the color that alpha maps to.</param>
/// <param name="ab">The blue-value of the color that alpha maps to.</param>
/// <param name="aa">The alpha-value of the color that alpha maps to.</param>
mat4x4f generate_palette_matrix(
	int rr = 255, int rg = 0, int rb = 0, int ra = 0,
	int gr = 0, int gg = 255, int gb = 0, int ga = 0,
	int br = 0, int bg = 0, int bb = 255, int ba = 0,
	int ar = 0, int ag = 0, int ab = 0, int aa = 255
);


// Something visible on-screen.
class Graphic
{
public:
	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	virtual int get_width() const = 0;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	virtual int get_height() const = 0;

	/// <summary>Draws the graphic to the screen.</summary>
	virtual void display() const = 0;
};


/// <summary>Creates a solid color graphic.</summary>
/// <param name="r">The red value.</param>
/// <param name="g">The green value.</param>
/// <param name="b">The blue value.</param>
/// <param name="a">The alpha value.</param>
/// <param name="width">The width of the graphic.</param>
/// <param name="height">The height of the graphic.</param>
Graphic* generate_solid_color_graphic(int r, int g, int b, int a, int width, int height);

/// <summary>Creates a solid color graphic.</summary>
/// <param name="r">The red value.</param>
/// <param name="g">The green value.</param>
/// <param name="b">The blue value.</param>
/// <param name="a">The alpha value.</param>
/// <param name="width">The width of the graphic.</param>
/// <param name="height">The height of the graphic.</param>
Graphic* generate_solid_color_graphic(float r, float g, float b, float a, int width, int height);


// Contains multiple sprites on a single texture.
class SpriteSheet
{
protected:
	// Protected so that it cannot be called from the outside.
	SpriteSheet();

public:
	/// <summary>Generates an empty sprite sheet.</summary>
	/// <returns>A pointer to an empty sprite sheet.</returns>
	static SpriteSheet* generate_empty();

	// The width of the sprite sheet in pixels.
	int width;

	// The height of the sprite sheet in pixels.
	int height;

	/// <summary>Loads sprite sheet from an image and meta file.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base. Note: The path to the meta file should be the same as the path to the image file, but with a .meta file extension instead.</param>
	virtual void load_sprite_sheet(const char* path) = 0;

	/// <summary>Loads sprite sheet from an image file, and equally partitions it into sprites.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base.</param>
	/// <param name="partition_width">The width of the individual sprites.</param>
	/// <param name="partition_height">The height of the individual sprites.</param>
	virtual void load_partitioned_sprite_sheet(const char* path, int partition_width, int partition_height) = 0;

	/// <summary>Draws a sprite from the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	/// <param name="color">The color tint matrix of the sprite.</param>
	virtual void display(SPRITE_KEY sprite, const mat4x4f& color) = 0;
};


// An individual sprite on a sprite sheet.
struct Sprite
{
private:
	static std::unordered_map<SPRITE_ID, Sprite*> m_Sprites;

public:
	/// <summary>Retrieves the sprite with the given ID.</summary>
	/// <param name="id">The ID of the sprite.</param>
	static Sprite* get_sprite(SPRITE_ID id);

	/// <summary>Sets the sprite with the given ID.</summary>
	/// <param name="id">The ID of the sprite.</param>
	/// <param name="sprite">The sprite to set.</param>
	static void set_sprite(SPRITE_ID id, Sprite* sprite);


	// The key of the sprite
	SPRITE_KEY key;

	// The width of the sprite
	int width;

	// The height of the sprite
	int height;

	/// <summary>Constructs sprite information.</summary>
	/// <param name="key">The key of the sprite.</param>
	/// <param name="width">The width of the sprite.</param>
	/// <param name="height">The height of the sprite.</param>
	Sprite(SPRITE_KEY key, int width, int height);
};

class SpriteGraphic : public Graphic
{
protected:
	// A pointer to the sprite information
	SpriteSheet* m_SpriteSheet;

	// The color tint matrix for the sprite
	mat4x4f m_TintMatrix;

	/// <summary>Constructs a graphic that draws sprites.</summary>
	/// <param name="sprite_sheet">The sprite sheet.</param>
	/// <param name="color">The color tint matrix for the sprites.</param>
	SpriteGraphic(SpriteSheet* sprite_sheet, mat4x4f& color);

	/// <summary>Retrieves the current sprite to be drawn.</summary>
	/// <returns>The current sprite.</returns>
	virtual Sprite* get_sprite() const = 0;

public:
	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	int get_width() const;

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	int get_height() const;

	/// <summary>Displays the sprite with the given key.</summary>
	void display() const;
};

// A graphic that always displays the same sprite.
class StaticSpriteGraphic : public SpriteGraphic
{
private:
	// The key to the sprite
	Sprite* m_Sprite;

	/// <summary>Retrieves the current sprite to be drawn.</summary>
	/// <returns>The key of the current sprite.</returns>
	Sprite* get_sprite() const;

public:
	/// <summary>Constructs a static sprite graphic.</summary>
	/// <param name="sprite_sheet">The sprite sheet that the sprites are on.</param>
	/// <param name="sprite">The sprite data.</param>
	/// <param name="color">The color tint matrix for the sprites.</param>
	StaticSpriteGraphic(SpriteSheet* sprite_sheet, Sprite* sprite, mat4x4f& color);
};

// A graphic that can swap between multiple frames of sprites.
class DynamicSpriteGraphic : public SpriteGraphic
{
private:
	// The keys to the sprites
	std::vector<Sprite*> m_Sprites;

	// The current frame
	int m_Current;

	/// <summary>Retrieves the current sprite to be drawn.</summary>
	/// <returns>The key of the current sprite.</returns>
	Sprite* get_sprite() const;

public:
	/// <summary>Constructs a static sprite graphic.</summary>
	/// <param name="sprite_sheet">The sprite sheet that the sprites are on.</param>
	/// <param name="color">The color tint matrix for the sprites.</param>
	DynamicSpriteGraphic(SpriteSheet* sprite_sheet, mat4x4f& color);

	/// <summary>Adds a new frame to the graphic.</summary>
	/// <param name="sprite">The frame's sprite.</param>
	void add_frame(Sprite* sprite);

	/// <summary>Sets the current frame.</summary>
	/// <param name="frame">The index of the frame.</param>
	void set_frame(int frame);
};




/*
*
*	World and objects
*
*/

// A thing within the game world.
class Object
{
public:
	/// <summary>Display the object.</summary>
	virtual void display() = 0;
};

// A block of static information about the world.
class Chunk
{
private:
	// A collection of all chunks
	static std::unordered_map<CHUNK_KEY, Chunk*> m_Chunks;

	// The sprite sheet for the tiles
	static SpriteSheet* m_TileSprites;

	// The path to the file containing the chunk's data
	const char* m_ChunkFilepath;

	// Tiles on the ground
	SPRITE_KEY* m_Tiles;

	// Objects in the chunk that don't move
	Object** m_Objects;

public:
	/// <summary>Retrieves the chunk with the given key.</summary>
	/// <param name="key">The key of the chunk</param>
	/// <returns>A pointer to the chunk with the given key</returns>
	static Chunk* get_chunk(CHUNK_KEY key);

	/// <summary>Creates a chunk with the given key and file path.</summary>
	/// <param name="key">The key of the chunk</param>
	/// <param name="path">The path to the file containing the chunk's data</param>
	/// <returns>A pointer to the created chunk</returns>
	static Chunk* set_chunk(CHUNK_KEY key, const char* path);

	// The width of the chunk, in tiles.
	int width;

	// The height of the chunk, in tiles.
	int height;

	/// <summary>Creates a chunk that uses data from the given file.</summary>
	/// <param name="path">The path to the file containing the chunk's data.</param>
	Chunk(const char* path);

	/// <summary>Loads the chunk into memory.</summary>
	void load();

	/// <summary>Unloads the chunk from memory.</summary>
	void unload();

	/// <summary>Draws the tiles of the chunk to the screen.</summary>
	/// <param name="xmin">The leftmost x-coordinate to draw.</param>
	/// <param name="xmax">The rightmost x-coordinate to draw.</param>
	/// <param name="ymin">The bottommost y-coordinate to draw.</param>
	/// <param name="ymax">The topmost y-coordinate to draw.</param>
	void display(int xmin, int ymin, int xmax, int ymax);
};

// All currently loaded information about the world.
class World
{
private:
	// The singleton world object
	static World* m_World;

	// The current chunk
	Chunk* m_Chunk;
	
	/// <summary>Loads the world.</summary>
	World();

public:
	/// <summary>Retrieves the world information.</summary>
	/// <returns>A pointer to the singleton world object.</summary>
	static World* get_world();

	/// <summary>Retrieves a pointer to the current chunk.</summary>
	/// <returns>A pointer to the current chunk.</returns>
	static Chunk* get_chunk();
};




/*
*
*	Events
*
*/

// An interface that responds to an event.
template <typename EventType>
class EventListener 
{
public:
	/// <summary>Responds to an event.</summary>
	/// <param name="event_data">The data for the event.</param>
	virtual int trigger(const EventType& event_data) = 0;
};

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
	void push(EventListener<EventType>* listener);

	/// <summary>Pops the listener on top of the stack.</summary>
	void pop();

	/// <summary>Responds to an event.</summary>
	/// <param name="event_data">The data for the event.</param>
	virtual int trigger(const EventType& event_data);
};


struct MouseMoveEvent
{
	// The x-coordinate of the mouse.
	int x;

	// The y-coordinate of the mouse.
	int y;
};

typedef EventListener<MouseMoveEvent> MouseMoveListener;
typedef StackEventListener<MouseMoveEvent> StackMouseMoveListener;


struct MouseButtonEvent
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

typedef EventListener<MouseButtonEvent> MousePressListener;
typedef StackEventListener<MouseButtonEvent> StackMousePressListener;


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





/*
*
*	Frames
*
*/

class Frame : public Graphic
{
protected:
	// The frame boundaries on the screen
	mat2x2i m_Bounds;

public:
	/// <summary>Creates an empty frame.</summary>
	Frame();

	/// <summary>Creates an empty frame with the given boundaries.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	Frame(int x, int y, int width, int height);

	/// <summary>Retrieves the boundaries of the frame on the screen.</summary>
	/// <returns>A const reference to the boundaries. The first column represents minimum values, and the second column represents maximum values.</returns>
	const mat2x2i& get_bounds() const;

	/// <summary>Sets the boundaries of the frame on the screen.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	virtual void set_bounds(int x, int y, int width, int height);

	/// <summary>Retrieves the width of the frame.</summary>
	/// <returns>The width of the frame.</returns>
	int get_width() const;

	/// <summary>Retrieves the height of the frame.</summary>
	/// <returns>The height of the frame.</returns>
	int get_height() const;
};



// A frame that allows a user to scroll between values.
class ScrollBarFrame : public Frame, public MousePressListener
{
private:
	// The background of the scroll area.
	Graphic* m_Background;

	// The left- or top-facing arrow.
	Graphic* m_Arrow;

	// The horizontal scroll object.
	Graphic* m_Scroller;

protected:
	// The value of the scroll bar.
	float m_Value;

	// Whether the scroll bar scrolls horizontally or vertically.
	bool m_Horizontal;

public:
	/// <summary>Constructs a scroll bar.</summary>
	/// <param name="background_graphic">A graphic to display as the background for the scrolling area.</param>
	/// <param name="arrow_graphic">A graphic to display as arrows on either side of the scrolling area.</param>
	/// <param name="scroll_graphic">A graphic to display to show the current value of the scroll bar.</param>
	/// <param name="x">The x-coordinate of the left side.</param>
	/// <param name="y">The y-coordinate of the bottom side.</param>
	/// <param name="horizontal">True if the scroll bar is horizontal, false if vertical.</param>
	ScrollBarFrame(Graphic* background_graphic, Graphic* arrow_graphic, Graphic* scroll_graphic, int x, int y, bool horizontal);

	/// <summary>Retrieves the current value of the scroll bar.</summary>
	/// <returns>The current value of the scroll bar.</returns>
	float get_value();

	/// <summary>Sets the value of the scroll bar.</summary>
	/// <param name="value">The value for the scroll bar.</param>
	virtual void set_value(float value);

	/// <summary>Triggers in response to a mouse button being pressed.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MouseButtonEvent& event_data);

	/// <summary>Displays the contents of the frame.</summary>
	void display() const;
};


class LayerFrame : public Frame
{
protected:
	// A sequence of things to be displayed.
	std::vector<Graphic*> m_Sequence;

	// The transformation to apply before displaying.
	mat4x4f m_Transform;

	// The z-scale of the transformation.
	float m_ZScale;

	/// <summary>Resets the transformation data.</summary>
	virtual void reset();

public:
	/// <summary>Constructs an empty frame that fills the screen.</summary>
	LayerFrame();

	/// <summary>Constructs an empty frame with the given dimensions.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	LayerFrame(int x, int y, int width, int height);

	/// <summary>Adds a graphic to be displayed over existing graphics.</summary>
	/// <param name="graphic">The graphic to be displayed.</param>
	void insert_top(Graphic* graphic);

	/// <summary>Displays the contents of the frame.</summary>
	void display() const;
};

// A frame of layers that applies an orthographic projection before displaying.
class UIFrame : public LayerFrame
{
protected:
	/// <summary>Resets the transformation matrix.</summary>
	void reset();

public:
	/// <summary>Creates an empty UI frame that takes up the whole screen.</summary>
	UIFrame();
};


// A frame that displays the world orthographically.
class WorldOrthographicFrame : public Frame
{
private:
	/// <summary>
	/// A transform that does the following:
	/// Centers the view at the center of the frame.
	/// Orthographically projects (pixel-perfect).
	/// Centers the camera in world space.
	/// Rotates by -45 degrees around the x-axis, and scales up by sqrt(2).
	/// </summary>
	mat4x4f m_Transform;

	// The position of the camera
	vec2f m_Camera;

	// The boundaries of the area in world space being displayed.
	mat2x2i m_DisplayArea;

	// The number of tiles around the edges that should not be drawn.
	const int m_TileMargin;

	// The current chunk
	Chunk* m_Chunk;

	/// <summary>Resets the transformation.</summary>
	void reset();

	/// <summary>Clamps the display area to the boundaries of the chunk.</summary>
	void clamp_display_area();

public:
	/// <summary>Creates an orthographic world frame.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	/// <param name="tile_margin">The number of tiles around the edges that should not be drawn.</param>
	WorldOrthographicFrame(int x, int y, int width, int height, int tile_margin = 1);

	/// <summary>Sets the boundaries of the frame on the screen.</summary>
	/// <param name="x">The x-coordinate of the frame.</param>
	/// <param name="y">The y-coordinate of the frame.</param>
	/// <param name="width">The width of the frame.</param>
	/// <param name="height">The height of the frame.</param>
	void set_bounds(int x, int y, int width, int height);

	/// <summary>Sets the position of the camera.</summary>
	/// <param name="x">The x-coordinate of the camera.</param>
	/// <param name="y">The y-coordinate of the camera.</param>
	void set_camera(float x, float y);

	/// <summary>Adjusts the position of the camera.</summary>
	/// <param name="dx">The x-axis adjustment.</param>
	/// <param name="dy">The y-axis adjustment.</param>
	void adjust_camera(float dx, float dy);

	/// <summary>Displays the contents of the frame.</summary>
	void display() const;
};





/*
*
*	Main functions
*
*/

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