#pragma once
#include <set>
#include "lighting.h"
#include "agent.h"

namespace onion
{
	namespace world
	{

		class ObjectManager
		{
		private:
			// A cube that stores all lights that should be turned on when the cube is displayed.
			struct Block
			{
				// The cubic shape of the block.
				OrthogonalPrism cube;

				// All static objects within the block.
				std::unordered_set<Object*> objects;

				// All lights that illuminate a point within the block.
				std::unordered_set<LightObject*> lights;

				/// <summary>Constructs a block and inserts a light object into it.</summary>
				/// <param name="pos">The position of the block.</param>
				/// <param name="obj">The first light object to insert.</param>
				Block(const vec3i& pos, Object* obj);

				/// <summary>Constructs a block and inserts a light object into it.</summary>
				/// <param name="pos">The position of the block.</param>
				/// <param name="obj">The first light object to insert.</param>
				Block(const vec3i& pos, LightObject* obj);

				/// <summary>Tests whether the given object has any collisions with an object inside the block.</summary>
				/// <param name="obj">The object to test.</param>
				/// <returns>True if there is a collision and the object needs to be reset to its original position, false otherwise.</returns>
				bool collision(Object* obj);
			};

			// The dimensions of each block.
			static vec3i m_BlockDimensions;

			// The maximum z-coordinate inspected.
			static Int m_UpperBound;

			// A map of all stored blocks.
			std::unordered_map<vec3i, Block*, std::hash<INT_VEC3>> m_Blocks;

			/// <summary>Retrieves the block with the specified index.</summary>
			/// <param name="index">The index of the block to retrieve.</param>
			/// <returns>A pointer to the block.</returns>
			Block* get_block(const vec3i& index);

			/// <summary>Tests whether the object intersects with the block with the specified index.</summary>
			/// <param name="index">The index of the block to be tested.</param>
			/// <param name="obj">The object to be tested.</param>
			/// <returns>True if the object intersects the block, false if it does not.</returns>
			bool contains(const vec3i& index, Object* obj) const;

			/// <summary>Tests whether the object has any collisions with another object being managed.</summary>
			/// <param name="obj">The object being tested.</param>
			/// <returns>True if there is a collision and the object needs to be reset to its original position, false otherwise.</returns>
			bool collision(Object* obj);


			/// <summary>Inserts the given object into all blocks it affects, by checking all blocks radiating outward from the block with the base index.</summary>
			/// <param name="base_index">The index at the block at the center of what is being checked.</param>
			/// <param name="obj">The object to insert.</param>
			/// <returns>True if the object affects the block with the given index, false otherwise.</returns>
			template <typename T, int N>
			bool __insert(const vec3i& base_index, T* obj);


			// The objects that move around.
			std::unordered_set<Actor*> m_Actors;


			// Struct that compares two objects and determines which should be displayed in front of the other.
			struct ObjectComparer
			{
				// The view to use when comparing two objects.
				static const WorldCamera* view;

				/// <summary>Compares two objects using the WorldCamera::View stored.</summary>
				/// <param name="lhs">One object to compare.</param>
				/// <param name="rhs">The other object to compare.</param>
				/// <returns>True if lhs is behind rhs, false otherwise.</returns>
				bool operator()(const Object* lhs, const Object* rhs) const;
			};

			
			// A set of all static objects that were determined to be in view the last time an update pass was run, listed in the order that they should be displayed.
			std::set<Object*, ObjectComparer> m_ActiveObjects;
			
			// A set of all lights that were activated the last time an update pass was run.
			std::unordered_set<LightObject*> m_ActiveLights;

		public:
			/// <summary>Constructs an empty object manager.</summary>
			ObjectManager();
			
			/// <summary>Deletes all blocks constructed for this manager.</summary>
			~ObjectManager();


			/// <summary>Adds an object to be managed. The template parameter should either be Object or LightObject.</summary>
			/// <param name="obj">The object to be managed.</param>
			template <typename T>
			void add(T* obj);


			/// <summary>Resets what is visible, in response to a change in the camera view.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			void reset_visible(const WorldCamera* view);

			/// <summary>Updates which dynamic objects are visible, in response to the passage of time.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			void update_visible(const WorldCamera* view, int frames_passed);

			/// <summary>Displays all visible managed objects.</summary>
			/// <param name="normal">The direction facing towards the camera.</param>
			void display(const vec3i& normal) const;
		};

	}
}