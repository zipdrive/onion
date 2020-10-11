#pragma once
#include "../matrix.h"
#include "../graphics/transform.h"
#include "geometry.h"

namespace onion
{
	namespace world
	{


#define UNITS_PER_PIXEL		32

#define LEFT_VIEW_EDGE		0
#define RIGHT_VIEW_EDGE		1
#define BOTTOM_VIEW_EDGE	2
#define TOP_VIEW_EDGE		3

		// A method of projecting model space onto the screen.
		class WorldCamera : public Camera
		{
		public:
			// The geometry of the space visible from the camera.
			struct View
			{
				// The ray through the center of the screen, pointing away from the screen.
				Ray center;

				/// <summary>The four planes marking the edges of the view perspective.
				/// The normals for each plane point inwards, towards the center.
				/// The dot product of any visible point with the normal of any plane will be greater than the dot product of any point on the plane with the normal for that plane.</summary>
				Plane edges[4];

				/// <summary>Calculates if a point in world space is visible on screen.</summary>
				/// <param name="pos">A point in world space.</param>
				/// <returns>True if the point is visible, false otherwise.</returns>
				bool is_visible(const vec3i& pos) const;
			};

		protected:
			// The bounds of the frame that the camera belongs to.
			const mat2x3i& m_FrameBounds;


			// The camera position.
			vec3i m_Position;

			// The space visible from the camera.
			View m_View;

		public:
			/// <summary>Constructs a camera object.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
			WorldCamera(const mat2x3i& frame_bounds);

			/// <summary>Virtual deconstructor.</summary>
			virtual ~WorldCamera() = default;


			/// <summary>Retrieves the position of the camera in model space.</summary>
			/// <returns>The position of the camera.</returns>
			const vec3i& get_position() const;

			/// <summary>Sets the position of the camera in model space.</summary>
			/// <param name="position">The new position of the camera.</param>
			void set_position(const vec3i& position);

			/// <summary>Translates the camera's position.</summary>
			/// <param name="trans">The translation of the camera.</param>
			virtual void translate(const vec3i& trans);


			/// <summary>Retrieves the visible space.</summary>
			/// <param name="view">Outputs the space visible from the camera.</param>
			const View& get_view() const;

			/// <summary>Retrieves the 2D direction that model space is being viewed from.</summary>
			/// <param name="direction">Outputs the direction facing towards the camera. Each element is equal to:
			/// +1 if the positive axis is facing towards the camera;
			/// -1 if the negative axis is facing towards the camera;
			/// +0 if the axis is perpendicular to the screen.</param>
			virtual void get_view(vec2i& direction) const = 0;
		};


		// A camera that uses a top-down oblique projection, never changing the angle that the world is viewed from or the scale of the world.
		class StaticTopDownWorldCamera : public WorldCamera
		{
		protected:
			/// <summary>Sets up the camera projection.</summary>
			void __activate();

		public:
			/// <summary>Constructs a top-down camera object.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
			StaticTopDownWorldCamera(const mat2x3i& frame_bounds);


			/// <summary>Retrieves the 2D direction that model space is being viewed from.</summary>
			/// <param name="direction">Outputs the direction facing towards the camera. Each element is equal to:
			/// +1 if the positive axis is facing towards the camera;
			/// -1 if the negative axis is facing towards the camera;
			/// +0 if the axis is perpendicular to the screen.</param>
			void get_view(vec2i& direction) const;
		};


		// A camera that uses an axonometric projection with an angle that can change.
		class DynamicAxonometricWorldCamera : public WorldCamera
		{
		protected:
			// The angle that the model space is viewed at from the top, in radians. 0 is from the top, PI/2 is from the side.
			float m_TopViewAngle;

			// The angle that the model space is viewed at from the side, in radians.
			float m_SideViewAngle;

			/// <summary>Sets up the camera projection.</summary>
			void __activate();

		public:
			/// <summary>Constructs a top-down camera object.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
			/// <param name="top_view_angle"></param>
			/// <param name="side_view_angle">The angle that </param>
			DynamicAxonometricWorldCamera(const mat2x3i& frame_bounds, float top_view_angle, float side_view_angle);


			/// <summary>Sets the top-down angle that the model space is viewed from.</summary>
			/// <param name="angle">An angle, in radians. 0 is from the top, PI/2 is from the side.</param>
			void set_top(float angle);

			/// <summary>Rotates the top-down angle that the model space is viewed from.</summary>
			/// <param name="angle">The change in angle, in radians.</param>
			void rotate_top(float angle);

			/// <summary>Sets the angle of rotation of the model space around the z-axis.</summary>
			/// <param name="angle">An angle, in radians.</param>
			void set_side(float angle);

			/// <summary>Rotates the model space around the z-axis.</summary>
			/// <param name="angle">The change in angle, in radians.</param>
			void rotate_side(float angle);


			/// <summary>Retrieves the 2D direction that model space is being viewed from.</summary>
			/// <param name="direction">Outputs the direction facing towards the camera. Each element is equal to:
			/// +1 if the positive axis is facing towards the camera;
			/// -1 if the negative axis is facing towards the camera;
			/// +0 if the axis is perpendicular to the screen.</param>
			void get_view(vec2i& direction) const;
		};

	}
}