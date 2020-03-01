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
	float a31, float a32, float a33, float a34,
	float a41, float a42, float a43, float a44)
{
	set(0, 0, a11); 
	set(0, 1, a12);
	set(0, 2, a13);
	set(0, 3, a14);

	set(1, 0, a21);
	set(1, 1, a22);
	set(1, 2, a23);
	set(1, 3, a24);

	set(2, 0, a31);
	set(2, 1, a32);
	set(2, 2, a33);
	set(2, 3, a34);

	set(3, 0, a41);
	set(3, 1, a42);
	set(3, 2, a43);
	set(3, 3, a44);
}


vec4f::vec4f() {}

vec4f::vec4f(float a1, float a2, float a3, float a4)
{
	set(0, 0, a1);
	set(1, 0, a2);
	set(2, 0, a3);
	set(3, 0, a4);
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


void mat_custom_transform(const mat4x4f& transform)
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
	trans.set(0, 3, dx);
	trans.set(1, 3, dy);
	trans.set(2, 3, dz);

	mat_custom_transform(trans);
}

void mat_scale(float sx, float sy, float sz)
{
	mat4x4f& top = mat_get();
	for (int k = 2; k >= 0; --k)
	{
		top(k, 0) *= sx;
		top(k, 1) *= sy;
		top(k, 2) *= sz;
	}
}

void mat_rotatex(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.set(1, 1, c);
	rot.set(1, 2, -s);
	rot.set(2, 1, s);
	rot.set(2, 2, c);

	mat_custom_transform(rot);
}

void mat_rotatey(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.set(0, 0, c);
	rot.set(0, 2, s);
	rot.set(2, 0, -s);
	rot.set(2, 2, c);

	mat_custom_transform(rot);
}

void mat_rotatez(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.set(0, 0, c);
	rot.set(0, 1, -s);
	rot.set(1, 0, s);
	rot.set(1, 1, c);

	mat_custom_transform(rot);
}




/*
*
*	Graphics
*
*/


mat4x4f generate_palette_matrix(
	int rr, int rg, int rb, int ra,
	int gr, int gg, int gb, int ga,
	int br, int bg, int bb, int ba,
	int ar, int ag, int ab, int aa) {

	static float scale = 0.00392157f;
	return mat4x4f(
		rr * scale, gr * scale, br * scale, ar * scale,
		rg * scale, gg * scale, bg * scale, ag * scale,
		rb * scale, gb * scale, bb * scale, ab * scale,
		ra * scale, ga * scale, ba * scale, aa * scale
	);
}


// Handles switching between array buffers.
class Buffer
{
protected:
	// The buffer that's currently bound.
	static GLuint m_ActiveBuffer;

	// The buffer's OpenGL key.
	GLuint m_Buffer;

public:
	/// <summary>Constructs a buffer object.</summary>
	Buffer(GLuint buffer)
	{
		m_Buffer = buffer;
	}

	/// <summary>Checks whether the buffer is the currently activated one.</summary>
	/// <returns>True if the buffer is active, false otherwise.</returns>
	bool is_buffer_active()
	{
		return m_ActiveBuffer == m_Buffer;
	}

	/// <summary>Activates the buffer.</summary>
	virtual void activate()
	{
		if (!is_buffer_active())
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
			m_ActiveBuffer = m_Buffer;
		}
	}
};

GLuint Buffer::m_ActiveBuffer{ GL_DEFAULT_VALUE };


// Handles switching between shader programs.
class Shader
{
protected:
	// The shader that's currently active.
	static GLuint m_ActiveShader;

	// The shader's OpenGL key.
	GLuint m_Shader;

public:
	Shader(GLuint shader)
	{
		m_Shader = shader;
	}

	Shader(const char* rawVertexShaderText, const char* rawFragmentShaderText)
	{
		// Vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &rawVertexShaderText, NULL);
		glCompileShader(vertexShader);

		// Fragment shader
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &rawFragmentShaderText, NULL);
		glCompileShader(fragmentShader);

		// Create shader program
		m_Shader = glCreateProgram();
		glAttachShader(m_Shader, vertexShader);
		glAttachShader(m_Shader, fragmentShader);
		glLinkProgram(m_Shader);
	}

	/// <summary>Retrieves whether this shader is active.</summary>
	/// <returns>True if this shader is active, false otherwise.</returns>
	bool is_shader_active()
	{
		return m_ActiveShader == m_Shader;
	}

	/// <summary>Activates the shader program.</summary>
	virtual void activate()
	{
		if (!is_shader_active())
		{
			glUseProgram(m_Shader);
			m_ActiveShader = m_Shader;
		}
	}
};

GLuint Shader::m_ActiveShader{ GL_DEFAULT_VALUE };


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
	class SolidColorShader : public Shader, public Buffer
	{
	private:
		// The OpenGL ID of the MVP uniform.
		GLuint m_MVP;

		// The OpenGL ID of the color uniform.
		GLuint m_Color;

	public:
		SolidColorShader() : Shader(colorVertexShaderText, colorFragmentShaderText), Buffer(GL_DEFAULT_VALUE)
		{
			// Determine the location of the uniforms
			m_MVP = glGetUniformLocation(m_Shader, "MVP");
			m_Color = glGetUniformLocation(m_Shader, "color");

			// Generate the buffer
			float bufferValues[] = {
				0.f, 0.f,
				1.f, 0.f, 
				1.f, 1.f,

				0.f, 0.f,
				0.f, 1.f, 
				1.f, 1.f
			};

			glGenBuffers(1, &m_Buffer);
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, bufferValues, GL_STATIC_DRAW);
		}

		void activate(const vec4f& color)
		{
			if (!is_shader_active())
			{
				// Activate the shader program
				glUseProgram(m_Shader);
				m_ActiveShader = m_Shader;

				// Change the buffer being used.
				glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
				m_ActiveBuffer = m_Buffer;

				// Set attrib array for position
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (void*)0);
			}

			// Set the current transformation
			glUniformMatrix4fv(m_MVP, 1, GL_FALSE, mat_get_values());

			// Set the color
			glUniform4fv(m_Color, 1, color.matrix_values());
		}
	};

	// The shader program for the graphic
	static SolidColorShader* m_Shader;

	// The color of the graphic
	vec4f m_Color;

	// The width of the graphic
	int m_Width;

	// The height of the graphic
	int m_Height;

public:
	/// <summary>Creates a solid color graphic.</summary>
	/// <param name="r">The red value.</param>
	/// <param name="g">The green value.</param>
	/// <param name="b">The blue value.</param>
	/// <param name="a">The alpha value.</param>
	/// <param name="width">The width of the graphic.</param>
	/// <param name="height">The height of the graphic.</param>
	SolidColorGraphic(float r, float g, float b, float a, int width, int height) : m_Color(r, g, b, a)
	{
		if (!m_Shader)
		{
			m_Shader = new SolidColorShader();
		}

		m_Width = width;
		m_Height = height;
	}

	/// <summary>Retrieves the width of the graphic.</summary>
	/// <returns>The width of the graphic.</returns>
	int get_width() const
	{
		return m_Width;
	}

	/// <summary>Retrieves the height of the graphic.</summary>
	/// <returns>The height of the graphic.</returns>
	int get_height() const
	{
		return m_Height;
	}

	/// <summary>Draws the graphic to the screen.</summary>
	void display() const
	{
		// Stretch the graphic to match width and height
		mat_push();
		mat_scale(m_Width, m_Height, 1.f);

		// Activate the shader program
		m_Shader->activate(m_Color);

		// Display the graphic
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Pop the transformation
		mat_pop();
	}
};

SolidColorGraphic::SolidColorShader* SolidColorGraphic::m_Shader{ nullptr };


Graphic* generate_solid_color_graphic(int r, int g, int b, int a, int width, int height)
{
	return new SolidColorGraphic(r / 255.f, g / 255.f, b / 255.f, a / 255.f, width, height);
}

Graphic* generate_solid_color_graphic(float r, float g, float b, float a, int width, int height)
{
	return new SolidColorGraphic(r, g, b, a, width, height);
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


SpriteSheet::SpriteSheet() {}

// A concrete implementation of a sprite sheet.
class SSpriteSheet : public SpriteSheet
{
private:
	// A shader that displays a sprite texture.
	class SpriteShader : public Shader
	{
	private:
		// The OpenGL ID for the MVP in the sprite shader program.
		GLuint m_MVP;

		// The OpenGL ID for the color tint matrix in the sprite shader program.
		GLuint m_TintMatrix;

	public:
		SpriteShader() : Shader(spriteVertexShaderText, spriteFragmentShaderText)
		{
			// Determine the location of the uniforms
			m_MVP = glGetUniformLocation(m_Shader, "MVP");
			m_TintMatrix = glGetUniformLocation(m_Shader, "tintMatrix");
		}

		void activate(const mat4x4f& tintMatrix)
		{
			// Activate the shader program
			Shader::activate();

			// Set the current transformation
			glUniformMatrix4fv(m_MVP, 1, GL_FALSE, mat_get_values());

			// Set the color tint matrix
			glUniformMatrix4fv(m_TintMatrix, 1, GL_FALSE, tintMatrix.matrix_values());
		}
	};


	// A buffer for the sprite data.
	class SpriteBuffer : public Buffer
	{
	private:
		// The OpenGL ID for the texture.
		GLuint m_Texture;

	public:
		/// <summary>Constructs a sprite buffer.</summary>
		/// <param name="buffer">The OpenGL buffer ID.</param>
		/// <param name="textureID">The OpenGL texture ID.</param>
		SpriteBuffer(GLuint buffer, GLuint texture) : Buffer(buffer)
		{
			m_Texture = texture;
		}

		/// <summary>Frees the buffer and texture.</summary>
		~SpriteBuffer()
		{
			glDeleteBuffers(1, &m_Buffer);
			glDeleteTextures(1, &m_Texture);
		}

		/// <summary>Activates the buffer.</summary>
		void activate()
		{
			if (!is_buffer_active())
			{
				// Change the texture being drawn from.
				glBindTexture(GL_TEXTURE_2D, m_Texture);

				// Change the buffer being used.
				glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
				m_ActiveBuffer = m_Buffer;

				// Set attrib array for position
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);

				// Set attrib array for tex coord
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
			}
		}
	};


	// The shader for all sprite sheets.
	static SpriteShader* m_Shader;

	// The buffer for this specific sprite sheet.
	SpriteBuffer* m_Buffer;


	GLuint load_raw_sprite_sheet(const char* path)
	{
		if (m_Buffer)
		{
			// If the sprite sheet already was loaded, clear it.
			delete m_Buffer;
			m_Buffer = nullptr;
		}

		// Load data from file using SOIL.
		unsigned char* data;
		int channels;

		data = SOIL_load_image(path, &width, &height, &channels, SOIL_LOAD_RGBA);

		// Bind data to texture.
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// If it hasn't been done yet, generate the shader program.
		if (!m_Shader)
		{
			m_Shader = new SpriteShader();
		}

		return tex;
	}

public:
	/// <summary>Loads sprite sheet from file.</summary>
	/// <param name="path">The path to the image file, using the res/img folder as a base. Note: The path to the meta file should be the same as the path to the image file, but with a .meta file extension instead.</param>
	void load_sprite_sheet(const char* path)
	{
		// Construct the path to the image file.
		string fpath("res/img/");
		fpath.append(path);

		// Load the raw sprite sheet
		GLuint tex = load_raw_sprite_sheet(fpath.c_str());

		// Construct the path to the meta file.
		regex fext_finder("(.*)\\.[^\\.]+"); // Regex to find the path excluding file extension
		fpath = regex_replace(fpath, fext_finder, "$1.meta");

		// Load data about sprites from meta file
		std::vector<float> spriteData;

		ifstream meta(fpath.c_str(), ios::in | ios::binary);
		char buffer[10];

		while (meta.good())
		{
			// Pull information about the sprite from file
			meta.read(buffer, 10);

			// The key for the sprite.
			SPRITE_ID id = buffer[0] | ((int16_t)buffer[1] << 8);

			// The distance from the left side of the sprite sheet to the left side of the sprite.
			int16_t left = buffer[2] | ((int16_t)buffer[3] << 8);
			// The distance from the top side of the sprite sheet to the top side of the sprite.
			int16_t top = buffer[4] | ((int16_t)buffer[5] << 8);
			// The width of the sprite.
			int16_t sprite_width = buffer[6] | ((int16_t)buffer[7] << 8);
			// The height of the sprite.
			int16_t sprite_height = buffer[8] | ((int16_t)buffer[9] << 8);

			// Calculate texcoord numbers
			float l = (float)left / width; // left texcoord
			float r = (float)(left + sprite_width) / width; // right texcoord
			float w = (float)sprite_width;

			float t = (float)top / height; // top texcoord
			float b = (float)(top + sprite_height) / height; // bottom texcoord
			float h = (float)sprite_height;

			// Set the sprite data
			Sprite::set_sprite(id, new Sprite(spriteData.size() / 4, sprite_width, sprite_height));

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
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * spriteData.size(), spriteData.data(), GL_STATIC_DRAW);

		// If the buffer already exists, free it
		if (m_Buffer)
		{
			delete m_Buffer;
		}

		// Create the buffer object
		m_Buffer = new SpriteBuffer(buf, tex);
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
		GLuint tex = load_raw_sprite_sheet(fpath.c_str());

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
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24 * num_rows * num_cols, spriteData, GL_STATIC_DRAW);

		// Free the sprite data buffer
		delete[] spriteData;

		// If the buffer already exists, free it
		if (m_Buffer)
		{
			delete m_Buffer;
		}

		// Create the buffer object
		m_Buffer = new SpriteBuffer(buf, tex);
	}

	/// <summary>Draws a sprite on the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	/// <param name="color">The color tint matrix of the sprite.</param>
	void display(SPRITE_KEY sprite, const mat4x4f& color)
	{
		// Activate the shader program
		m_Shader->activate(color);

		// Activate the buffer
		m_Buffer->activate();

		// Draw the sprite using information from buffer
		glDrawArrays(GL_TRIANGLES, sprite, 6);
	}
};

SSpriteSheet::SpriteShader* SSpriteSheet::m_Shader{ nullptr };


SpriteSheet* SpriteSheet::generate_empty()
{
	return new SSpriteSheet();
}



std::unordered_map<SPRITE_ID, Sprite*> Sprite::m_Sprites;

Sprite::Sprite(SPRITE_KEY key, int width, int height) : key(key), width(width), height(height) {}

Sprite* Sprite::get_sprite(SPRITE_ID id)
{
	auto iter = m_Sprites.find(id);
	if (iter != m_Sprites.end())
	{
		return iter->second;
	}
	return nullptr;
}

void Sprite::set_sprite(SPRITE_ID id, Sprite* sprite)
{
	auto iter = m_Sprites.find(id);
	if (iter == m_Sprites.end())
	{
		m_Sprites.emplace(id, sprite);
	}
	else
	{
		m_Sprites.erase(iter);
		m_Sprites.emplace_hint(iter, id, sprite);
	}
}


SpriteGraphic::SpriteGraphic(SpriteSheet* sprite_sheet, mat4x4f& color)
{
	m_SpriteSheet = sprite_sheet;
	m_TintMatrix = color;
}

int SpriteGraphic::get_width() const
{
	return get_sprite()->width;
}

int SpriteGraphic::get_height() const
{
	return get_sprite()->height;
}

void SpriteGraphic::display() const
{
	m_SpriteSheet->display(get_sprite()->key, m_TintMatrix);
}


StaticSpriteGraphic::StaticSpriteGraphic(SpriteSheet* sprite_sheet, Sprite* sprite, mat4x4f& color) : SpriteGraphic(sprite_sheet, color)
{
	m_Sprite = sprite;
}

Sprite* StaticSpriteGraphic::get_sprite() const
{
	return m_Sprite;
}


DynamicSpriteGraphic::DynamicSpriteGraphic(SpriteSheet* spriteSheet, mat4x4f& color) : SpriteGraphic(spriteSheet, color) {}

Sprite* DynamicSpriteGraphic::get_sprite() const
{
	return m_Sprites[m_Current];
}

void DynamicSpriteGraphic::add_frame(Sprite* sprite)
{
	m_Sprites.push_back(sprite);
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
*	Events
*
*/

template <typename EventType>
void StackEventListener<EventType>::push(EventListener<EventType>* listener)
{
	m_Stack.push_back(listener);
}

template <typename EventType>
void StackEventListener<EventType>::pop()
{
	if (!m_Stack.empty())
	{
		m_Stack.pop_back();
	}
}

template <typename EventType>
int StackEventListener<EventType>::trigger(const EventType& event_data)
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



// A listener that keeps track of the mouse state.
class StackMouseListener : public StackMouseMoveListener, public StackMousePressListener
{
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

		return StackMouseMoveListener::trigger(event_data);
	}

	/// <summary>Triggers against a mouse press event.</summary>
	/// <param name="event_data">The data for the event.</param>
	int trigger(const MouseButtonEvent& event_data)
	{
		return StackMousePressListener::trigger(event_data);
	}

	void push(MouseMoveListener* listener)
	{
		StackMouseMoveListener::push(listener);
	}

	void push(MousePressListener* listener)
	{
		StackMousePressListener::push(listener);
	}

	void pop_mouse_move_listener()
	{
		StackMouseMoveListener::pop();
	}

	void pop_mouse_press_listener()
	{
		StackMousePressListener::pop();
	}
};

StackMouseListener g_MouseListener;

void push_mouse_move_listener(MouseMoveListener* listener)
{
	g_MouseListener.push(listener);
}

void pop_mouse_move_listener()
{
	g_MouseListener.pop_mouse_move_listener();
}

void push_mouse_press_listener(MousePressListener* listener)
{
	g_MouseListener.push(listener);
}

void pop_mouse_press_listener()
{
	g_MouseListener.pop_mouse_press_listener();
}




/*
*
*	Frames
*
*/

Frame::Frame() {}

Frame::Frame(int x, int y, int width, int height)
{
	set_bounds(x, y, width, height);
}

void Frame::set_bounds(int x, int y, int width, int height)
{
	m_Bounds.set(0, 0, x);
	m_Bounds.set(1, 0, y);

	m_Bounds.set(0, 1, x + width);
	m_Bounds.set(1, 1, y + height);
}

int Frame::get_width() const
{
	return m_Bounds.get(0, 1) - m_Bounds.get(0, 0);
}

int Frame::get_height() const
{
	return m_Bounds.get(1, 1) - m_Bounds.get(1, 0);
}


ScrollBarFrame::ScrollBarFrame(Graphic* backgroundGraphic, Graphic* arrowGraphic, Graphic* scrollGraphic, int x, int y, bool horizontal)
{
	m_Background = backgroundGraphic;
	m_Arrow = arrowGraphic;
	m_Scroller = scrollGraphic;

	if (horizontal)
	{
		m_Horizontal = true;
		set_bounds(x, y, m_Background->get_width() + (m_Arrow->get_width() * 2), max(m_Background->get_height(), m_Arrow->get_height()));
	}
	else
	{
		m_Horizontal = false;
		set_bounds(x, y, max(m_Background->get_width(), m_Arrow->get_width()), m_Background->get_height() + (m_Arrow->get_height() * 2));
	}

	m_Value = 0.f;
}

float ScrollBarFrame::get_value()
{
	return m_Value;
}

void ScrollBarFrame::set_value(float value)
{
	m_Value = value;
}

int ScrollBarFrame::trigger(const MouseButtonEvent& event_data)
{
	int dx = event_data.x - m_Bounds.get(0, 0);
	int dy = event_data.y - m_Bounds.get(1, 0);

	if (dx >= 0 && dx < get_width() && dy >= 0 && dy < get_height())
	{
		if (m_Horizontal)
		{
			if ((dx -= m_Arrow->get_width()) < 0)
			{
				// Click the left arrow
				set_value(0.f);
			}
			else if (dx < m_Background->get_width())
			{
				// Click in the scroll area
				set_value(
					min(
						max(dx - (m_Scroller->get_width() / 2), 0) 
							/ (float)(m_Background->get_width() - m_Scroller->get_width()),
						1.f
					)
				);
			}
			else
			{
				// Click the right arrow
				set_value(1.f);
			}
		}
		else
		{
			if ((dy -= m_Arrow->get_height()) < 0)
			{
				// Click the bottom arrow
				set_value(0.f);
			}
			else if (dy < m_Background->get_height())
			{
				// Click in the scroll area
				set_value(
					min(
						max(dy - (m_Scroller->get_height() / 2), 0)
							/ (float)(m_Background->get_height() - m_Scroller->get_height()),
						1.f
					)
				);
			}
			else
			{
				// Click the top arrow
				set_value(1.f);
			}
		}

		return EVENT_STOP;
	}

	return EVENT_CONTINUE;
}

void ScrollBarFrame::display() const
{
	// Set up transformation
	mat_push();
	mat_translate(m_Bounds.get(0), m_Bounds.get(1), 0.f);

	if (m_Horizontal)
	{
		// Draw the left arrow
		m_Arrow->display();

		// Draw the background
		mat_translate(m_Arrow->get_width(), 0.f, 0.f);
		m_Background->display();

		// Draw the scroller
		mat_push();
		mat_translate(roundf(m_Value * (m_Background->get_width() - m_Scroller->get_width())), 0.f, -0.1f);
		m_Scroller->display();
		mat_pop();
		
		// Draw the right arrow
		mat_translate(m_Background->get_width() + m_Arrow->get_width(), 0.f, 0.f);
		mat_scale(-1.f, 1.f, 1.f);
		m_Arrow->display();
	}
	else
	{
		// Draw the bottom arrow
		mat_translate(0.f, m_Arrow->get_height(), 0.f);

		mat_push();
		mat_scale(1.f, -1.f, 1.f);
		m_Arrow->display();
		mat_pop();

		// Draw the background
		m_Background->display();

		// Draw the scroller
		mat_push();
		mat_translate(0.f, roundf(m_Value * (m_Background->get_height() - m_Scroller->get_height() - (0.5f * m_Arrow->get_height()))), -0.1f);
		mat_scale(-1.f, 1.f, 1.f);
		mat_rotatez(1.570796f);
		m_Scroller->display();
		mat_pop();

		// Draw the top arrow
		mat_translate(0.f, m_Background->get_height(), 0.f);
		m_Arrow->display();
	}

	mat_pop();
}


LayerFrame::LayerFrame()
{
	Application* app = get_application_settings();
	set_bounds(0, 0, app->width, app->height);
}

LayerFrame::LayerFrame(int x, int y, int width, int height)
{
	set_bounds(x, y, width, height);
}

void LayerFrame::reset()
{
	if (m_Sequence.empty())
	{
		m_ZScale = 1.f;
	}
	else
	{
		m_ZScale = 1.f / m_Sequence.size();
	}

	m_Transform.set(2, 2, m_ZScale);
	m_Transform.set(2, 3, 1.f - m_ZScale);
}

void LayerFrame::insert_top(Graphic* graphic)
{
	m_Sequence.push_back(graphic);
	this->reset();
}

void LayerFrame::display() const
{
	// Set up the transformation
	mat_push();
	mat_custom_transform(m_Transform);

	// Display the layers from back to front
	float dz = -2.f * m_ZScale;
	for (auto iter = m_Sequence.begin(); iter != m_Sequence.end(); ++iter)
	{
		(*iter)->display();
		mat_translate(0.f, 0.f, dz);
	}

	mat_pop();
}


UIFrame::UIFrame() : LayerFrame() {}

void UIFrame::reset()
{
	LayerFrame::reset();

	Application* app = get_application_settings();
	float sx = 2.f / app->width;
	float sy = 2.f / app->height;

	m_Transform.set(0, 0, sx);
	m_Transform.set(1, 1, sy);
	m_Transform.set(0, 3, (sx * m_Bounds.get(0, 0)) - 1.f);
	m_Transform.set(1, 3, (sy * m_Bounds.get(1, 0)) - 1.f);
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

	int width = m_Bounds.get(0, 1) - m_Bounds.get(0, 0);
	int height = m_Bounds.get(1, 1) - m_Bounds.get(1, 0);
	int halfWidth = (width + 1) / 2;
	int halfHeight = (height + 1) / 2;

	m_DisplayArea.set(0, 0, x - halfWidth);
	m_DisplayArea.set(0, 1, x + (width / 2) - 1);
	m_DisplayArea.set(1, 0, y - halfHeight);
	m_DisplayArea.set(1, 1, y + (height / 2) - 1);
	clamp_display_area();

	// Construct the transformation matrix
	Application* app = get_application_settings();
	float sx = 2.f / app->width; // The x-axis scale of the projection
	float sy = 2.f / app->height; // The y-axis scale of the projection

	float bx = m_Bounds.get(0, 0) + halfWidth;
	float by = m_Bounds.get(1, 0) + halfHeight;

	int mx = 1 + ((m_Bounds.get(0, 1) - m_Bounds.get(0, 0)) % 2);
	int my = 1 + ((m_Bounds.get(1, 1) - m_Bounds.get(1, 0)) % 2);
	float cx = -sx * ((m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1) + mx) / 2);
	float cy = -sy * ((m_DisplayArea.get(1, 0) + m_DisplayArea.get(1, 1) + my) / 2);

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
		m_DisplayArea(0, 1) += xleft;
		m_DisplayArea.set(0, 0, xmin);
		m_Camera.set(0, 0, (m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1)) / 2);
	}
	else if (xright < 0)
	{
		m_DisplayArea(0, 0) += xright;
		m_DisplayArea.set(0, 1, xmax);
		m_Camera.set(0, 0, (m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1)) / 2);
	}

	int ymin = TILE_HEIGHT * m_TileMargin;
	int ymax = (m_Chunk->height * TILE_HEIGHT) - (ymin + 1);
	int ybottom = ymin - m_DisplayArea.get(1, 0);
	int ytop = ymax - m_DisplayArea.get(1, 1);
	if (ybottom > 0)
	{
		m_DisplayArea(1, 1) += ybottom;
		m_DisplayArea.set(1, 0, ymin);
		m_Camera.set(1, 0, (m_DisplayArea.get(1, 0) + m_DisplayArea.get(1, 1)) / 2);
	}
	else if (ytop < 0)
	{
		m_DisplayArea(1, 0) += ytop;
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
	float& cx = m_Camera(0);
	float& cy = m_Camera(1);
	
	int former_x = roundf(cx);
	int former_y = roundf(cy);

	cx += dx;
	cy += dy;

	int dcx = (int)roundf(cx) - former_x;
	if (dcx)
	{
		m_DisplayArea(0, 0) += dcx;
		m_DisplayArea(0, 1) += dcx;
	}

	int dcy = (int)roundf(cy) - former_y;
	if (dcy)
	{
		m_DisplayArea(1, 0) += dcy;
		m_DisplayArea(1, 1) += dcy;
	}

	if (dcx || dcy)
	{
		clamp_display_area();
	}
}

void WorldOrthographicFrame::display() const
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
		MouseButtonEvent event_data = { g_MouseListener.x, g_MouseListener.y, button, mods };

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