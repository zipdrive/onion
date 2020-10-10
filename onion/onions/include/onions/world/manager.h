#pragma once
#include "lighting.h"

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
				RectangularPrism cube;

				// All lights that illuminate a point within the cube.
				std::unordered_set<LightObject*> lights;

				/// <summary>Constructs a block and inserts a light object into it.</summary>
				/// <param name="pos">The position of the block.</param>
				/// <param name="obj">The first light object to insert.</param>
				Block(const vec3i& pos, LightObject* obj);
			};

			// The dimensions of each block.
			static vec3i m_BlockDimensions;

			// A map of all stored blocks.
			std::unordered_map<vec3i, Block*, std::hash<INT_VEC3>> m_Blocks;

			/// <summary>Retrieves the block with the specified index.</summary>
			/// <param name="index">The index of the block to retrieve.</param>
			/// <returns>A pointer to the block.</returns>
			Block* get_block(const vec3i& index);


			/// <summary>Inserts the given object into all blocks it affects, by checking all blocks radiating outward from the block with the base index.</summary>
			/// <param name="base_index">The index at the block at the center of what is being checked.</param>
			/// <param name="obj">The object to insert.</param>
			/// <returns>True if the object affects the block with the given index, false otherwise.</returns>
			template <typename T, int N>
			bool __insert(const vec3i& base_index, T* obj);


			// A set of all objects that were determined to be in view the last time an update pass was run.
			std::vector<Object*> m_ActiveObjects;
			
			// A set of all lights that were activated the last time an update pass was run.
			std::unordered_set<LightObject*> m_ActiveLights;

		public:
			/// <summary>Deletes all blocks constructed for this manager.</summary>
			~ObjectManager();


			/// <summary>Adds an object to be managed. The template parameter should either be Object or LightObject.</summary>
			/// <param name="obj">The object to be managed.</param>
			template <typename T>
			void add(T* obj);


			/// <summary>Resets what is visible, in response to a change in the camera view.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			void reset_visible(const WorldCamera::View& view);

			/// <summary>Updates which dynamic objects are visible, in response to the passage of time.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			void update_visible(const WorldCamera::View& view);

			/// <summary>Displays all visible managed objects.</summary>
			void display() const;
		};

	}
}