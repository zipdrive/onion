#include "../../../include/onions/application.h"
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
		if (!m_Buffer)
		{
			m_Buffer = new opengl::_UniformBuffer(
				"Camera",
				{ 
					new opengl::_UniformBlockAttribute<FLOAT_MAT4>("projection", 0), 
					new opengl::_UniformBlockAttribute<FLOAT_MAT4>("view", type_size<FLOAT_MAT4>::whole)
				},
				2 * type_size<FLOAT_MAT4>::whole
			);
		}
	}

	void Transform::set_projection()
	{
		m_Buffer->set<FLOAT_MAT4>(0, projection.get());
	}

	void Transform::set_view()
	{
		m_Buffer->set<FLOAT_MAT4>(1, view.get());
	}


	Camera* Camera::m_ActiveCamera{ nullptr };
	
	bool Camera::is_active() const
	{
		return this == m_ActiveCamera;
	}

	void Camera::activate()
	{
		if (!is_active())
		{
			m_ActiveCamera = this;

			Transform::projection.identity();
			Transform::view.identity();
			__activate();
			Transform::set_projection();
			Transform::set_view();
		}
	}


	void OrthogonalCamera::__activate() const
	{
		Application* app = get_application_settings();
		Transform::projection.ortho(0.f, app->width, 0.f, app->height, -app->width, app->width);
	}

}