#include "../../../include/onions/graphics/graphic.h"

namespace onion
{

	
	SolidColorGraphic::SolidColorShader* SolidColorGraphic::m_Shader{ nullptr };
	
	SolidColorGraphic::SolidColorGraphic(int r, int g, int b, int a, int width, int height) 
		: SolidColorGraphic(r * RGBA_INT_TO_FLOAT, g * RGBA_INT_TO_FLOAT, b * RGBA_INT_TO_FLOAT, a * RGBA_INT_TO_FLOAT, width, height) {}

	SolidColorGraphic::SolidColorGraphic(float r, float g, float b, float a, int width, int height) : color(r, g, b, a), width(width), height(height)
	{
		if (!m_Shader)
		{
			m_Shader = new SolidColorGraphic::SolidColorShader(

				// The raw vertex shader text
				"#version 330 core\n"
				"layout(location = 0) in vec2 vertexPosition;\n"
				"uniform mat4 MVP;\n"
				"void main() {\n"
				"	gl_Position = MVP * vec4(vertexPosition, 0, 1);\n"
				"}",

				// The raw fragment shader text
				"#version 330 core\n"
				"uniform vec4 color;\n"
				"void main() {\n"
				"	gl_FragColor = color;\n"
				"}",

				// The uniform variables for the shader program
				{ "MVP", "color" }

			);
		}

		// Construct the buffer
		m_Displayer = new opengl::_SquareBufferDisplayer();
		m_Displayer->set_buffer(
			new SolidColorGraphic::SolidColorBuffer(
				
				// The shader
				m_Shader,

				// The vertex attributes
				{ "vertexPosition" },

				// The position of each vertex
				{ 
					// The first triangle of vertices
					0.f, 0.f,
					1.f, 0.f,
					1.f, 1.f,

					// The second triangle of vertices
					0.f, 0.f,
					0.f, 1.f,
					1.f, 1.f 
				}

			)
		);
	}

	SolidColorGraphic::~SolidColorGraphic()
	{
		delete m_Displayer;
	}

	int SolidColorGraphic::get_width() const
	{
		return width;
	}

	int SolidColorGraphic::get_height() const
	{
		return height;
	}

	void SolidColorGraphic::display() const
	{
		// Activate the shader
		m_Shader->activate(&color);

		// Display the rectangle
		MatrixStack& m = model();
		m.push();
		m.scale(width, height);
		m_Displayer->display(0);
		m.pop();
	}


}