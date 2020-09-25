#pragma once
#include "shader.h"

namespace onion
{

	// 
	class Transform
	{
	private:
		// The uniform buffer for the projection and view matrices.
		static opengl::_UniformBuffer* m_Buffer;

	public:
		// The projection transformation matrix.
		static MatrixStack projection;

		// The view transformation matrix.
		static MatrixStack view;

		// The model transformation matrix.
		static MatrixStack model;


		/// <summary>Initializes all the transform objects.</summary>
		static void init();
		
		/// <summary>Writes the current projection matrix to the buffer.</summary>
		static void set_projection();

		/// <summary>Writes the current view matrix to the buffer.</summary>
		static void set_view();
	};

}