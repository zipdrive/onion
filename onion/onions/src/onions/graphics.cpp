#include <fstream>
#include <string>
#include <regex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include "../../include/onions/graphics.h"

#include <iostream>


#define GL_DEFAULT_VALUE 0

#define SHADER_KEY int


using namespace std;



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


class SSolidColorGraphic : public SolidColorGraphic
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

public:
	/// <summary>Creates a solid color graphic.</summary>
	/// <param name="r">The red value.</param>
	/// <param name="g">The green value.</param>
	/// <param name="b">The blue value.</param>
	/// <param name="a">The alpha value.</param>
	/// <param name="width">The width of the graphic.</param>
	/// <param name="height">The height of the graphic.</param>
	SSolidColorGraphic(float r, float g, float b, float a, int width, int height) : SolidColorGraphic(vec4f(r, g, b, a), width, height)
	{
		if (!m_Shader)
		{
			m_Shader = new SolidColorShader();
		}
	}

	/// <summary>Draws the graphic to the screen.</summary>
	void display() const
	{
		// Stretch the graphic to match width and height
		mat_push();
		mat_scale(width, height, 1.f);

		// Activate the shader program
		m_Shader->activate(color);

		// Display the graphic
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Pop the transformation
		mat_pop();
	}
};

SSolidColorGraphic::SolidColorShader* SSolidColorGraphic::m_Shader{ nullptr };


SolidColorGraphic* SolidColorGraphic::generate(int r, int g, int b, int a, int width, int height)
{
	return new SSolidColorGraphic(r / 255.f, g / 255.f, b / 255.f, a / 255.f, width, height);
}

SolidColorGraphic* SolidColorGraphic::generate(float r, float g, float b, float a, int width, int height)
{
	return new SSolidColorGraphic(r, g, b, a, width, height);
}

SolidColorGraphic::SolidColorGraphic(const vec4f& color, int width, int height) : color(color), width(width), height(height) {}

int SolidColorGraphic::get_width() const
{
	return width;
}

int SolidColorGraphic::get_height() const
{
	return height;
}



SlicedGraphic::SlicedGraphic(
		Graphic* top_left, Graphic* top, Graphic* top_right,
		Graphic* left, Graphic* center, Graphic* right,
		Graphic* bottom_left, Graphic* bottom, Graphic* bottom_right,
		int width, int height
	) : width(width), height(height)
{
	m_Graphics[0] = top_left;
	m_Graphics[1] = top;
	m_Graphics[2] = top_right;
	m_Graphics[3] = left;
	m_Graphics[4] = center;
	m_Graphics[5] = right;
	m_Graphics[6] = bottom_left;
	m_Graphics[7] = bottom;
	m_Graphics[8] = bottom_right;
}

int SlicedGraphic::get_width() const
{
	return width;
}

int SlicedGraphic::get_height() const
{
	return height;
}

void SlicedGraphic::display() const
{
	// TODO revise later, doesn't work right if corners aren't all the same size

	int lmax; // Maximum right edge of a left-side corner
	int rmin; // Minimum left edge of a right-side corner
	int bmax; // Maximum top edge of a bottom-side corner
	int tmin; // Minimum bottom edge of a top-side corner

	// Display corners
	if (m_Graphics[6]) // Bottom-left corner
	{
		lmax = m_Graphics[6]->get_width();
		bmax = m_Graphics[6]->get_height();

		m_Graphics[6]->display();
	}
	if (m_Graphics[2]) // Top-right corner
	{
		rmin = m_Graphics[2]->get_width();
		tmin = m_Graphics[2]->get_height();

		mat_push();
		mat_translate(width - rmin, height - tmin, 0.f);
		m_Graphics[2]->display();
		mat_pop();
	}
	if (m_Graphics[0]) // Top-left corner
	{
		if (m_Graphics[0]->get_width() > lmax) lmax = m_Graphics[0]->get_width();
		if (m_Graphics[0]->get_height() > tmin) tmin = m_Graphics[0]->get_height();

		mat_push();
		mat_translate(0.f, height - tmin, 0.f);
		m_Graphics[0]->display();
		mat_pop();
	}
	if (m_Graphics[8]) // Bottom-right corner
	{
		if (m_Graphics[8]->get_width() > rmin) rmin = m_Graphics[8]->get_width();
		if (m_Graphics[8]->get_height() > bmax) bmax = m_Graphics[8]->get_height();

		mat_push();
		mat_translate(width - rmin, 0.f, 0.f);
		m_Graphics[8]->display();
		mat_pop();
	}

	// Display edges
	if (m_Graphics[1]) // Top edge
	{
		int l = m_Graphics[0] ? m_Graphics[0]->get_width() : 0;
		int w = width - l - (m_Graphics[2] ? m_Graphics[2]->get_width() : 0);

		mat_push();
		mat_translate(l, height - tmin, 0.f);
		mat_scale(w / (float)m_Graphics[1]->get_width(), tmin / (float)m_Graphics[1]->get_height(), 1.f);
		m_Graphics[1]->display();
		mat_pop();
	}
	if (m_Graphics[3]) // Left edge
	{
		int b = m_Graphics[6] ? m_Graphics[6]->get_height() : 0;
		int h = height - b - (m_Graphics[0] ? m_Graphics[0]->get_height() : 0);

		mat_push();
		mat_translate(0.f, b, 0.f);
		mat_scale(lmax / (float)m_Graphics[3]->get_width(), h / (float)m_Graphics[3]->get_height(), 1.f);
		m_Graphics[3]->display();
		mat_pop();
	}
	if (m_Graphics[5]) // Right edge
	{
		int b = m_Graphics[8] ? m_Graphics[8]->get_height() : 0;
		int h = height - b - (m_Graphics[2] ? m_Graphics[2]->get_height() : 0);

		mat_push();
		mat_translate(width - rmin, b, 0.f);
		mat_scale(rmin / (float)m_Graphics[5]->get_width(), h / (float)m_Graphics[5]->get_height(), 1.f);
		m_Graphics[5]->display();
		mat_pop();
	}
	if (m_Graphics[7]) // Bottom edge
	{
		int l = m_Graphics[6] ? m_Graphics[6]->get_width() : 0;
		int w = width - l - (m_Graphics[8] ? m_Graphics[8]->get_width() : 0);

		mat_push();
		mat_translate(l, 0.f, 0.f);
		mat_scale(w / (float)m_Graphics[7]->get_width(), bmax / (float)m_Graphics[7]->get_height(), 1.f);
		m_Graphics[7]->display();
		mat_pop();
	}

	// Display center
	if (m_Graphics[4]) // Center
	{
		mat_push();
		mat_translate(lmax, bmax, 0.f);
		mat_scale((width - lmax - rmin) / (float)m_Graphics[4]->get_width(), (height - bmax - tmin) / (float)m_Graphics[4]->get_height(), 1.f);
		m_Graphics[4]->display();
		mat_pop();
	}
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
"   vec4 fragColor = tintMatrix * texture(tex2D, UV);\n"
"   if (fragColor.a < 0.1) discard;\n"
"	gl_FragColor = fragColor;\n"
"}";


SpriteSheet::SpriteSheet() {}

// A concrete implementation of a sprite sheet.
class SSpriteSheet : public SpriteSheet
{
protected:
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

		// Create a sprite that is the whole sprite sheet

		// First triangle: bottom-left, bottom-right, top-right
		// Bottom-left corner, vertices
		spriteData.push_back(0.f);
		spriteData.push_back(0.f);
		// Bottom-left corner, tex coord
		spriteData.push_back(0.f);
		spriteData.push_back(0.f);
		// Bottom-right corner, vertices
		spriteData.push_back(width);
		spriteData.push_back(0.f);
		// Bottom-right corner, tex coord
		spriteData.push_back(1.f);
		spriteData.push_back(0.f);
		// Top-right corner, vertices
		spriteData.push_back(width);
		spriteData.push_back(height);
		// Top-right corner, tex coord
		spriteData.push_back(1.f);
		spriteData.push_back(1.f);

		// Second triangle: bottom-left, top-left, top-right
		// Bottom-left corner, vertices
		spriteData.push_back(0.f);
		spriteData.push_back(0.f);
		// Bottom-left corner, tex coord
		spriteData.push_back(0.f);
		spriteData.push_back(0.f);
		// Top-left corner, vertices
		spriteData.push_back(0.f);
		spriteData.push_back(height);
		// Top-left corner, tex coord
		spriteData.push_back(0.f);
		spriteData.push_back(1.f);
		// Top-right corner, vertices
		spriteData.push_back(width);
		spriteData.push_back(height);
		// Top-right corner, tex coord
		spriteData.push_back(1.f);
		spriteData.push_back(1.f);


		// Load the file
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


SpriteSheet* SpriteSheet::generate(const char* path)
{
	SpriteSheet* sheet = new SSpriteSheet();
	sheet->load_sprite_sheet(path);
	return sheet;
}

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


SpriteGraphic::SpriteGraphic(SpriteSheet* sprite_sheet, const mat4x4f& color)
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


StaticSpriteGraphic::StaticSpriteGraphic(SpriteSheet* sprite_sheet, Sprite* sprite, const mat4x4f& color) : SpriteGraphic(sprite_sheet, color)
{
	m_Sprite = sprite;
}

Sprite* StaticSpriteGraphic::get_sprite() const
{
	return m_Sprite;
}


DynamicSpriteGraphic::DynamicSpriteGraphic(SpriteSheet* spriteSheet, const mat4x4f& color) : SpriteGraphic(spriteSheet, color) {}

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



class SpriteFont : public Font, protected SSpriteSheet
{
public:
	void load_sprite_sheet(const char* path)
	{
		// Construct the path to the image file.
		string fpath("res/img/fonts/");
		fpath.append(path);

		// Load the raw sprite sheet
		GLuint tex = load_raw_sprite_sheet(fpath.c_str());

		// Construct the path to the meta file.
		regex fext_finder("(.*)\\.[^\\.]+"); // Regex to find the path excluding file extension
		fpath = regex_replace(fpath, fext_finder, "$1.meta");

		// Load data about sprites from meta file
		std::vector<float> spriteData;

		ifstream meta(fpath.c_str(), ios::in | ios::binary);
		char buffer[9];

		while (meta.good())
		{
			// Pull information about the sprite from file
			meta.read(buffer, 9);

			// The key for the sprite.
			char character = buffer[0];

			// The distance from the left side of the sprite sheet to the left side of the sprite.
			uint16_t left = (unsigned char)buffer[1] | ((uint16_t)(unsigned char)buffer[2] << 8);
			// The distance from the top side of the sprite sheet to the top side of the sprite.
			uint16_t top = (unsigned char)buffer[3] | ((uint16_t)(unsigned char)buffer[4] << 8);
			// The width of the sprite.
			uint16_t sprite_width = (unsigned char)buffer[5] | ((uint16_t)(unsigned char)buffer[6] << 8);
			// The height of the sprite.
			uint16_t sprite_height = (unsigned char)buffer[7] | ((uint16_t)(unsigned char)buffer[8] << 8);

			// Calculate texcoord numbers
			float l = (float)left / width; // left texcoord
			float r = (float)(left + sprite_width) / width; // right texcoord
			float w = (float)sprite_width;

			float t = (float)top / height; // top texcoord
			float b = (float)(top + sprite_height) / height; // bottom texcoord
			float h = (float)sprite_height;

			// Set the sprite data
			m_Characters.emplace(character, Sprite(spriteData.size() / 4, sprite_width, sprite_height));

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

	void display_line(string line, const mat4x4f& color)
	{
		mat_push();

		for (int k = 0; k < line.size(); ++k)
		{
			auto iter = m_Characters.find(line.at(k));

			if (iter != m_Characters.end())
			{
				Sprite& sprite = iter->second;
				display(sprite.key, color);
				mat_translate(sprite.width + m_Kerning, 0.f, 0.f);
			}
			else
			{
				//cout << "Could not find '" << line.at(k) << "'.\n";
				mat_translate(3.f, 0.f, 0.f);
			}
		}

		mat_pop();
	}

	void display_paragraph(std::string text, const mat4x4f& color, int width)
	{
		// TODO
	}
};

Font* Font::load_sprite_font(const char* path)
{
	SpriteFont* font = new SpriteFont();
	font->load_sprite_sheet(path);
	return font;
}



TextLineGraphic::TextLineGraphic(Font* font, string text, const mat4x4f& color) : text(text)
{
	m_Font = font;
	m_Color = color;
}

void TextLineGraphic::display() const
{
	m_Font->display_line(text, m_Color);
}