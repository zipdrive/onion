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
				"solid_color",
				{ "model", "color" }
			);
		}

		// Construct the data buffer
		VertexBufferData<FLOAT_VEC2> data;
		data.push(6);

		data.set<0>(0, vec2f(0.f, 0.f));
		data.set<0>(1, vec2f(1.f, 0.f));
		data.set<0>(2, vec2f(1.f, 1.f));

		data.set<0>(3, vec2f(0.f, 0.f));
		data.set<0>(4, vec2f(0.f, 1.f));
		data.set<0>(5, vec2f(1.f, 1.f));

		// Construct the buffer
		m_Displayer = new opengl::_SquareBufferDisplayer();
		m_Displayer->set_buffer(
			new opengl::_VertexBuffer(

				// The position of each vertex
				&data,
				
				// All vertex attributes
				m_Shader->get_attribs()
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
		// Stretch the graphic to match the width and height
		Transform::model.push();
		Transform::model.scale(width, height);

		// Activate the shader
		m_Shader->activate(color);

		// Display the rectangle
		m_Displayer->display(0);

		// Clean up the transformation
		Transform::model.pop();
	}


}