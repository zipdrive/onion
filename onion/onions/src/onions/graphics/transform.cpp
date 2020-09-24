#include "../../../include/onions/graphics/transform.h"

namespace onion
{

	opengl::_UniformBuffer* Transform::m_Buffer{ nullptr };

	MatrixStack Transform::projection;
	MatrixStack Transform::view;
	MatrixStack Transform::model;

	void Transform::init()
	{
		projection.reset();
		view.reset();
		model.reset();

		m_Buffer = opengl::_UniformBuffer::get_buffer("Camera");
	}

	void Transform::set_projection()
	{
		m_Buffer->set(0, projection);
	}

	void Transform::set_view()
	{
		m_Buffer->set(1, view);
	}
}