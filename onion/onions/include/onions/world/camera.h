#pragma once
#include "../matrix.h"
#include "../graphics/transform.h"
#include "geometry.h"

namespace onion
{
	namespace world
	{

		class World;



#define LEFT_VIEW_EDGE		0
#define BOTTOM_VIEW_EDGE	1
#define RIGHT_VIEW_EDGE		2
#define TOP_VIEW_EDGE		3

		// A method of projecting model space onto the screen.
		class WorldCamera : public Camera, public Shape
		{
		protected:
			friend class World;

			// The bounds of the frame that the camera belongs to.
			const mat2x3i& m_FrameBounds;

			/// <summary>Responds to a change in the frame bounds.</summary>
			virtual void __set_bounds();


			// The camera position. Uses unit coordinates.
			vec3i m_Position;

		public:
			/// <summary>Constructs a camera object.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
			WorldCamera(const mat2x3i& frame_bounds);

			/// <summary>Virtual deconstructor.</summary>
			virtual ~WorldCamera() = default;


			/// <summary>Retrieves the position of the camera in world space.</summary>
			/// <returns>The position of the camera.</returns>
			vec3i get_position() const;

			/// <summary>Sets the position of the camera in world space.</summary>
			/// <param name="position">The new position of the camera.</param>
			void set_position(const vec3i& position);

			/// <summary>Translates the camera's position.</summary>
			/// <param name="trans">The translation of the camera.</param>
			virtual void translate(const vec3i& trans);


			/// <summary>Retrieves a vector representing the direction facing the screen.</summary>
			/// <returns>A vector representing the direction facing the screen.</returns>
			virtual vec3i get_normal() const = 0;


			/// <summary>Compares two shapes to see which should be rendered behind the other.</summary>
			/// <param name="lhs">One shape being compared.</param>
			/// <param name="rhs">The other shape being compared.</param>
			/// <returns>True if lhs should be rendered behind rhs, false if rhs should be rendered behind lhs.</returns>
			virtual bool compare(const Shape* lhs, const Shape* rhs) const = 0;
		};



		// A camera that uses a top-down oblique projection, never changing the angle that the world is viewed from or the scale of the world.
		class StaticTopDownWorldCamera : public WorldCamera
		{
		protected:
			// The minimum visible point on the plane z = 0.
			vec2i m_ZeroPosition;

			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			vec3f support(const vec3f& dir) const;


			/// <summary>Responds to a change in the frame bounds.</summary>
			void __set_bounds();


			/// <summary>Sets up the camera projection.</summary>
			void __activate();

		public:
			/// <summary>Constructs a top-down camera object.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
			StaticTopDownWorldCamera(const mat2x3i& frame_bounds);


			/// <summary>Translates the camera's position.</summary>
			/// <param name="trans">The translation of the camera.</param>
			virtual void translate(const vec3i& trans);


			/// <summary>Retrieves a vector representing the direction facing the screen.</summary>
			/// <returns>A vector representing the direction facing the screen.</returns>
			vec3i get_normal() const;


			/// <summary>Compares two shapes to see which should be rendered behind the other.</summary>
			/// <param name="lhs">One shape being compared.</param>
			/// <param name="rhs">The other shape being compared.</param>
			/// <returns>True if lhs should be rendered behind rhs, false if rhs should be rendered behind lhs.</returns>
			bool compare(const Shape* lhs, const Shape* rhs) const;
		};


		// A camera that uses an axonometric projection with an angle that can change.
		class DynamicAxonometricWorldCamera : public WorldCamera
		{
		protected:
			// The bottom-leftmost point on the plane z = 0.
			vec2i m_ZeroPosition;

			// The radial vectors.
			vec2i m_Radii[2];

			// The vector pointing towards the screen.
			vec3i m_Normal;

			/// <summary>Resets the radial and normal vectors.</summary>
			void reset_vectors();


			/// <summary>Responds to a change in the frame bounds.</summary>
			void __set_bounds();


			/// <summary>A support function for the GJK algorithm.</summary>
			/// <param name="dir">A direction vector.</param>
			/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
			vec3f support(const vec3f& dir) const;


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


			/// <summary>Translates the camera's position.</summary>
			/// <param name="trans">The translation of the camera.</param>
			virtual void translate(const vec3i& trans);


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


			/// <summary>Retrieves a vector representing the direction facing the screen.</summary>
			/// <returns>A vector representing the direction facing the screen.</returns>
			vec3i get_normal() const;


			/// <summary>Compares two shapes to see which should be rendered behind the other.</summary>
			/// <param name="lhs">One shape being compared.</param>
			/// <param name="rhs">The other shape being compared.</param>
			/// <returns>True if lhs should be rendered behind rhs, false if rhs should be rendered behind lhs.</returns>
			bool compare(const Shape* lhs, const Shape* rhs) const;
		};



		// A world camera whose center is bounded inside a rectangular prism of values.
		template <typename T>
		class _BoundedWorldCamera : public T
		{
		protected:
			// The minimum and maximum possible visible points.
			mat2x3i m_CameraBounds;

			/// <summary>Clamps the camera position so that no point visible on-screen lies outside of the camera bounds.</summary>
			void clamp()
			{
				// Clamp the position so that no point on-screen lies outside of the camera bounds
				vec3i trans;
				for (int k = 2; k >= 0; --k)
				{
					Int d = m_FrameBounds.get(k, 1) - m_FrameBounds.get(k, 0);

					Int min_pos = m_CameraBounds.get(k, 0) + (d / 2);
					Int max_pos = m_CameraBounds.get(k, 1) - (d / 2) - (d % 2);

					if (m_Position.get(k) < min_pos)
						trans(k) = min_pos - m_Position.get(k);
					else if (m_Position.get(k) > max_pos)
						trans(k) = max_pos - m_Position.get(k);
					else
						trans(k) = 0;
				}
				T::translate(trans);
			}

			
			/// <summary>Responds to a change in the frame bounds.</summary>
			virtual void __set_bounds()
			{
				clamp();
				T::__set_bounds();
			}

		public:
			/// <summary>Constructs a bounded world camera.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the world that the camera is viewing.</param>
			/// <param name="minimum">The minimum possible point visible on-screen.</param>
			/// <param name="maximum">The maximum possible point visible on-screen.</param>
			/// <param name="args">The other arguments to pass to the class this one inherits from.</param>
			template <typename... _Args>
			_BoundedWorldCamera(const mat2x3i& frame_bounds, const vec3i& minimum, const vec3i& maximum, _Args... args) : T(frame_bounds, args...)
			{
				set_camera_bounds(minimum, maximum);
			}

			/// <summary>Translates the camera's position.</summary>
			/// <param name="trans">The translation of the camera.</param>
			virtual void translate(const vec3i& trans)
			{
				T::translate(trans);
				clamp();
			}

			/// <summary>Sets the minimum and maximum possible values for the camera position.</summary>
			/// <param name="minimum">The minimum possible position for the camera.</param>
			/// <param name="maximum">The maximum possible position for the camera.</param>
			void set_camera_bounds(const vec3i& minimum, const vec3i& maximum)
			{
				for (int k = 2; k >= 0; --k)
				{
					m_CameraBounds(k, 0) = minimum.get(k);
					m_CameraBounds(k, 1) = maximum.get(k);
				}

				// Clamp the position
				clamp();
			}
		};

		typedef _BoundedWorldCamera<StaticTopDownWorldCamera> StaticBoundedTopDownWorldCamera;

	}
}