#pragma once
#include "../matrix.h"
#include "../graphics/transform.h"
#include "geometry.h"

namespace onion
{
	namespace world
	{

		/*
			EXPLANATION OF THE TWO KINDS OF WORLD COORDINATES
			Unit coordinates: used to calculate position, velocity, boundaries, etc.
			Pixel coordinates: maps unit coordinates to the screen
		*/



#define LEFT_VIEW_EDGE		0
#define BOTTOM_VIEW_EDGE	1
#define RIGHT_VIEW_EDGE		2
#define TOP_VIEW_EDGE		3

		// A method of projecting model space onto the screen.
		class WorldCamera : public Camera
		{
		protected:
			// The bounds of the frame that the camera belongs to.
			const mat2x3i& m_FrameBounds;


			// The camera position. Uses unit coordinates.
			vec3i m_Position;

		public:
			// The geometry of the space visible from the camera. Uses unit coordinates.
			class View : public Shape
			{
			protected:
				// The bounds of the world frame.
				const mat2x3i& m_FrameBounds;

			public:
				/// <summary>Constructs the view geometry.</summary>
				/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
				View(const mat2x3i& frame_bounds);


				/// <summary>Retrieves a vector representing the direction facing the screen.</summary>
				/// <returns>A vector representing the direction facing the screen.</returns>
				virtual vec3i get_normal() const = 0;


				/// <summary>Compares two shapes to see which should be rendered behind the other.</summary>
				/// <param name="lhs">One shape being compared.</param>
				/// <param name="rhs">The other shape being compared.</param>
				/// <returns>True if lhs should be rendered behind rhs, false if rhs should be rendered behind lhs.</returns>
				virtual bool compare(const Shape* lhs, const Shape* rhs) const = 0;
			};


			/// <summary>Constructs a camera object.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
			WorldCamera(const mat2x3i& frame_bounds);

			/// <summary>Virtual deconstructor.</summary>
			virtual ~WorldCamera() = default;


			/// <summary>Retrieves the position of the camera in world space, using unit coordinates.</summary>
			/// <returns>The position of the camera.</returns>
			const vec3i& get_position() const;

			/// <summary>Sets the position of the camera in world space, using unit coordinates.</summary>
			/// <param name="position">The new position of the camera.</param>
			void set_position(const vec3i& position);

			/// <summary>Translates the camera's position, using unit coordinates.</summary>
			/// <param name="trans">The translation of the camera.</param>
			virtual void translate(const vec3i& trans);

			
			/// <summary>Retrieves the visible space, using unit coordinates.</summary>
			/// <param name="view">Outputs the space visible from the camera.</param>
			virtual const View* get_view() const = 0;

			/// <summary>Resets the visible space.</summary>
			virtual void reset_view() = 0;
		};


		// A camera that uses a top-down oblique projection, never changing the angle that the world is viewed from or the scale of the world.
		class StaticTopDownWorldCamera : public WorldCamera
		{
		protected:
			class StaticTopDownView : public View
			{
			protected:
				// The minimum visible point on the plane z = 0.
				vec2i m_Position;


				/// <summary>A support function for the GJK algorithm.</summary>
				/// <param name="dir">A direction vector.</param>
				/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
				vec3i support(const vec3i& dir) const;

			public:
				/// <summary>Constructs the view geometry.</summary>
				/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
				StaticTopDownView(const mat2x3i& frame_bounds);


				/// <summary>Retrieves the position of the shape.</summary>
				/// <returns>A reference to the position of the shape.</returns>
				vec3i get_position() const;

				/// <summary>Sets the center of the shape.</summary>
				/// <param name="pos">The new center of the shape.</param>
				void set_position(const vec3i& pos);

				/// <summary>Translates the shape by a given vector.</summary>
				/// <param name="trans">The vector of translation.</param>
				void translate(const vec3i& trans);


				/// <summary>Retrieves a vector representing the direction facing the screen.</summary>
				/// <returns>A vector representing the direction facing the screen.</returns>
				vec3i get_normal() const;


				/// <summary>Compares two shapes to see which should be rendered behind the other.</summary>
				/// <param name="lhs">One shape being compared.</param>
				/// <param name="rhs">The other shape being compared.</param>
				/// <returns>True if lhs should be rendered behind rhs, false if rhs should be rendered behind lhs.</returns>
				bool compare(const Shape* lhs, const Shape* rhs) const;
			} 
			m_View;


			/// <summary>Sets up the camera projection.</summary>
			void __activate();

		public:
			/// <summary>Constructs a top-down camera object.</summary>
			/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
			StaticTopDownWorldCamera(const mat2x3i& frame_bounds);


			/// <summary>Translates the camera's position, using unit coordinates.</summary>
			/// <param name="trans">The translation of the camera.</param>
			void translate(const vec3i& trans);


			/// <summary>Retrieves the visible space, using unit coordinates.</summary>
			/// <param name="view">Outputs the space visible from the camera.</param>
			const View* get_view() const;

			/// <summary>Resets the visible space.</summary>
			void reset_view();
		};


		// A camera that uses an axonometric projection with an angle that can change.
		class DynamicAxonometricWorldCamera : public WorldCamera
		{
		protected:
			class DynamicAxonometricView : public View
			{
			protected:
				// The bottom-leftmost point on the plane z = 0.
				vec2i m_Position;

				// The radial vectors.
				vec2i m_Radii[2];

				// The vector pointing towards the screen.
				vec3i m_Normal;


				/// <summary>A support function for the GJK algorithm.</summary>
				/// <param name="dir">A direction vector.</param>
				/// <param name="point">Outputs the point on the shape that produces the largest dot product with dir.</param>
				vec3i support(const vec3i& dir) const;

			public:
				/// <summary>Constructs the view geometry.</summary>
				/// <param name="frame_bounds">A reference to the bounds of the frame that the camera belongs to.</param>
				DynamicAxonometricView(const mat2x3i& frame_bounds);


				/// <summary>Retrieves the position of the shape.</summary>
				/// <returns>A reference to the position of the shape.</returns>
				vec3i get_position() const;

				/// <summary>Sets the center of the shape.</summary>
				/// <param name="pos">The new center of the shape.</param>
				void set_position(const vec3i& pos);

				/// <summary>Translates the shape by a given vector.</summary>
				/// <param name="trans">The vector of translation.</param>
				void translate(const vec3i& trans);


				/// <summary>Retrieves a vector representing the direction facing the screen.</summary>
				/// <returns>A vector representing the direction facing the screen.</returns>
				vec3i get_normal() const;


				/// <summary>Compares two shapes to see which should be rendered behind the other.</summary>
				/// <param name="lhs">One shape being compared.</param>
				/// <param name="rhs">The other shape being compared.</param>
				/// <returns>True if lhs should be rendered behind rhs, false if rhs should be rendered behind lhs.</returns>
				bool compare(const Shape* lhs, const Shape* rhs) const;
			}
			m_View;


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


			/// <summary>Resets the visible space.</summary>
			void reset_view();


			/// <summary>Retrieves the 2D direction that model space is being viewed from.</summary>
			/// <param name="direction">Outputs the direction facing towards the camera. Each element is equal to:
			/// +1 if the positive axis is facing towards the camera;
			/// -1 if the negative axis is facing towards the camera;
			/// +0 if the axis is perpendicular to the screen.</param>
			void get_view(vec2i& direction) const;
		};

	}
}