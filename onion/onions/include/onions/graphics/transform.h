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


	// A camera that controls the projection and view matrices.
	class Camera
	{
	private:
		// The currently active camera.
		static Camera* m_ActiveCamera;

	protected:
		/// <summary>Sets up the camera projection.</summary>
		virtual void __activate() const = 0;

	public:
		/// <summary>Checks whether this is the active camera.</summary>
		/// <returns>True if this is the active camera, false otherwise.</returns>
		bool is_active() const;

		/// <summary>Activates the camera.</summary>
		void activate();
	};

	// A camera that does a pixel-perfect orthogonal projection.
	class OrthogonalCamera : public Camera
	{
	protected:
		/// <summary>Sets up an orthogonal camera projection.</summary>
		virtual void __activate() const;
	};

}