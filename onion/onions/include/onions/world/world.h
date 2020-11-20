#pragma once
#include "../graphics/frame.h"
#include "../state.h"
#include "chunk.h"
#include "camera.h"

namespace onion
{
	namespace world
	{

		// Handles loading and displaying chunks
		class World : public Frame, public UpdateListener
		{
		protected:
			// The camera that displays the world.
			WorldCamera* m_Camera;

			/// <summary>Resets what is visible in response to a camera update.</summary>
			virtual void reset_camera() = 0;

			/// <summary>Responds to an adjustment of the frame bounds.</summary>
			virtual void __set_bounds();

		public:
			/// <summary>Sets up the camera.</summary>
			World();

			/// <summary>Destroys the camera.</summary>
			virtual ~World();

			/// <summary>Displays the world.</summary>
			virtual void display() const;
		};

		// A state that displays the world.
		class WorldState : public State
		{
		protected:
			// The world.
			World* m_World;

		public:
			/// <summary>Constructs a state that displays the world.</summary>
			/// <param name="world">The world to display.</param>
			WorldState(World* world);

			/// <summary>Is called when the state is made or when the window is resized.</summary>
			/// <param name="width">The width of the window.</param>
			/// <param name="height">The height of the window.</param>
			virtual void set_bounds(int width, int height);

			/// <summary>Displays the world.</summary>
			virtual void display() const;
		};


		// A world where only one chunk is loaded into memory at a time and the camera is fixed.
		class BasicWorld : public World
		{
		protected:
			// The current chunk loaded into memory.
			Chunk* m_Chunk;

			/// <summary>Updates what is visible in response to a camera update.</summary>
			void reset_camera();

			/// <summary>Updates the world.</summary>
			/// <param name="frames_passed">The number of frames passed since the last update.</param>
			virtual void update(int frames_passed);

			/// <summary>Displays the world.</summary>
			void __display() const;

		public:
			/// <summary>Sets the current chunk
			BasicWorld(Chunk* chunk);

			/// <summary>Sets the chunk being displayed.</summary>
			void set_chunk(Chunk* chunk);
		};


		// A world where the camera follows an actor around.
		template <typename T>
		class _ActorFollowingWorld : public T
		{
		protected:
			// The actor being followed.
			Actor* m_Following;


			// The percentage of the frame bounds taken up by the center.
			vec3f m_CenterDimensions;

			// The bounds of the area that the actor can move around in without adjusting the camera.
			mat2x3i m_CenterBounds;


			/// <summary>Responds to an adjustment of the frame bounds.</summary>
			virtual void __set_bounds()
			{
				T::__set_bounds();

				// Adjust the center bounds around the current camera position
				vec3i pos = m_Camera->get_position();
				for (int k = 2; k >= 0; --k)
				{
					Int d = (Int)floor(m_CenterDimensions.get(k) * (m_Bounds.get(k, 1) - m_Bounds.get(k, 0)));
					m_CenterBounds.set(k, 0, pos.get(k) - (d / 2));
					m_CenterBounds.set(k, 1, m_CenterBounds.get(k, 0) + d);
				}
			}

			/// <summary>Follows the object around.</summary>
			/// <param name="frames_passed">The number of frames passed since the last update.</param>
			virtual void update(int frames_passed)
			{
				// Call the parent's update function
				T::update(frames_passed);

				// Translate the camera to follow the actor
				vec3i pos = m_Following->get_bounds()->get_position();

				vec3i t;
				for (int k = 2; k >= 0; --k)
				{
					if (pos.get(k) < m_CenterBounds.get(k, 0))
						t(k) = pos.get(k) - m_CenterBounds.get(k, 0);
					else if (pos.get(k) > m_CenterBounds.get(k, 1))
						t(k) = pos.get(k) - m_CenterBounds.get(k, 1);
					else
						t(k) = 0;
				}

				vec3i trans = m_Camera->get_position(); // The actual translation of the camera
				m_Camera->translate(t);
				trans = m_Camera->get_position() - trans;

				bool trans_nonzero = false;
				for (int k = 2; k >= 0; --k)
				{
					if (trans.get(k) != 0)
						trans_nonzero = true;

					for (int m = 1; m >= 0; --m)
						m_CenterBounds(k, m) += trans.get(k);
				}

				// Reset what is visible if the camera moved
				if (trans_nonzero)
					reset_camera();
			}

		public:
			/// <summary>Constructs a world where the camera follows an actor.</summary>
			/// <param name="following">The actor that the camera follows.</param>
			/// <param name="center_dimensions">The percentage of the bounds taken up by the center.</param>
			/// <param name="args">The arguments to pass to the parent class.</param>
			template <typename... _Args>
			_ActorFollowingWorld(Actor* following, const vec3f& center_dimensions, _Args... args) : T(args...)
			{
				m_Following = following;

				vec3i pos = m_Following->get_bounds()->get_position();
				m_Camera->set_position(pos);
				pos = m_Camera->get_position();

				m_CenterDimensions = center_dimensions;
				for (int k = 2; k >= 0; --k)
				{
					Int d = (Int)floor(center_dimensions.get(k) * (m_Bounds.get(k, 1) - m_Bounds.get(k, 0)));
					m_CenterBounds.set(k, 0, pos.get(k) - (d / 2));
					m_CenterBounds.set(k, 1, m_CenterBounds.get(k, 0) + d);
				}
			}
		};

		typedef _ActorFollowingWorld<BasicWorld> ActorFollowingBasicWorld;

	}
}