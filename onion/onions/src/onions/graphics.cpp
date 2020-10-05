#include <fstream>
#include <string>
#include <regex>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include "../../include/onions/fileio.h"
#include "../../include/onions/graphics.h"
#include "../../include/onions/graphics/frame.h"
#include "../../include/onions/application.h"

#include <iostream>


#define GL_DEFAULT_VALUE 0

#define SHADER_KEY int


using namespace std;
using namespace onion;



/*

#define RGB_INT_TO_FLOAT	0.00392157f

void Palette::generate_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to, PALETTE_MATRIX& matrix)
{
	matrix.set(0, 0, RGB_INT_TO_FLOAT * red_maps_to.get(0));
	matrix.set(1, 0, RGB_INT_TO_FLOAT * red_maps_to.get(1));
	matrix.set(2, 0, RGB_INT_TO_FLOAT * red_maps_to.get(2));
	matrix.set(3, 0, RGB_INT_TO_FLOAT * red_maps_to.get(3));

	matrix.set(0, 1, RGB_INT_TO_FLOAT * green_maps_to.get(0));
	matrix.set(1, 1, RGB_INT_TO_FLOAT * green_maps_to.get(1));
	matrix.set(2, 1, RGB_INT_TO_FLOAT * green_maps_to.get(2));
	matrix.set(3, 1, RGB_INT_TO_FLOAT * green_maps_to.get(3));

	matrix.set(0, 2, RGB_INT_TO_FLOAT * blue_maps_to.get(0));
	matrix.set(1, 2, RGB_INT_TO_FLOAT * blue_maps_to.get(1));
	matrix.set(2, 2, RGB_INT_TO_FLOAT * blue_maps_to.get(2));
	matrix.set(3, 2, RGB_INT_TO_FLOAT * blue_maps_to.get(3));

	matrix.set(0, 3, 0.f);
	matrix.set(1, 3, 0.f);
	matrix.set(2, 3, 0.f);
	matrix.set(3, 3, 1.f);
}

void Palette::generate_palette_matrix(const vec4f& red_maps_to, const vec4f& green_maps_to, const vec4f& blue_maps_to, PALETTE_MATRIX& matrix)
{
	matrix.set(0, 0, red_maps_to.get(0));
	matrix.set(1, 0, red_maps_to.get(1));
	matrix.set(2, 0, red_maps_to.get(2));
	matrix.set(3, 0, red_maps_to.get(3));

	matrix.set(0, 1, green_maps_to.get(0));
	matrix.set(1, 1, green_maps_to.get(1));
	matrix.set(2, 1, green_maps_to.get(2));
	matrix.set(3, 1, green_maps_to.get(3));

	matrix.set(0, 2, blue_maps_to.get(0));
	matrix.set(1, 2, blue_maps_to.get(1));
	matrix.set(2, 2, blue_maps_to.get(2));
	matrix.set(3, 2, blue_maps_to.get(3));

	matrix.set(0, 3, 0.f);
	matrix.set(1, 3, 0.f);
	matrix.set(2, 3, 0.f);
	matrix.set(3, 3, 1.f);
}



SinglePalette::SinglePalette(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
{
	generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_Matrix);
}

SinglePalette::SinglePalette(const vec4f& red_maps_to, const vec4f& green_maps_to, const vec4f& blue_maps_to)
{
	generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_Matrix);
}

const PALETTE_MATRIX& SinglePalette::get_red_palette_matrix() const
{
	return m_Matrix;
}

const PALETTE_MATRIX& SinglePalette::get_green_palette_matrix() const
{
	return m_Matrix;
}

const PALETTE_MATRIX& SinglePalette::get_blue_palette_matrix() const
{
	return m_Matrix;
}

void SinglePalette::get_red_maps_to(vec4f& color) const
{
	color = vec4f(
		m_Matrix.get(0, 0),
		m_Matrix.get(1, 0),
		m_Matrix.get(2, 0),
		m_Matrix.get(3, 0)
	);
}

void SinglePalette::get_green_maps_to(vec4f& color) const
{
	color = vec4f(
		m_Matrix.get(0, 1),
		m_Matrix.get(1, 1),
		m_Matrix.get(2, 1),
		m_Matrix.get(3, 1)
	);
}

void SinglePalette::get_blue_maps_to(vec4f& color) const
{
	color = vec4f(
		m_Matrix.get(0, 2),
		m_Matrix.get(1, 2),
		m_Matrix.get(2, 2),
		m_Matrix.get(3, 2)
	);
}

void SinglePalette::set_red_maps_to(const vec4i& red_maps_to)
{
	set_red_maps_to(vec4f(RGB_INT_TO_FLOAT * vec4f(red_maps_to)));
}

void SinglePalette::set_red_maps_to(const vec4f& red_maps_to)
{
	m_Matrix.set(0, 0, red_maps_to.get(0));
	m_Matrix.set(1, 0, red_maps_to.get(1));
	m_Matrix.set(2, 0, red_maps_to.get(2));
	m_Matrix.set(3, 0, red_maps_to.get(3));
}

void SinglePalette::set_green_maps_to(const vec4i& green_maps_to)
{
	set_green_maps_to(vec4f(RGB_INT_TO_FLOAT * vec4f(green_maps_to)));
}

void SinglePalette::set_green_maps_to(const vec4f& green_maps_to)
{
	m_Matrix.set(0, 1, green_maps_to.get(0));
	m_Matrix.set(1, 1, green_maps_to.get(1));
	m_Matrix.set(2, 1, green_maps_to.get(2));
	m_Matrix.set(3, 1, green_maps_to.get(3));
}

void SinglePalette::set_blue_maps_to(const vec4i& blue_maps_to)
{
	set_blue_maps_to(vec4f(RGB_INT_TO_FLOAT * vec4f(blue_maps_to)));
}

void SinglePalette::set_blue_maps_to(const vec4f& blue_maps_to)
{
	m_Matrix.set(0, 2, blue_maps_to.get(0));
	m_Matrix.set(1, 2, blue_maps_to.get(1));
	m_Matrix.set(2, 2, blue_maps_to.get(2));
	m_Matrix.set(3, 2, blue_maps_to.get(3));
}



MultiplePalette::MultiplePalette()
{
	generate_palette_matrix(vec4i(255, 0, 0, 0), vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), m_RedMatrix);
	generate_palette_matrix(vec4i(0, 255, 0, 0), vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), m_GreenMatrix);
	generate_palette_matrix(vec4i(0, 0, 255, 0), vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), m_BlueMatrix);
}

MultiplePalette::MultiplePalette(const vec4i& red_maps_red_maps_to, const vec4i& red_maps_green_maps_to, const vec4i& red_maps_blue_maps_to,
	const vec4i& green_maps_red_maps_to, const vec4i& green_maps_green_maps_to, const vec4i& green_maps_blue_maps_to, 
	const vec4i& blue_maps_red_maps_to, const vec4i& blue_maps_green_maps_to, const vec4i& blue_maps_blue_maps_to)
{
	generate_palette_matrix(red_maps_red_maps_to, red_maps_green_maps_to, red_maps_blue_maps_to, m_RedMatrix);
	generate_palette_matrix(green_maps_red_maps_to, green_maps_green_maps_to, green_maps_blue_maps_to, m_GreenMatrix);
	generate_palette_matrix(blue_maps_red_maps_to, blue_maps_green_maps_to, blue_maps_blue_maps_to, m_BlueMatrix);
}

MultiplePalette::MultiplePalette(const vec4f& red_maps_red_maps_to, const vec4f& red_maps_green_maps_to, const vec4f& red_maps_blue_maps_to,
	const vec4f& green_maps_red_maps_to, const vec4f& green_maps_green_maps_to, const vec4f& green_maps_blue_maps_to,
	const vec4f& blue_maps_red_maps_to, const vec4f& blue_maps_green_maps_to, const vec4f& blue_maps_blue_maps_to)
{
	generate_palette_matrix(red_maps_red_maps_to, red_maps_green_maps_to, red_maps_blue_maps_to, m_RedMatrix);
	generate_palette_matrix(green_maps_red_maps_to, green_maps_green_maps_to, green_maps_blue_maps_to, m_GreenMatrix);
	generate_palette_matrix(blue_maps_red_maps_to, blue_maps_green_maps_to, blue_maps_blue_maps_to, m_BlueMatrix);
}

const PALETTE_MATRIX& MultiplePalette::get_red_palette_matrix() const
{
	return m_RedMatrix;
}

void MultiplePalette::set_red_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
{
	generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_RedMatrix);
}

const PALETTE_MATRIX& MultiplePalette::get_green_palette_matrix() const
{
	return m_GreenMatrix;
}

void MultiplePalette::set_green_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
{
	generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_GreenMatrix);
}

const PALETTE_MATRIX& MultiplePalette::get_blue_palette_matrix() const
{
	return m_BlueMatrix;
}

void MultiplePalette::set_blue_palette_matrix(const vec4i& red_maps_to, const vec4i& green_maps_to, const vec4i& blue_maps_to)
{
	generate_palette_matrix(red_maps_to, green_maps_to, blue_maps_to, m_BlueMatrix);
}


// Handles switching between array buffers.
class Buffer
{
protected:
	// The buffer that's currently bound.
	static GLuint m_ActiveBuffer;

	// The clipping rectangle for the shader, in (-1, -1) to (+1, +1) coordinates.
	static mat2x2f m_ClippingRect;

	// The buffer's OpenGL key.
	GLuint m_Buffer;

public:
	/// <summary>Sets the clipping rectangle for the buffer.</summary>
	/// <param name="rect">The bounds of the clipping rectangle.</param>
	static void set_clipping_rectangle(const mat2x2f& rect)
	{
		const mat4x4f& trans = mat_get();

		m_ClippingRect.set(0, 0, (rect.get(0, 0) * trans.get(0, 0)) + (rect.get(1, 0) * trans.get(0, 1)) + trans.get(0, 3));
		m_ClippingRect.set(0, 1, (rect.get(0, 1) * trans.get(0, 0)) + (rect.get(1, 1) * trans.get(0, 1)) + trans.get(0, 3));

		m_ClippingRect.set(1, 0, (rect.get(0, 0) * trans.get(1, 0)) + (rect.get(1, 0) * trans.get(1, 1)) + trans.get(1, 3));
		m_ClippingRect.set(1, 1, (rect.get(0, 1) * trans.get(1, 0)) + (rect.get(1, 1) * trans.get(1, 1)) + trans.get(1, 3));
	}

	static void clear_clipping_rectangle()
	{
		m_ClippingRect.set(0, 0, -1.f);
		m_ClippingRect.set(0, 1, 1.f);

		m_ClippingRect.set(1, 0, -1.f);
		m_ClippingRect.set(1, 1, 1.f);
	}

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
mat2x2f Buffer::m_ClippingRect(0.f, 0.f, INFINITY, INFINITY);


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

		void activate(const Palette* palette)
		{
			// Activate the shader program
			Shader::activate();

			// Set the current transformation
			glUniformMatrix4fv(m_MVP, 1, GL_FALSE, mat_get_values());

			// Set the color tint matrix
			glUniformMatrix4fv(m_TintMatrix, 1, GL_FALSE, palette->get_red_palette_matrix().matrix_values());
		}
	};


	// A buffer for the sprite data.
	class SpriteBuffer : public Buffer
	{
	private:
		// The OpenGL ID for the texture.
		GLuint m_Image;

	public:
		/// <summary>Constructs a sprite buffer.</summary>
		/// <param name="buffer">The OpenGL buffer ID.</param>
		/// <param name="textureID">The OpenGL texture ID.</param>
		SpriteBuffer(GLuint buffer, GLuint texture) : Buffer(buffer)
		{
			m_Image = texture;
		}

		/// <summary>Frees the buffer and texture.</summary>
		~SpriteBuffer()
		{
			glDeleteBuffers(1, &m_Buffer);
			glDeleteTextures(1, &m_Image);
		}

		/// <summary>Activates the buffer.</summary>
		void activate()
		{
			if (!is_buffer_active())
			{
				// Change the texture being drawn from.
				glBindTexture(GL_TEXTURE_2D, m_Image);

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
	SpriteBuffer* m_Buffer = nullptr;


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
		vector<float> sprite_data;

		// Load the file
		LoadFile meta(fpath.c_str());

		while (meta.good())
		{
			// Load the sprite data from file
			unordered_map<string, int> file_data;
			string id = meta.load_data(file_data);

			if (!file_data.empty()) // Check to make sure that the line wasn't empty
			{
				// Load the integer values
				// TODO make this operation safer
				int left = file_data["left"];
				int top = file_data["top"];
				int sprite_width = file_data["width"];
				int sprite_height = file_data["height"];

				// Calculate texcoord numbers
				float l = (float)left / width; // left texcoord
				float r = (float)(left + sprite_width) / width; // right texcoord
				float w = (float)sprite_width;

				float t = (float)top / height; // top texcoord
				float b = (float)(top + sprite_height) / height; // bottom texcoord
				float h = (float)sprite_height;

				// Set the sprite data
				Sprite::set_sprite(id, new Sprite(sprite_data.size() / 4, sprite_width, sprite_height));

				// First triangle: bottom-left, bottom-right, top-right
				// Bottom-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(0.0f);
				// Bottom-left corner, tex coord
				sprite_data.push_back(l);
				sprite_data.push_back(b);
				// Bottom-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(0.0f);
				// Bottom-right corner, tex coord
				sprite_data.push_back(r);
				sprite_data.push_back(b);
				// Top-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(h);
				// Top-right corner, tex coord
				sprite_data.push_back(r);
				sprite_data.push_back(t);

				// Second triangle: bottom-left, top-left, top-right
				// Bottom-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(0.0f);
				// Bottom-left corner, tex coord
				sprite_data.push_back(l);
				sprite_data.push_back(b);
				// Top-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(h);
				// Top-left corner, tex coord
				sprite_data.push_back(l);
				sprite_data.push_back(t);
				// Top-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(h);
				// Top-right corner, tex coord
				sprite_data.push_back(r);
				sprite_data.push_back(t);
			}
		}

		// Bind the sprite data to a buffer
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sprite_data.size(), sprite_data.data(), GL_STATIC_DRAW);

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
		int num_cols = height / partition_height;
		int num_rows = width / partition_width;

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
	/// <param name="palette">The color palette of the sprite.</param>
	void display(SPRITE_KEY sprite, const Palette* palette) const
	{
		// Activate the shader program
		m_Shader->activate(palette);

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



// The raw text of the vertex shader for texture-map sprites.
const char* texmapSpriteVertexShaderText =
"#version 330 core\n"
"layout(location = 0) in vec2 vertexPosition;\n"
"layout(location = 1) in vec2 vertexShadingUV;\n"
"layout(location = 2) in vec2 vertexMappingUV;\n"
"uniform mat4 MVP;\n"
"out vec2 fragmentShadingUV;\n"
"out vec2 fragmentMappingUV;\n"
"void main() {\n"
"	gl_Position = MVP * vec4(vertexPosition, 0, 1);\n"
"	fragmentShadingUV = vertexShadingUV;\n"
"	fragmentMappingUV = vertexMappingUV;\n"
"}";

// The raw text of the fragment shader for texture-map sprites.
const char* texmapSpriteFragmentShaderText =
"#version 330 core\n"
"in vec2 fragmentShadingUV;\n"
"in vec2 fragmentMappingUV;\n"
"uniform mat4x2 mappingMatrix;\n"
"uniform mat4 redPaletteMatrix;\n"
"uniform mat4 greenPaletteMatrix;\n"
"uniform mat4 bluePaletteMatrix;\n"
"uniform sampler2D tex2D;\n"
"void main() {\n"
"   vec4 fragShading = texture(tex2D, fragmentShadingUV);\n"
"   if (fragShading.a < 0.1) discard;\n"
"   vec2 fragMapping = vec2(mappingMatrix * texture(tex2D, fragmentMappingUV));\n"
"   vec4 fragPalette = texture(tex2D, fragMapping);\n"
"   if (fragPalette.a < 0.1) discard;\n"
"   mat4 fragPaletteMatrix = (fragPalette.r * redPaletteMatrix) + (fragPalette.g * greenPaletteMatrix) + (fragPalette.b * bluePaletteMatrix);\n"
"   fragPaletteMatrix[3][3] *= fragPalette.a;\n"
"   vec4 fragColor = fragPaletteMatrix * fragShading;\n"
"	gl_FragColor = fragColor;\n"
"}";


// A sprite sheet that maps 
class TextureMapSSpriteSheet : public TextureMapSpriteSheet
{
protected:
	// A shader that displays a sprite texture.
	class TextureMapSpriteShader : public Shader
	{
	private:
		// The OpenGL ID for the MVP in the sprite shader program.
		GLuint m_MVP;

		// The OpenGL ID for the matrix that maps an RGBA color into texture coordinates.
		GLuint m_ImageMapping;

		// The OpenGL ID for the color palette matrix that the texture-mapping maps RED to in the sprite shader program.
		GLuint m_RedMatrix;

		// The OpenGL ID for the color palette matrix that the texture-mapping maps GREEN to in the sprite shader program.
		GLuint m_GreenMatrix;

		// The OpenGL ID for the color palette matrix that the texture-mapping maps BLUE to in the sprite shader program.
		GLuint m_BlueMatrix;

	public:
		TextureMapSpriteShader() : Shader(texmapSpriteVertexShaderText, texmapSpriteFragmentShaderText)
		{
			// Determine the location of the uniforms
			m_MVP = glGetUniformLocation(m_Shader, "MVP");
			m_ImageMapping = glGetUniformLocation(m_Shader, "mappingMatrix");
			m_RedMatrix = glGetUniformLocation(m_Shader, "redPaletteMatrix");
			m_GreenMatrix = glGetUniformLocation(m_Shader, "greenPaletteMatrix");
			m_BlueMatrix = glGetUniformLocation(m_Shader, "bluePaletteMatrix");
		}

		void activate(const mat4x2f& texture, const Palette* palette)
		{
			// Activate the shader program
			Shader::activate();

			// Set the current transformation
			glUniformMatrix4fv(m_MVP, 1, GL_FALSE, mat_get_values());

			// Set the texture mapping transformation
			glUniformMatrix4x2fv(m_ImageMapping, 1, GL_FALSE, texture.matrix_values());

			// Set the color palette matrices
			glUniformMatrix4fv(m_RedMatrix, 1, GL_FALSE, palette->get_red_palette_matrix().matrix_values());
			glUniformMatrix4fv(m_GreenMatrix, 1, GL_FALSE, palette->get_green_palette_matrix().matrix_values());
			glUniformMatrix4fv(m_BlueMatrix, 1, GL_FALSE, palette->get_blue_palette_matrix().matrix_values());
		}
	};

	// A buffer for the sprite data.
	class TextureMapSpriteBuffer : public Buffer
	{
	private:
		// The OpenGL ID for the texture.
		GLuint m_Image;

	public:
		/// <summary>Constructs a sprite buffer.</summary>
		/// <param name="buffer">The OpenGL buffer ID.</param>
		/// <param name="textureID">The OpenGL texture ID.</param>
		TextureMapSpriteBuffer(GLuint buffer, GLuint texture) : Buffer(buffer)
		{
			m_Image = texture;
		}

		/// <summary>Frees the buffer and texture.</summary>
		~TextureMapSpriteBuffer()
		{
			glDeleteBuffers(1, &m_Buffer);
			glDeleteTextures(1, &m_Image);
		}

		/// <summary>Activates the buffer.</summary>
		void activate()
		{
			if (!is_buffer_active())
			{
				// Change the texture being drawn from.
				glBindTexture(GL_TEXTURE_2D, m_Image);

				// Change the buffer being used.
				glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
				m_ActiveBuffer = m_Buffer;

				// Set attrib array for position
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);

				// Set attrib array for shading tex coord
				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 2));

				// Set attrib array for mapping tex coord
				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 4));
			}
		}
	};


	static TextureMapSpriteShader* m_Shader;

	TextureMapSpriteBuffer* m_Buffer;


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
			m_Shader = new TextureMapSpriteShader();
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
		std::vector<float> sprite_data;

		// Load the file
		LoadFile meta(fpath.c_str());
		regex texture_finder("texture\\s+(\\S.+)");

		while (meta.good())
		{
			unordered_map<string, int> file_data;
			string id = meta.load_data(file_data);

			smatch idmatch;
			if (regex_match(id, idmatch, texture_finder))
			{
				id = idmatch[1].str();

				int texture_width = file_data["width"];
				int texture_height = file_data["height"];
				int left = file_data["left"];
				int top = file_data["top"];

				float half_width = (0.5f * texture_width) / width;

				Texture* texture = new Texture(mat4x2f(
					-half_width, half_width, 0.f, ((float)left / width) + half_width,
					0.f, 0.f, (float)texture_height / height, (float)top / height
				));
				Texture::set_texture(id, texture);
			}
			else
			{
				// Load the data values
				int sprite_width = file_data["width"];
				int sprite_height = file_data["height"];
				int shading_left = file_data["shading_left"];
				int shading_top = file_data["shading_top"];
				int mapping_left = file_data["mapping_left"];
				int mapping_top = file_data["mapping_top"];

				// Calculate texcoords
				float w = (float)sprite_width;
				float h = (float)sprite_height;

				float sl = (float)shading_left / width; // left texcoord for shading
				float sr = (float)(shading_left + sprite_width) / width; // right texcoord for shading
				float st = (float)shading_top / height; // top texcoord for shading
				float sb = (float)(shading_top + sprite_height) / height; // bottom texcoord for shading

				float ml = (float)mapping_left / width; // left texcoord for shading
				float mr = (float)(mapping_left + sprite_width) / width; // right texcoord for shading
				float mt = (float)mapping_top / height; // top texcoord for shading
				float mb = (float)(mapping_top + sprite_height) / height; // bottom texcoord for shading

				// Set the sprite data
				Sprite::set_sprite(id, new Sprite(sprite_data.size() / 6, sprite_width, sprite_height));

				// First triangle: bottom-left, bottom-right, top-right
				// Bottom-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(0.0f);
				// Bottom-left corner, shading tex coord
				sprite_data.push_back(sl);
				sprite_data.push_back(sb);
				// Bottom-left corner, mapping tex coord
				sprite_data.push_back(ml);
				sprite_data.push_back(mb);
				// Bottom-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(0.0f);
				// Bottom-right corner, shading tex coord
				sprite_data.push_back(sr);
				sprite_data.push_back(sb);
				// Bottom-right corner, mapping tex coord
				sprite_data.push_back(mr);
				sprite_data.push_back(mb);
				// Top-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(h);
				// Top-right corner, shading tex coord
				sprite_data.push_back(sr);
				sprite_data.push_back(st);
				// Top-right corner, mapping tex coord
				sprite_data.push_back(mr);
				sprite_data.push_back(mt);

				// Second triangle: bottom-left, top-left, top-right
				// Bottom-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(0.0f);
				// Bottom-left corner, shading tex coord
				sprite_data.push_back(sl);
				sprite_data.push_back(sb);
				// Bottom-left corner, mapping tex coord
				sprite_data.push_back(ml);
				sprite_data.push_back(mb);
				// Top-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(h);
				// Top-left corner, shading tex coord
				sprite_data.push_back(sl);
				sprite_data.push_back(st);
				// Top-left corner, shading tex coord
				sprite_data.push_back(ml);
				sprite_data.push_back(mt);
				// Top-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(h);
				// Top-right corner, shading tex coord
				sprite_data.push_back(sr);
				sprite_data.push_back(st);
				// Top-right corner, mapping tex coord
				sprite_data.push_back(mr);
				sprite_data.push_back(mt);
			}
		}

		// Bind the sprite data to a buffer
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sprite_data.size(), sprite_data.data(), GL_STATIC_DRAW);

		// If the buffer already exists, free it
		if (m_Buffer)
		{
			delete m_Buffer;
		}

		// Create the buffer object
		m_Buffer = new TextureMapSpriteBuffer(buf, tex);
	}

	/// <summary>Draws a sprite on the sprite sheet.</summary>
	/// <param name="sprite">The key of the sprite.</param>
	/// <param name="palette">The color palette of the sprite.</param>
	void display(SPRITE_KEY sprite, const mat4x2f& texture, const Palette* palette) const
	{
		// Activate the shader program
		m_Shader->activate(texture, palette);

		// Activate the buffer
		m_Buffer->activate();

		// Draw the sprite using information from buffer
		glDrawArrays(GL_TRIANGLES, sprite, 6);
	}
};

TextureMapSSpriteSheet::TextureMapSpriteShader* TextureMapSSpriteSheet::m_Shader{ nullptr };


TextureMapSpriteSheet::TextureMapSpriteSheet() {}

TextureMapSpriteSheet* TextureMapSpriteSheet::generate(const char* path)
{
	TextureMapSpriteSheet* sprite_sheet = new TextureMapSSpriteSheet();
	sprite_sheet->load_sprite_sheet(path);
	return sprite_sheet;
}

TextureMapSpriteSheet* TextureMapSpriteSheet::generate_empty()
{
	return new TextureMapSSpriteSheet();
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


SpriteGraphic::SpriteGraphic(SpriteSheet* sprite_sheet, const Palette* palette)
{
	m_SpriteSheet = sprite_sheet;
	m_Palette = palette;
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
	m_SpriteSheet->display(get_sprite()->key, m_Palette);
}


StaticSpriteGraphic::StaticSpriteGraphic(SpriteSheet* sprite_sheet, Sprite* sprite, const Palette* palette) : SpriteGraphic(sprite_sheet, palette)
{
	m_Sprite = sprite;
}

Sprite* StaticSpriteGraphic::get_sprite() const
{
	return m_Sprite;
}


DynamicSpriteGraphic::DynamicSpriteGraphic(SpriteSheet* spriteSheet, const Palette* palette) : SpriteGraphic(spriteSheet, palette) {}

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




unordered_map<TEXTURE_ID, Texture*> Texture::m_Images{};

Texture* Texture::get_texture(TEXTURE_ID id)
{
	auto iter = m_Images.find(id);
	return iter != m_Images.end() ? iter->second : nullptr;
}

void Texture::set_texture(TEXTURE_ID id, Texture* texture)
{
	auto iter = m_Images.find(id);
	if (iter != m_Images.end())
	{
		m_Images.erase(iter);
		m_Images.emplace_hint(iter, id, texture);
	}
	else
	{
		m_Images.emplace(id, texture);
	}
}

Texture::Texture(const mat4x2f& trans) : transform(trans) {}



FontSprite::FontSprite(SPRITE_KEY key, int width, int height, int flush_width) : Sprite(key, width, height), flush_width(flush_width) {}


int Font::get_line_height() const
{
	return m_LineHeight;
}


class SpriteFont : public Font, protected SSpriteSheet
{
protected:
	int get_character_dx(char prev, char current) const
	{
		if (prev == ' ') return m_Spacing + m_Kerning;

		auto iter = m_Characters.find(prev);
		if (iter == m_Characters.end()) return 0;

		if (current == 'a' || current == 'c' || current == 'e' ||
			current == 'g' || current == 'i' || current == 'j' ||
			current == 'm' || current == 'n' || current == 'o' ||
			current == 'p' || current == 'q' || current == 'r' ||
			current == 's' || current == 'u' || current == 'v' || 
			current == 'w' || current == 'x' || current == 'y' || 
			current == 'z' || current == ',' || current == '.' ||
			current == '+' || current == '=' || current == '-' ||
			current == '_') 
		{
			return iter->second.flush_width + m_Kerning;
		}

		return iter->second.width + m_Kerning;
	}

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
		std::vector<float> sprite_data;

		// Load the file
		LoadFile meta(fpath.c_str());
		regex height_finder("height\\s*=\\s*(\\d+)");
		regex kerning_finder("kerning\\s*=\\s*(\\-?\\d+)");
		regex spacing_finder("spacing\\s*=\\s*(\\-?\\d+)");

		while (meta.good())
		{
			unordered_map<string, int> file_data;
			string id = meta.load_data(file_data);

			smatch idmatch;
			if (regex_match(id, idmatch, height_finder))
			{
				m_LineHeight = stoi(idmatch[1].str());
			}
			else if (regex_match(id, idmatch, kerning_finder))
			{
				m_Kerning = stoi(idmatch[1].str());
			}
			else if (regex_match(id, idmatch, spacing_finder))
			{
				m_Spacing = stoi(idmatch[1].str());
			}
			else if (id.size() > 0)
			{
				char character = id[0];

				int sprite_width = file_data["width"];
				int left = file_data["left"];
				int top = file_data["top"];

				int flush = sprite_width;
				auto flush_iter = file_data.find("flush");
				if (flush_iter != file_data.end())
					flush = flush_iter->second;

				// Calculate texcoord numbers
				float l = (float)left / width; // left texcoord
				float r = (float)(left + sprite_width) / width; // right texcoord
				float w = (float)sprite_width;

				float t = (float)top / height; // top texcoord
				float b = (float)(top + m_LineHeight) / height; // bottom texcoord
				float h = (float)m_LineHeight;

				// Set the sprite data
				m_Characters.emplace(character, FontSprite(sprite_data.size() / 4, sprite_width, m_LineHeight, flush));

				// First triangle: bottom-left, bottom-right, top-right
				// Bottom-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(0.0f);
				// Bottom-left corner, tex coord
				sprite_data.push_back(l);
				sprite_data.push_back(b);
				// Bottom-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(0.0f);
				// Bottom-right corner, tex coord
				sprite_data.push_back(r);
				sprite_data.push_back(b);
				// Top-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(h);
				// Top-right corner, tex coord
				sprite_data.push_back(r);
				sprite_data.push_back(t);

				// Second triangle: bottom-left, top-left, top-right
				// Bottom-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(0.0f);
				// Bottom-left corner, tex coord
				sprite_data.push_back(l);
				sprite_data.push_back(b);
				// Top-left corner, vertices
				sprite_data.push_back(0.0f);
				sprite_data.push_back(h);
				// Top-left corner, tex coord
				sprite_data.push_back(l);
				sprite_data.push_back(t);
				// Top-right corner, vertices
				sprite_data.push_back(w);
				sprite_data.push_back(h);
				// Top-right corner, tex coord
				sprite_data.push_back(r);
				sprite_data.push_back(t);
			}
		}

		// Bind the sprite data to a buffer
		GLuint buf;
		glGenBuffers(1, &buf);
		glBindBuffer(GL_ARRAY_BUFFER, buf);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * sprite_data.size(), sprite_data.data(), GL_STATIC_DRAW);

		// If the buffer already exists, free it
		if (m_Buffer)
		{
			delete m_Buffer;
		}

		// Create the buffer object
		m_Buffer = new SpriteBuffer(buf, tex);
	}

	int get_line_width(string line) const
	{
		if (line.size() == 0)
			return 0;

		int width = 0;
		char prev = line.at(0);

		for (int k = 1; k < line.size(); ++k)
		{
			char current = line.at(k);
			width += get_character_dx(prev, current);
			prev = current;
		}

		return width + get_character_dx(prev, '\0');
	}

	void display_line(string line, const Palette* palette)
	{
		if (line.size() == 0)
			return;

		mat_push();
		char prev = line.at(0);

		for (int k = 1; k <= line.size(); ++k)
		{
			auto iter = m_Characters.find(prev);

			if (iter != m_Characters.end())
			{
				FontSprite& sprite = iter->second;
				display(sprite.key, palette);
			}

			char current = (k == line.size() ? '\0' : line.at(k));
			int dx = get_character_dx(prev, current);
			prev = current;
			mat_translate(dx, 0.f, 0.f);
		}

		mat_pop();
	}
};

Font* Font::load_sprite_font(const char* path)
{
	SpriteFont* font = new SpriteFont();
	font->load_sprite_sheet(path);
	return font;
}



TextGraphic::TextGraphic(Font* font, const Palette* palette, string text, int width, TextAlignment alignment)
{
	m_Font = font;
	m_Palette = palette;
	m_Width = width;

	m_Alignment = alignment;

	set_text(text);
}

int TextGraphic::get_width() const
{
	return m_Width;
}

void TextGraphic::set_width(int width)
{
	m_Width = width;
	set_text(m_Text);
}

int TextGraphic::get_height() const
{
	return m_Lines.size() * m_Font->get_line_height();
}

string TextGraphic::get_text() const
{
	string text;

	for (auto iter = m_Lines.begin(); iter != m_Lines.end(); ++iter)
	{
		text += iter->text + " ";
	}

	return text;
}

void TextGraphic::set_text(string text)
{
	m_Text = text;
	m_Lines.clear();

	string remainder = text;
	string line;
	int line_width = 0;

	while (remainder.size() > 0)
	{
		size_t next_space = remainder.find_first_of(' ');
		string word = remainder.substr(0, next_space);
		remainder = next_space == string::npos ? "" : remainder.substr(next_space + 1);

		string word_with_leading_space = " " + word;
		int word_width = m_Font->get_line_width(word_with_leading_space);
		if (line_width + word_width > m_Width)
		{
			int xpos;
			switch (m_Alignment)
			{
			case TEXT_LEFT:
				xpos = 0;
				break;
			case TEXT_RIGHT:
				xpos = m_Width - line_width;
				break;
			case TEXT_CENTER:
				xpos = (m_Width - line_width) / 2;
				break;
			}

			m_Lines.push_back({ line, xpos });
			line = word;
			line_width = m_Font->get_line_width(word);
		}
		else
		{
			if (line.empty())
			{
				line = word;
				line_width = m_Font->get_line_width(word);
			}
			else
			{
				line += word_with_leading_space;
				line_width += word_width;
			}
		}
	}
	
	if (!line.empty())
	{
		int xpos;
		switch (m_Alignment)
		{
		case TEXT_LEFT:
			xpos = 0;
			break;
		case TEXT_RIGHT:
			xpos = m_Width - line_width;
			break;
		case TEXT_CENTER:
			xpos = (m_Width - line_width) / 2;
			break;
		}

		m_Lines.push_back({ line, xpos });
	}
}

void TextGraphic::display() const
{
	if (!m_Lines.empty())
	{
		mat_push();
		mat_translate(0.f, -get_height(), 0.f);

		auto iter = m_Lines.rbegin();
		while (true)
		{
			mat_push();
			mat_translate(iter->xpos, 0.f, 0.f);
			m_Font->display_line(iter->text, m_Palette);
			mat_pop();

			if (++iter == m_Lines.rend())
			{
				break;
			}
			else
			{
				mat_translate(0.f, m_Font->get_line_height(), 0.f);
			}
		}

		mat_pop();
	}
}








/*
*
*
*	FRAMES
*
*
*/

/*
Frame::Frame()
{
	m_Parent = nullptr;
}

Frame::Frame(int x, int y, int width, int height)
{
	m_Parent = nullptr;
	set_bounds(x, y, width, height);
}

const mat2x2i& Frame::get_bounds() const
{
	return m_Bounds;
}

mat2x2i Frame::get_absolute_bounds() const
{
	if (m_Parent)
	{
		mat2x2i pbounds = m_Parent->get_absolute_bounds();
		int px = pbounds.get(0, 0);
		int py = pbounds.get(1, 0);

		return mat2x2i(m_Bounds.get(0, 0) + px, m_Bounds.get(0, 1) + px,
			m_Bounds.get(1, 0) + py, m_Bounds.get(1, 1) + py);
	}
	else
	{
		return m_Bounds;
	}
}

void Frame::__set_bounds() {}

void Frame::set_bounds(int x, int y, int width, int height)
{
	m_Bounds.set(0, 0, x);
	m_Bounds.set(1, 0, y);

	m_Bounds.set(0, 1, x + width);
	m_Bounds.set(1, 1, y + height);

	__set_bounds();
}

int Frame::get_width() const
{
	return m_Bounds.get(0, 1) - m_Bounds.get(0, 0);
}

int Frame::get_height() const
{
	return m_Bounds.get(1, 1) - m_Bounds.get(1, 0);
}

void Frame::set_parent(Frame* parent)
{
	m_Parent = parent;
}

void Frame::__display() const {}

void Frame::display() const
{
	// Set the clipping rectangle to the bounds of the frame
	Buffer::set_clipping_rectangle(m_Bounds);

	// Set up the transformation
	mat_push();
	mat_translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

	// Display the contents of the frame
	__display();

	// Deconstruct the transformation
	mat_pop();

	// Set the clipping rectangle back to bounds of the parent
	if (m_Parent)
	{
		Buffer::set_clipping_rectangle(m_Parent->get_bounds());
	}
	else
	{
		Buffer::set_clipping_rectangle(mat2x2f(0.f, 0.f, INFINITY, INFINITY));
	}
}



void Button::highlight() {}
void Button::unhighlight() {}
void Button::click() {}

int Button::trigger(const MouseMoveEvent& event_data)
{
	mat2x2i absbounds = get_absolute_bounds();

	if (event_data.x >= absbounds.get(0, 0) && event_data.x < absbounds.get(0, 1)
		&& event_data.y >= absbounds.get(1, 0) && event_data.y < absbounds.get(1, 1))
	{
		if (!m_Highlighted)
		{
			m_Highlighted = true;
			highlight();
		}

		return EVENT_STOP;
	}
	else if (m_Highlighted)
	{
		m_Highlighted = false;
		unhighlight();
	}

	return EVENT_CONTINUE;
}

int Button::trigger(const MousePressEvent& event_data)
{
	if (m_Highlighted)
	{
		click();
		return EVENT_STOP;
	}

	return EVENT_CONTINUE;
}

void Button::freeze()
{
	MouseMoveListener::freeze();
	MousePressListener::freeze();
}

void Button::unfreeze()
{
	MouseMoveListener::unfreeze();
	MousePressListener::unfreeze();
}



TextInput::TextInput()
{
	m_CursorGraphic = nullptr;
}

TextInput::~TextInput()
{
	if (m_CursorGraphic)
	{
		delete m_CursorGraphic;
	}
}

string TextInput::get_input() const
{
	return m_TextInput;
}

int TextInput::trigger(const MousePressEvent& event_data)
{
	mat2x2i absbounds = get_absolute_bounds();

	if (event_data.x >= absbounds.get(0, 0) && event_data.x < absbounds.get(0, 1)
		&& event_data.y >= absbounds.get(1, 0) && event_data.y < absbounds.get(1, 1))
	{
		// Start responding to keyboard inputs
		if (m_TextFrozen)
		{
			m_TextFrozen = false;
			KeyboardListener::unfreeze();
		}

		// Construct cursor object
		if (!m_CursorGraphic)
		{
			m_CursorGraphic = SolidColorGraphic::generate(0, 0, 0, 255, 1, get_font()->get_line_height() + 1);
		}

		// Position cursor
		// TODO

		return EVENT_STOP;
	}
	else if (!m_TextFrozen)
	{
		m_TextFrozen = true;
		KeyboardListener::freeze();
	}

	return EVENT_CONTINUE;
}

int TextInput::trigger(const UnicodeEvent& event_data)
{
	if (event_data.character == 0x08 && m_Cursor > 0) // Backspace
	{
		m_TextInput.erase(--m_Cursor, 1);
	}
	else if (event_data.character == 0x7f && m_Cursor < m_TextInput.size() - 1) // Delete
	{
		m_TextInput.erase(m_Cursor, 1);
	}
	else
	{
		m_TextInput.insert(m_Cursor++, 1, event_data.character);
	}

	return EVENT_STOP;
}

void TextInput::freeze()
{
	m_TextFrozen = true;
	MousePressListener::freeze();
	KeyboardListener::freeze();
}

void TextInput::unfreeze()
{
	MousePressListener::unfreeze();
}

void TextInput::display() const
{
	// Set up the transform
	mat_push();
	mat_translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

	// Display the text input
	get_font()->display_line(m_TextInput, get_font_palette());

	// Display the cursor
	if (!m_TextFrozen)
	{
		mat_translate(get_font()->get_line_width(m_TextInput.substr(0, m_Cursor)), 0.f, 0.f);
		m_CursorGraphic->display();
	}

	// Clean up the transform
	mat_pop();
}



ScrollBar::ScrollBar(Graphic* backgroundGraphic, Graphic* arrowGraphic, Graphic* scrollGraphic, int x, int y, bool horizontal)
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

void ScrollBar::set_center_of_scroller(int dx, int dy)
{
	if (m_Horizontal)
	{
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
		set_value(
			min(
				max(dy - (m_Scroller->get_height() / 2), 0)
				/ (float)(m_Background->get_height() - m_Scroller->get_height()),
				1.f
			)
		);
	}
}

float ScrollBar::get_value()
{
	return m_Value;
}

void ScrollBar::set_value(float value)
{
	m_Value = value;
}

int ScrollBar::trigger(const MousePressEvent& event_data)
{
	mat2x2i absbounds = get_absolute_bounds();
	int dx = event_data.x - absbounds.get(0, 0);
	int dy = event_data.y - absbounds.get(1, 0);

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
				set_center_of_scroller(dx, dy);
				dragged = this;
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
				set_center_of_scroller(dx, dy);
				dragged = this;
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

int ScrollBar::trigger(const MouseMoveEvent& event_data)
{
	if (dragged == this)
	{
		mat2x2i absbounds = get_absolute_bounds();
		int dx = event_data.x - absbounds.get(0, 0) - m_Arrow->get_width();
		int dy = event_data.y - absbounds.get(1, 0) - m_Arrow->get_height();

		set_center_of_scroller(dx, dy);
	}

	return EVENT_CONTINUE;
}

void ScrollBar::display() const
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



ScrollableFrame::ScrollableFrame(Frame* frame, ScrollBar* horizontal, ScrollBar* vertical, int x, int y, int width, int height)
{
	m_Frame = frame;
	m_HorizontalScrollBar = horizontal;
	m_VerticalScrollBar = vertical;

	m_Frame->set_parent(this);
	if (m_HorizontalScrollBar) m_HorizontalScrollBar->set_parent(this);
	if (m_VerticalScrollBar) m_VerticalScrollBar->set_parent(this);

	set_bounds(x, y, width, height);
}

void ScrollableFrame::__update(int frames_passed)
{
	int w = get_width() - m_Frame->get_width();
	int h = get_height() - m_Frame->get_height();

	m_ScrollDistance = vec2i(min(w, 0), h);
}

void ScrollableFrame::display() const
{
	// Set up the translation
	mat_push();
	mat_translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

	// Draw the frame itself
	mat_push();
	int dx = m_HorizontalScrollBar ? m_HorizontalScrollBar->get_value() * m_ScrollDistance.get(0) : 0;
	int dy = m_VerticalScrollBar ?
		(m_ScrollDistance.get(1) < 0 ? m_VerticalScrollBar->get_value() * m_ScrollDistance.get(1) : m_ScrollDistance.get(1))
		: 0;
	mat_translate(dx, dy, 0.f);
	m_Frame->display();
	mat_pop();

	// Draw the scroll bars
	if (m_HorizontalScrollBar) m_HorizontalScrollBar->display();
	if (m_VerticalScrollBar) m_VerticalScrollBar->display();

	// Clean up the translation
	mat_pop();
}




LayerFrame::LayerFrame()
{
	if (m_Parent)
		set_bounds(0, 0, m_Parent->get_width(), m_Parent->get_height());
	else
		set_bounds(-1, -1, 2, 2);

	insert_top(SolidColorGraphic::generate(rand() % 256, rand() % 256, rand() % 256, 255, 2, 2));
}

LayerFrame::LayerFrame(int x, int y, int width, int height)
{
	set_bounds(x, y, width, height);

	insert_top(SolidColorGraphic::generate(rand() % 256, rand() % 256, rand() % 256, 255, width, height));
}

void LayerFrame::reset()
{
	static Application*& app = get_application_settings();

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

	if (Frame* frame = dynamic_cast<Frame*>(graphic))
	{
		frame->set_parent(this);
	}

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


UIFrame::UIFrame()
{
	Application* app = get_application_settings();
	set_bounds(0, 0, app->width, app->height);
	reset();
}

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




vec2i WorldOrthographicFrame::Tool::get_tile(int dx, int dy)
{
	return vec2i(dx / TILE_WIDTH, dy / TILE_HEIGHT);
}

Object* WorldOrthographicFrame::Tool::get_object(int dx, int dy)
{
	return nullptr;
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

	int width = get_width();
	int height = get_height();
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
	float sz = 2.f / (TILE_HEIGHT * m_Chunk->height); // The z-scale of the projection

	int mx = 1 + (width % 2);
	int my = 1 + (height % 2);

	int fx = -(m_DisplayArea.get(0, 0) + m_DisplayArea.get(0, 1) + mx) / 2;
	int fy = -(m_DisplayArea.get(1, 0) + m_DisplayArea.get(1, 1) + my) / 2;

	float cx = (sx * ((m_Bounds.get(0, 0) + halfWidth) + fx)) - 1.f;
	float cy = (sy * ((m_Bounds.get(1, 0) + halfHeight) + fy)) - 1.f;

	m_Transform = mat4x4f(
		sx, 0.f, 0.f, cx,
		0.f, sy, -sy, cy,
		0.f, sz, sz, sz * fy
	);

	m_Tool = nullptr;
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
	}
	else if (xright < 0)
	{
		m_DisplayArea(0, 0) += xright;
		m_DisplayArea.set(0, 1, xmax);
	}

	int ymin = TILE_HEIGHT * m_TileMargin;
	int ymax = (m_Chunk->height * TILE_HEIGHT) - (ymin + 1);
	int ybottom = ymin - m_DisplayArea.get(1, 0);
	int ytop = ymax - m_DisplayArea.get(1, 1);
	if (ybottom > 0)
	{
		m_DisplayArea(1, 1) += ybottom;
		m_DisplayArea.set(1, 0, ymin);
	}
	else if (ytop < 0)
	{
		m_DisplayArea(1, 0) += ytop;
		m_DisplayArea.set(1, 1, ymax);
	}
}

void WorldOrthographicFrame::set_camera(float x, float y)
{
	m_Camera.set(0, 0, x);
	m_Camera.set(1, 0, y);
	reset();
}

void WorldOrthographicFrame::set_tool(WorldOrthographicFrame::Tool* tool)
{
	m_Tool = tool;
}

void WorldOrthographicFrame::adjust_camera(float dx, float dy)
{
	// Get camera coordinates
	float& cx = m_Camera(0);
	float& cy = m_Camera(1);

	// Store original (integer) camera coordinates
	int former_x = roundf(cx);
	int former_y = roundf(cy);

	// Adjust camera
	cx += dx;
	cy += dy;

	// Calculate absolute change in x-coordinates of camera
	int dcx = (int)roundf(cx) - former_x;
	if (dcx)
	{
		int width = m_Bounds.get(0, 1) - m_Bounds.get(0, 0);

		int xmin = TILE_WIDTH * m_TileMargin;
		int xmax = (TILE_WIDTH * m_Chunk->width) - xmin - 1;

		// Calculate displayed change in x-coordinates of camera
		// (Which may be different from absolute change due to clamping of display)
		if (dcx > 0)
		{
			int former_xmin = former_x - ((width + 1) / 2);

			if (former_xmin < xmin)
				dcx = max(0, dcx - xmin + former_xmin);
			dcx = min(dcx, xmax - m_DisplayArea.get(0, 1));
		}
		else
		{
			int former_xmax = former_x + (width / 2) - 1;

			if (former_xmax > xmax)
				dcx = min(0, dcx - xmax + former_xmax);
			dcx = max(dcx, xmin - m_DisplayArea.get(0, 0));
		}

		if (dcx)
		{
			m_DisplayArea(0, 0) += dcx;
			m_DisplayArea(0, 1) += dcx;
		}
	}

	// Calculate absolute change in y-coordinates of camera
	int dcy = (int)roundf(cy) - former_y;
	if (dcy)
	{
		int height = m_Bounds.get(1, 1) - m_Bounds.get(1, 0);

		int ymin = TILE_HEIGHT * m_TileMargin;
		int ymax = (TILE_HEIGHT * m_Chunk->width) - ymin - 1;

		// Calculate displayed change in y-coordinates of camera
		if (dcy > 0)
		{
			int former_ymin = former_y - ((height + 1) / 2);

			if (former_ymin < ymin)
				dcy = max(0, dcy - ymin + former_ymin);
			dcy = min(dcy, ymax - m_DisplayArea.get(1, 1));
		}
		else
		{
			int former_ymax = former_y + (height / 2) - 1;

			if (former_ymax > ymax)
				dcy = min(0, dcy - ymax + former_ymax);
			dcy = max(dcy, ymin - m_DisplayArea.get(1, 0));
		}

		if (dcy)
		{
			m_DisplayArea(1, 0) += dcy;
			m_DisplayArea(1, 1) += dcy;
		}
	}

	// If there was a change in the integer camera position, clamp the display and adjust the transformation
	if (dcx || dcy)
	{
		// Calculate the change in the translation
		float dtx = -m_Transform.get(0, 0) * dcx;
		float dty = -m_Transform.get(1, 1) * dcy;
		float dtz = -m_Transform.get(2, 2) * dcy;

		// Adjust the transformation
		m_Transform(0, 3) += dtx;
		m_Transform(1, 3) += dty;
		m_Transform(2, 3) += dtz;
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

int WorldOrthographicFrame::trigger(const MousePressEvent& event_data)
{
	if (m_Tool)
	{
		mat2x2i absbounds = get_absolute_bounds();
		if (event_data.x >= absbounds.get(0, 0) && event_data.x < absbounds.get(0, 1)
			&& event_data.y >= absbounds.get(1, 0) && event_data.y < absbounds.get(1, 1))
		{
			m_Tool->select();
			dragged = this;
			return EVENT_STOP;
		}
	}

	return EVENT_CONTINUE;
}

int WorldOrthographicFrame::trigger(const MouseMoveEvent& event_data)
{
	if (m_Tool)
	{
		mat2x2i absbounds = get_absolute_bounds();
		int dx = event_data.x - absbounds.get(0, 0);
		int dy = event_data.y - absbounds.get(1, 0);
		if (dx >= 0 && event_data.x < absbounds.get(0, 1)
			&& dy >= 0 && dy < absbounds.get(1, 1))
		{
			// Call the tool highlight function
			m_Tool->highlight(dx + m_DisplayArea.get(0, 0), dy + m_DisplayArea.get(1, 0));

			// If mouse button is held down, call the tool select function
			if (dragged == this) m_Tool->select();

			return EVENT_STOP;
		}
	}

	return EVENT_CONTINUE;
}

*/