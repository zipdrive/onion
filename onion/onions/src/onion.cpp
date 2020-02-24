#include <iostream>
#include <forward_list>
#include <fstream>
#include <string>
#include <regex>

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL.h>

#include "..\include\onion.h"

/*
*
*	Definitions
*
*/

#define GL_DEFAULT_VALUE UINT16_MAX

#define MAXIMUM_NUMBER_OF_CHUNKS 100
#define CHUNK_SIZE 32
#define CHUNK_INDEX(x, y) ((x) + (width * (y)))



using namespace std;


/*
*
*	Matrices
*
*/

mat4x4f::mat4x4f() {}

mat4x4f::mat4x4f(float a11, float a12, float a13, float a14,
	float a21, float a22, float a23, float a24,
	float a31, float a32, float a33, float a34)
{
	get(0, 0) = a11;
	get(0, 1) = a12;
	get(0, 2) = a13;
	get(0, 3) = a14;

	get(1, 0) = a21;
	get(1, 1) = a22;
	get(1, 2) = a23;
	get(1, 3) = a24;

	get(2, 0) = a31;
	get(2, 1) = a32;
	get(2, 2) = a33;
	get(2, 3) = a34;
}


// The matrix stack.
std::forward_list<mat4x4f> g_MatrixStack;

mat4x4f& mat_get()
{
	if (g_MatrixStack.empty())
	{
		g_MatrixStack.emplace_front();
	}

	return g_MatrixStack.front();
}

const float* mat_get_values()
{
	return mat_get().matrix_values();
}


void mat_push()
{
	if (g_MatrixStack.empty())
	{
		g_MatrixStack.emplace_front();
	}
	else
	{
		mat4x4f top = g_MatrixStack.front();
		g_MatrixStack.push_front(top);
	}
}

void mat_pop()
{
	g_MatrixStack.pop_front();
}


void mat_custom_transform(mat4x4f& transform)
{
	mat4x4f top = mat_get();
	mat_mul(top, transform, mat_get());
}

void mat_ortho(float left, float right, float bottom, float top, float near, float far)
{
	g_MatrixStack.clear();

	mat4x4f mat;
	mat.set(0, 0, 2.f / (right - left));
	mat.set(1, 1, 2.f / (top - bottom));
	mat.set(2, 2, 2.f / (far - near));
	mat.set(0, 3, (left + right) / (left - right));
	mat.set(1, 3, (bottom + top) / (bottom - top));
	mat.set(2, 3, (near + far) / (near - far));

	g_MatrixStack.push_front(mat);
}

void mat_translate(float dx, float dy, float dz)
{
	mat4x4f trans;
	trans.get(0, 3) = dx;
	trans.get(1, 3) = dy;
	trans.get(2, 3) = dz;

	mat_custom_transform(trans);
}

void mat_scale(float sx, float sy, float sz)
{
	mat4x4f& top = mat_get();
	for (int k = 3; k >= 0; --k)
	{
		top.get(0, k) *= sx;
		top.get(1, k) *= sy;
		top.get(2, k) *= sz;
	}
}

void mat_rotatex(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.get(1, 1) = c;
	rot.get(1, 2) = -s;
	rot.get(2, 1) = s;
	rot.get(2, 2) = c;

	mat_custom_transform(rot);
}

void mat_rotatey(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.get(0, 0) = c;
	rot.get(0, 2) = s;
	rot.get(2, 0) = -s;
	rot.get(2, 2) = c;

	mat_custom_transform(rot);
}

void mat_rotatez(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.get(0, 0) = c;
	rot.get(0, 1) = -s;
	rot.get(1, 0) = s;
	rot.get(1, 1) = c;

	mat_custom_transform(rot);
}




/*
*
*	Graphics
*
*/

// The raw text of the vertex shader for sprites.
const char* spriteVertexShaderText =
"#version 330 core\n"
"\n"
"layout(location = 0) in vec2 vertexPosition;\n"
"layout(location = 1) in vec2 vertexUV;\n"
"\n"
"uniform mat4 MVP;\n"
"\n"
"out vec2 UV;\n"
"\n"
"void main() {\n"
"	gl_Position = MVP * vec4(vertexPosition, 0, 1);\n"
"	UV = vertexUV;\n"
"}";

// The raw text of the fragment shader for sprites.
const char* spriteFragmentShaderText =
"#version 330 core\n"
"\n"
"in vec2 UV;\n"
"\n"
"uniform sampler2D tex2D;\n"
"\n"
"void main() {\n"
"	gl_FragColor = texture(tex2D, UV);\n"
"}";


SpriteInfo::SpriteInfo(int left, int top, int width, int height) : left(left), top(top), width(width), height(height)
{
	key = 0;
}


SpriteSheet::SpriteSheet() {}

// A concrete implementation of a sprite sheet.
class SSpriteSheet : public SpriteSheet
{
private:
	// The ID for which sprite sheet is currently being drawn from.
	static SPRITE_SHEET_KEY m_Enabled;

	// The next available sprite sheet ID.
	static SPRITE_SHEET_KEY m_NextAvailableID;

	// The ID for this sprite sheet.
	SPRITE_SHEET_KEY m_ID;


	// The OpenGL ID for the sprite shader program.
	static GLuint m_SpriteShader;

	// The OpenGL ID for the MVP in the sprite shader program.
	static GLuint m_SpriteShaderMVP;


	// The OpenGL ID for the texture.
	GLuint m_TextureID;

	// An array that contains information about the sprites.
	GLuint m_SpriteBuffer;

	void load_raw_sprite_sheet(const char* path)
	{
		// Load data from file using SOIL.
		unsigned char* data;
		int channels;

		data = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);

		// Bind data to texture.
		glGenTextures(1, &m_TextureID);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// If it hasn't been done yet, generate the shader program.
		if (m_SpriteShader == GL_DEFAULT_VALUE)
		{
			// Vertex shader
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &spriteVertexShaderText, NULL);
			glCompileShader(vertexShader);

			// Fragment shader
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &spriteFragmentShaderText, NULL);
			glCompileShader(fragmentShader);

			// Create shader program
			m_SpriteShader = glCreateProgram();
			glAttachShader(m_SpriteShader, vertexShader);
			glAttachShader(m_SpriteShader, fragmentShader);
			glLinkProgram(m_SpriteShader);

			// Determine the location of the MVP uniform
			m_SpriteShaderMVP = glGetUniformLocation(m_SpriteShader, "MVP");
		}
	}

public:
	/// <summary>Creates an empty sprite sheet.</summary>
	SSpriteSheet()
	{
		// Generate an ID for the sprite sheet.
		m_ID = m_NextAvailableID++;
	}

	/// <summary>Loads sprite sheet from file.</summary>
	/// <param name="path">The path to the image file, using the res/img folder as a base. Note: The path to the meta file should be the same as the path to the image file, but with a .meta file extension instead.</param>
	void load_sprite_sheet(const char* path)
	{
		// Construct the path to the image file.
		string fpath("res/img/");
		fpath.append(path);

		// Load the raw sprite sheet
		load_raw_sprite_sheet(fpath.c_str());

		// Construct the path to the meta file.
		regex fext_finder("(.*)\\.[^\\.]+"); // Regex to find the path excluding file extension
		fpath = regex_replace(fpath, fext_finder, "$1.meta");

		// Load data about sprites from meta file
		// TODO
	}

	/// <summary>Loads sprite sheet from an image file, and equally partitions it into sprites.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base.</param>
	/// <param name="partition_width">The width of the individual sprites.</param>
	/// <param name="partition_height">The height of the individual sprites.</param>
	void load_partitioned_sprite_sheet(const char* path, int partition_width, int partition_height)
	{
		// Construct the path to the image file.
		string fpath("res/img/");
		fpath.append(path);

		// Load the raw sprite sheet
		load_raw_sprite_sheet(fpath.c_str());

		// Figure out how many sprites are across each row and column
		int num_rows = height / partition_height;
		int num_cols = width / partition_width;

		// Construct the raw sprite data
		float* spriteData = new float[24 * num_rows * num_cols];

		for (int y = num_cols - 1; y >= 0; --y)
		{
			for (int x = num_rows - 1; x >= 0; --x)
			{
				float l = (float)(partition_width * x) / width; // left texcoord
				float r = (float)(partition_width * (x + 1)) / width; // right texcoord
				float w = (float)partition_width;

				float t = (float)(partition_height * y) / height; // top texcoord
				float b = (float)(partition_height * (y + 1)) / height; // bottom texcoord
				float h = (float)partition_height;

				// Calculate base index and starting pointer for sprite data
				int i = 24 * (x + (num_rows * y));
				float* data = spriteData + i;

				// First triangle: bottom-left, bottom-right, top-right
				// Bottom-left corner, vertices
				data[0] = 0.0f;
				data[1] = 0.0f;
				// Bottom-left corner, tex coord
				data[2] = l;
				data[3] = b;
				// Bottom-right corner, vertices
				data[4] = w;
				data[5] = 0.0f;
				// Bottom-right corner, tex coord
				data[6] = r;
				data[7] = b;
				// Top-right corner, vertices
				data[8] = w;
				data[9] = h;
				// Top-right corner, tex coord
				data[10] = r;
				data[11] = t;

				// Second triangle: bottom-left, top-left, top-right
				// Bottom-left corner, vertices
				data[12] = 0.0f;
				data[13] = 0.0f;
				// Bottom-left corner, tex coord
				data[14] = l;
				data[15] = b;
				// Top-left corner, vertices
				data[16] = 0.0f;
				data[17] = h;
				// Top-left corner, tex coord
				data[18] = l;
				data[19] = t;
				// Top-right corner, vertices
				data[20] = w;
				data[21] = h;
				// Top-right corner, tex coord
				data[22] = r;
				data[23] = t;
			}
		}

		// Bind the sprite data to a buffer
		glGenBuffers(1, &m_SpriteBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_SpriteBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * num_rows * num_cols, spriteData, GL_STATIC_DRAW);
	}

	/// <summary>Draws a sprite on the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	void display(SPRITE_KEY sprite)
	{
		if (m_ID != m_Enabled) // If not the currently bound sprite sheet, bind all of its data.
		{
			m_Enabled = m_ID;

			// Change the texture being drawn from.
			glBindTexture(GL_TEXTURE_2D, m_TextureID);

			// Change the buffer being used.
			glBindBuffer(GL_ARRAY_BUFFER, m_SpriteBuffer);

			// Set attrib array for position
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);

			// Set attrib array for tex coord
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
		}

		// Set the shader program
		glUseProgram(m_SpriteShader);

		// Set the current transformation
		glUniformMatrix4fv(m_SpriteShaderMVP, 1, GL_FALSE, mat_get_values());

		// Draw the sprite using information from buffer
		glDrawArrays(GL_TRIANGLES, sprite, 6);
	}
};

SPRITE_SHEET_KEY SSpriteSheet::m_Enabled{ INT16_MAX };
SPRITE_SHEET_KEY SSpriteSheet::m_NextAvailableID{ 0 };
GLuint SSpriteSheet::m_SpriteShader{ GL_DEFAULT_VALUE };
GLuint SSpriteSheet::m_SpriteShaderMVP{ GL_DEFAULT_VALUE };


SpriteSheet* SpriteSheet::generate_empty()
{
	return new SSpriteSheet();
}





/*
*
*	Objects and world
*
*/

std::unordered_map<CHUNK_KEY, Chunk*> Chunk::m_Chunks{};
SpriteSheet* Chunk::m_TileSprites{ nullptr };

Chunk* Chunk::get_chunk(CHUNK_KEY key)
{
	auto iter = m_Chunks.find(key);
	if (iter != m_Chunks.end())
		return iter->second;
	else
		return nullptr;
}

Chunk* Chunk::set_chunk(CHUNK_KEY key, const char* path)
{
	auto iter = m_Chunks.find(key);
	if (iter == m_Chunks.end())
	{
		Chunk* chunk = new Chunk(path);
		m_Chunks.emplace(key, chunk);
		return chunk;
	}

	return nullptr; // TODO something else???
}

Chunk::Chunk(const char* path) : m_ChunkFilepath(path)
{
	// Load tile sprites
	if (!m_TileSprites)
	{
		m_TileSprites = SpriteSheet::generate_empty();
		m_TileSprites->load_partitioned_sprite_sheet("tiles.png", TILE_WIDTH, TILE_HEIGHT);
	}

	m_Tiles = nullptr;
	m_Objects = nullptr;
}

void Chunk::load()
{
	if (!m_Objects) // Make sure chunk hasn't already been loaded.
	{
		int s = width * height; // Size of arrays

		// Initialize tiles and objects and set default values
		m_Tiles = new SPRITE_KEY[s];
		memset(m_Tiles, 0, s * sizeof(SPRITE_KEY));

		m_Objects = new Object*[s];
		memset(m_Objects, NULL, s * sizeof(Object*));
	}
}

void Chunk::unload()
{
	if (m_Objects) // Make sure chunk has been loaded previously.
	{
		// Unload tiles
		delete[] m_Tiles;

		// Unload objects
		for (int k = (width * height) - 1; k >= 0; --k)
		{
			if (m_Objects[k])
			{
				delete m_Objects[k];
			}
		}
		delete[] m_Objects;
	}
}

void Chunk::display(int xmin, int ymin, int xmax, int ymax)
{
	if (m_Objects) // Check to make sure the chunk has actually been loaded first
	{
		// Set up transformation for tiles.
		int imin = xmin / TILE_WIDTH;
		int imax = xmax / TILE_WIDTH;
		int jmin = ymin / TILE_HEIGHT;
		int jmax = ymax / TILE_HEIGHT;

		mat_push();
		mat_translate((imin - 1) * TILE_WIDTH, (jmin - 1) * TILE_HEIGHT, 0.f);

		// Draw tiles
		for (int i = imin; i <= imax; ++i)
		{
			mat_translate(TILE_WIDTH, 0.f, 0.f);
			mat_push();

			for (int j = jmin; j <= jmax; ++j)
			{
				mat_translate(0.f, TILE_HEIGHT, 0.f);
				m_TileSprites->display(m_Tiles[CHUNK_INDEX(i, j)]);
			}
			mat_pop();
		}
		mat_pop();
	}
}


World* World::m_World{ nullptr };

World* World::get_world()
{
	if (m_World)
		return m_World;
	return m_World = new World();
}

World::World() :
	m_Transform(
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, -1.f, 0.f,
		0.f, 1.f, 1.f, 0.f
	)
{
	// Load information about all chunks
	for (int k = MAXIMUM_NUMBER_OF_CHUNKS; k > 0; --k)
	{
		std::string filename_to_check("res/data/chunks/chunk");
		filename_to_check.append(to_string(k)).append(".dat");
		const char* filename_to_check_cstr = filename_to_check.c_str();

		std::ifstream file_to_check(filename_to_check_cstr);

		if (file_to_check.good())
		{
			Chunk* chunk = Chunk::set_chunk(k, filename_to_check_cstr);

			// TODO change this
			chunk->width = CHUNK_SIZE;
			chunk->height = CHUNK_SIZE;
		}
	}

	// TODO
	m_Chunk = Chunk::get_chunk(1);
	m_Chunk->load();

	// Set camera
	camera_set(330, 50);
}

void World::camera_set(float x, float y)
{
	// Sets the camera object
	m_Camera.set(0, 0, x);
	m_Camera.set(1, 0, y);

	// Prepares to set transform center and bounds
	Application* app = get_application_settings();

	int cx = (int)roundf(x); // the centered x-coordinate
	int cy = (int)roundf(y); // the centered y-coordinate
	int wHalf = app->width / 2; // half the width of the screen
	int hHalf = app->height / 2; // half the height of the screen
	int wc = (m_Chunk->width * TILE_WIDTH) - 1; // width of chunk, in pixels (minus 1)
	int hc = (m_Chunk->height * TILE_HEIGHT) - 1; // height of chunk, in pixels (minus 1)

	if (cx - wHalf < 0)
	{
		// If left edge would be less than 0, set left edge to be 0.
		m_Bounds.set(0, 0, 0);
		m_Bounds.set(0, 1, 2 * wHalf);
		m_Transform.set(0, 3, -wHalf);
	}
	else if (cx + wHalf >= wc)
	{
		// If right edge would be greater than width of chunk, set right edge to width of chunk.
		m_Bounds.set(0, 0, wc - (2 * wHalf));
		m_Bounds.set(0, 1, wc);
		m_Transform.set(0, 3, wHalf - wc);
	}
	else
	{
		// Set center to camera coordinates
		m_Bounds.set(0, 0, cx - wHalf);
		m_Bounds.set(0, 1, cx + wHalf);
		m_Transform.set(0, 3, -cx);
	}

	if (cy - hHalf < 0)
	{
		// If bottom edge would be less than 0, set bottom edge to be 0.
		m_Bounds.set(1, 0, 0);
		m_Bounds.set(1, 1, 2 * hHalf);
		m_Transform.set(1, 3, -hHalf);
		m_Transform.set(2, 3, -hHalf);
	}
	else if (cy + hHalf >= hc)
	{
		// If top edge would be greater than height of chunk, set top edge to height of chunk.
		m_Bounds.set(1, 0, hc - (2 * hHalf));
		m_Bounds.set(1, 1, hc);
		m_Transform.set(1, 3, hHalf - hc);
		m_Transform.set(2, 3, hHalf - hc);
	}
	else
	{
		m_Bounds.set(1, 0, cy - hHalf);
		m_Bounds.set(1, 1, cy + hHalf);
		m_Transform.set(1, 3, -cy);
		m_Transform.set(2, 3, -cy);
	}
}

void World::camera_adjust(float dx, float dy)
{
	m_Camera.get(0) -= dx;
	m_Camera.get(1) -= dy;
}

void World::display()
{
	// Set up transformation.
	mat_push();
	mat_custom_transform(m_Transform);

	// Draw chunk
	m_Chunk->display(m_Bounds.get(0, 0), m_Bounds.get(1, 0), m_Bounds.get(0, 1), m_Bounds.get(1, 1));

	mat_pop();
}





/*
*
*	Main functions and classes
*
*/

// The main Application object.
Application* g_Application = NULL;

// The application window.
GLFWwindow* g_Window = NULL;


void load_settings()
{
	g_Application = new Application();

	string line;
	ifstream settings("settings.ini");

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
	ofstream settings("settings.ini", ios::out | ios::trunc);

	if (settings.is_open())
	{
		settings << "title = " << g_Application->title;
		settings << "\nwidth = " << g_Application->width;
		settings << "\nheight = " << g_Application->height;
		settings << "\nfullscreen = " << (g_Application->fullscreen ? "true" : "false");

		settings.close();
	}
}


Application* get_application_settings()
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

	// Set the orthogonal projection
	float halfWidth = 0.5f * width;
	float halfHeight = 0.5f * height;
	mat_ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -halfHeight, halfHeight);

	return 0;
}



int onion_init()
{
	// Initialize the GLFW library.
	if (!glfwInit())
	{
		return 1;
	}

	// Load the application settings.
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

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(g_Window);
		glfwTerminate();
		return 1;
	}

	return 0;
}

void onion_main()
{
	World* world = World::get_world();

	while (!glfwWindowShouldClose(g_Window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the world
		world->display();

		// Swap buffers
		glfwSwapBuffers(g_Window);

		// Poll events
		glfwPollEvents();
	}

	// Close everything down.
	glfwDestroyWindow(g_Window);
	glfwTerminate();
}