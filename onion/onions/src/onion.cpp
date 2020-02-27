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

#define GL_DEFAULT_VALUE 0

#define MAXIMUM_NUMBER_OF_CHUNKS 100
#define CHUNK_SIZE 32
#define CHUNK_INDEX(x, y) ((x) + (width * (y)))

#define SHADER_KEY int



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


vec4f::vec4f() {}

vec4f::vec4f(float a1, float a2, float a3, float a4)
{
	get(0) = a1;
	get(1) = a2;
	get(2) = a3;
	get(3) = a4;
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


class Shader
{
private:
	// The shader that's currently enabled.
	static SHADER_KEY m_Enabled;

	// The shader's key.
	SHADER_KEY m_Key;

protected:
	bool is_enabled()
	{
		return m_Enabled == m_Key;
	}

	void enable()
	{

	}
};


// The raw text of the vertex shader for solid colors.
const char* colorVertexShaderText =
"#version 330 core\n"
"layout(location = 0) in vec2 vertexPosition;\n"
"uniform mat4 MVP;\n"
"void main() {\n"
"	gl_Position = MVP * vec4(vertexPosition, 0, 1);\n"
"}";

// The raw text of the fragment shader for solid colors.
const char* colorFragmentShaderText =
"#version 330 core\n"
"uniform vec4 color;\n"
"void main() {\n"
"	gl_FragColor = color;\n"
"}";


class SolidColorGraphic : public Graphic
{
private:
	// The color of the graphic
	vec4f m_Color;

public:
	/// <summary>Creates a solid color graphic.</summary>
	/// <param name="r">The red value.</param>
	/// <param name="g">The green value.</param>
	/// <param name="b">The blue value.</param>
	/// <param name="a">The alpha value.</param>
	SolidColorGraphic(float r, float g, float b, float a) : m_Color(r, g, b, a)
	{

	}

	/// <summary>Draws the graphic to the screen.</summary>
	void display()
	{

	}
};

Graphic* generate_solid_color_graphic(int r, int g, int b, int a)
{
	return new SolidColorGraphic(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

Graphic* generate_solid_color_graphic(float r, float g, float b, float a)
{
	return new SolidColorGraphic(r, g, b, a);
}



// The raw text of the vertex shader for sprites.
const char* spriteVertexShaderText =
"#version 330 core\n"
"layout(location = 0) in vec2 vertexPosition;\n"
"layout(location = 1) in vec2 vertexUV;\n"
"uniform mat4 MVP;\n"
"out vec2 UV;\n"
"void main() {\n"
"	gl_Position = MVP * vec4(vertexPosition, 0, 1);\n"
"	UV = vertexUV;\n"
"}";

// The raw text of the fragment shader for sprites.
const char* spriteFragmentShaderText =
"#version 330 core\n"
"in vec2 UV;\n"
"uniform mat4 tintMatrix;\n"
"uniform sampler2D tex2D;\n"
"void main() {\n"
"	gl_FragColor = tintMatrix * texture(tex2D, UV);\n"
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

	// The OpenGL ID for the color tint matrix in the sprite shader program.
	static GLuint m_SpriteShaderTint;


	// The OpenGL ID for the texture.
	GLuint m_TextureID;

	// An array that contains information about the sprites.
	GLuint m_SpriteBuffer;

	void load_raw_sprite_sheet(const char* path)
	{
		if (m_TextureID != GL_DEFAULT_VALUE)
		{
			// If the sprite sheet already was loaded, clear it.
			glDeleteTextures(1, &m_TextureID);
		}

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

			// Determine the location of the uniforms
			m_SpriteShaderMVP = glGetUniformLocation(m_SpriteShader, "MVP");
			m_SpriteShaderTint = glGetUniformLocation(m_SpriteShader, "tintMatrix");
		}
	}

public:
	/// <summary>Creates an empty sprite sheet.</summary>
	SSpriteSheet()
	{
		// Generate an ID for the sprite sheet.
		m_ID = m_NextAvailableID++;

		// Set OpenGL IDs to defaults.
		m_TextureID = GL_DEFAULT_VALUE;
		m_SpriteBuffer = GL_DEFAULT_VALUE;
	}

	/// <summary>Loads sprite sheet from file.</summary>
	/// <param name="path">The path to the image file, using the res/img folder as a base. Note: The path to the meta file should be the same as the path to the image file, but with a .meta file extension instead.</param>
	void load_sprite_sheet(const char* path)
	{
		if (m_SpriteBuffer != GL_DEFAULT_VALUE)
		{
			// If the sprite sheet already was loaded, clear it.
			glDeleteBuffers(1, &m_SpriteBuffer);
		}

		// Construct the path to the image file.
		string fpath("res/img/");
		fpath.append(path);

		// Load the raw sprite sheet
		load_raw_sprite_sheet(fpath.c_str());

		// Construct the path to the meta file.
		regex fext_finder("(.*)\\.[^\\.]+"); // Regex to find the path excluding file extension
		fpath = regex_replace(fpath, fext_finder, "$1.meta");

		// Load data about sprites from meta file
		std::vector<float> spriteData;

		ifstream meta(fpath.c_str(), ios::in | ios::binary);

		while (meta.good())
		{
			// Pull information about the sprite from file
			int16_t left; // The distance from the left side of the sprite sheet to the left side of the sprite
			meta >> left;
			int16_t top; // The distance from the top side of the sprite sheet to the top side of the sprite
			meta >> top;
			int16_t sprite_width; // The width of the sprite
			meta >> sprite_width;
			int16_t sprite_height; // The height of the sprite
			meta >> sprite_height;

			// Debug: Print values
			cout << "(" << left << ", " << top << ") -> (" << (left + sprite_width) << ", " << (top + sprite_height) << ")\n";

			// Calculate texcoord numbers
			float l = (float)left / width; // left texcoord
			float r = (float)(left + sprite_width) / width; // right texcoord
			float w = (float)sprite_width;

			float t = (float)top / height; // top texcoord
			float b = (float)(top + sprite_height) / height; // bottom texcoord
			float h = (float)sprite_height;

			// First triangle: bottom-left, bottom-right, top-right
			// Bottom-left corner, vertices
			spriteData.push_back(0.0f);
			spriteData.push_back(0.0f);
			// Bottom-left corner, tex coord
			spriteData.push_back(l);
			spriteData.push_back(b);
			// Bottom-right corner, vertices
			spriteData.push_back(w);
			spriteData.push_back(0.0f);
			// Bottom-right corner, tex coord
			spriteData.push_back(r);
			spriteData.push_back(b);
			// Top-right corner, vertices
			spriteData.push_back(w);
			spriteData.push_back(h);
			// Top-right corner, tex coord
			spriteData.push_back(r);
			spriteData.push_back(t);

			// Second triangle: bottom-left, top-left, top-right
			// Bottom-left corner, vertices
			spriteData.push_back(0.0f);
			spriteData.push_back(0.0f);
			// Bottom-left corner, tex coord
			spriteData.push_back(l);
			spriteData.push_back(b);
			// Top-left corner, vertices
			spriteData.push_back(0.0f);
			spriteData.push_back(h);
			// Top-left corner, tex coord
			spriteData.push_back(l);
			spriteData.push_back(t);
			// Top-right corner, vertices
			spriteData.push_back(w);
			spriteData.push_back(h);
			// Top-right corner, tex coord
			spriteData.push_back(r);
			spriteData.push_back(t);
		}

		// Close the meta file
		meta.close();

		// Bind the sprite data to a buffer
		glGenBuffers(1, &m_SpriteBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_SpriteBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * spriteData.size(), spriteData.data(), GL_STATIC_DRAW);
	}

	/// <summary>Loads sprite sheet from an image file, and equally partitions it into sprites.</summary>
	/// <param name="path">The path to the image file, using the res/img/ folder as a base.</param>
	/// <param name="partition_width">The width of the individual sprites.</param>
	/// <param name="partition_height">The height of the individual sprites.</param>
	void load_partitioned_sprite_sheet(const char* path, int partition_width, int partition_height)
	{
		if (m_SpriteBuffer != GL_DEFAULT_VALUE)
		{
			// If the sprite sheet already was loaded, clear it.
			glDeleteBuffers(1, &m_SpriteBuffer);
		}

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

		// Free the sprite data buffer
		delete[] spriteData;
	}

	/// <summary>Draws a sprite on the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	/// <param name="color">The color tint matrix of the sprite.</param>
	void display(SPRITE_KEY sprite, const mat4x4f& color)
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

		// Set the color tint matrix
		glUniformMatrix4fv(m_SpriteShaderTint, 1, GL_FALSE, color.matrix_values());

		// Draw the sprite using information from buffer
		glDrawArrays(GL_TRIANGLES, sprite, 6);
	}
};

SPRITE_SHEET_KEY SSpriteSheet::m_Enabled{ INT16_MAX };
SPRITE_SHEET_KEY SSpriteSheet::m_NextAvailableID{ 0 };
GLuint SSpriteSheet::m_SpriteShader{ GL_DEFAULT_VALUE };
GLuint SSpriteSheet::m_SpriteShaderMVP{ GL_DEFAULT_VALUE };
GLuint SSpriteSheet::m_SpriteShaderTint{ GL_DEFAULT_VALUE };


SpriteSheet* SpriteSheet::generate_empty()
{
	return new SSpriteSheet();
}


SpriteGraphic::SpriteGraphic(SpriteSheet* spriteSheet, mat4x4f& color)
{
	m_SpriteSheet = spriteSheet;
	m_TintMatrix = color;
}

void SpriteGraphic::display()
{
	m_SpriteSheet->display(get_sprite(), m_TintMatrix);
}


StaticSpriteGraphic::StaticSpriteGraphic(SpriteSheet* spriteSheet, SPRITE_KEY key, mat4x4f& color) : SpriteGraphic(spriteSheet, color)
{
	m_Key = key;
}

SPRITE_KEY StaticSpriteGraphic::get_sprite()
{
	return m_Key;
}


DynamicSpriteGraphic::DynamicSpriteGraphic(SpriteSheet* spriteSheet, mat4x4f& color) : SpriteGraphic(spriteSheet, color) {}

SPRITE_KEY DynamicSpriteGraphic::get_sprite()
{
	return m_Keys[m_Current];
}

void DynamicSpriteGraphic::add_frame(SPRITE_KEY key)
{
	m_Keys.push_back(key);
}

void DynamicSpriteGraphic::set_frame(int frame)
{
	m_Current = frame;
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

		// Initialize tiles
		for (int i = width - 1; i >= 0; --i)
		{
			for (int j = height - 1; j >= 0; --j)
			{
				m_Tiles[CHUNK_INDEX(i, j)] = 6 * ((i + j) % 4);
			}
		}
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
	const mat4x4f identityMatrix;

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
				m_TileSprites->display(m_Tiles[CHUNK_INDEX(i, j)], identityMatrix);
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

Chunk* World::get_chunk()
{
	return get_world()->m_Chunk;
}

World::World()
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
}




/*
*
*	Frames
*
*/

void Frame::set_bounds(int x, int y, int width, int height)
{
	m_Bounds.set(0, 0, x);
	m_Bounds.set(1, 0, y);

	m_Bounds.set(0, 1, x + width);
	m_Bounds.set(1, 1, y + height);
}


WorldOrthographicFrame::WorldOrthographicFrame(int x, int y, int width, int height, int tile_margin) : m_TileMargin(tile_margin)
{
	set_bounds(x, y, width, height);
}

void WorldOrthographicFrame::reset()
{
	// Calculate the world space area
	m_Chunk = World::get_chunk();

	int x = roundf(m_Camera.get(0));
	int y = roundf(m_Camera.get(1));
	int halfWidth = (m_Bounds.get(0, 1) - m_Bounds.get(0, 0)) / 2;
	int halfHeight = (m_Bounds.get(1, 1) - m_Bounds.get(1, 0)) / 2;

	m_DisplayArea.set(0, 0, x - halfWidth);
	m_DisplayArea.set(0, 1, x + halfWidth - 1);
	m_DisplayArea.set(1, 0, y - halfHeight);
	m_DisplayArea.set(1, 1, y + halfHeight - 1);
	clamp_display_area();

	// Construct the transformation matrix
	Application* app = get_application_settings();
	float sx = 2.f / app->width; // The x-axis scale of the projection
	float sy = 2.f / app->height; // The y-axis scale of the projection

	float bx = 0.5f * (m_Bounds.get(0, 0) + m_Bounds.get(0, 1));
	float by = 0.5f * (m_Bounds.get(1, 0) + m_Bounds.get(1, 1));

	float cx = -sx * 0.5f * (m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1));
	float cy = -sy * 0.5f * (m_DisplayArea.get(1, 0) + m_DisplayArea.get(1, 1));

	m_Transform = mat4x4f(
		sx, 0.f, 0.f, (sx * bx) - 1.f + cx,
		0.f, sy, -sy, (sy * by) - 1.f + cy,
		0.f, sy, sy, cy
	);
}

void WorldOrthographicFrame::clamp_display_area()
{
	int xmin = TILE_WIDTH * m_TileMargin;
	int xmax = (m_Chunk->width * TILE_WIDTH) - (xmin + 1);
	int xleft = xmin - m_DisplayArea.get(0, 0);
	int xright = xmax - m_DisplayArea.get(0, 1);
	if (xleft > 0)
	{
		m_DisplayArea.get(0, 1) += xleft;
		m_DisplayArea.set(0, 0, xmin);
		m_Camera.set(0, 0, (m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1)) / 2);
	}
	else if (xright < 0)
	{
		m_DisplayArea.get(0, 0) += xright;
		m_DisplayArea.set(0, 1, xmax);
		m_Camera.set(0, 0, (m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1)) / 2);
	}

	int ymin = TILE_HEIGHT * m_TileMargin;
	int ymax = (m_Chunk->height * TILE_HEIGHT) - (ymin + 1);
	int ybottom = ymin - m_DisplayArea.get(1, 0);
	int ytop = ymax - m_DisplayArea.get(1, 1);
	if (ybottom > 0)
	{
		m_DisplayArea.get(1, 1) += ybottom;
		m_DisplayArea.set(1, 0, ymin);
		m_Camera.set(1, 0, (m_DisplayArea.get(1, 0) + m_DisplayArea.get(1, 1)) / 2);
	}
	else if (ytop < 0)
	{
		m_DisplayArea.get(1, 0) += ytop;
		m_DisplayArea.set(1, 1, ymax);
		m_Camera.set(1, 0, (m_DisplayArea.get(1, 0) + m_DisplayArea.get(1, 1)) / 2);
	}
}

void WorldOrthographicFrame::set_bounds(int x, int y, int width, int height)
{
	Frame::set_bounds(x, y, width, height);
	reset();
}

void WorldOrthographicFrame::set_camera(float x, float y)
{
	m_Camera.set(0, 0, x);
	m_Camera.set(1, 0, y);
	reset();
}

void WorldOrthographicFrame::adjust_camera(float dx, float dy)
{
	float& cx = m_Camera.get(0);
	float& cy = m_Camera.get(1);
	
	int former_x = roundf(cx);
	int former_y = roundf(cy);

	cx += dx;
	cy += dy;

	int dcx = (int)roundf(cx) - former_x;
	if (dcx)
	{
		m_DisplayArea.get(0, 0) += dcx;
		m_DisplayArea.get(0, 1) += dcx;
	}

	int dcy = (int)roundf(cy) - former_y;
	if (dcy)
	{
		m_DisplayArea.get(1, 0) += dcy;
		m_DisplayArea.get(1, 1) += dcy;
	}

	if (dcx || dcy)
	{
		clamp_display_area();
	}
}

void WorldOrthographicFrame::display()
{
	// Set up transformation.
	mat_push();
	mat_custom_transform(m_Transform);

	// Draw chunk
	m_Chunk->display(
		m_DisplayArea.get(0, 0), m_DisplayArea.get(1, 0), 
		m_DisplayArea.get(0, 1), m_DisplayArea.get(1, 1)
	);

	mat_pop();
}





/*
*
*	Main functions and classes
*
*/

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
	while (!glfwWindowShouldClose(g_Window))
	{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

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