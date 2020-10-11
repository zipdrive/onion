#include <algorithm>
#include "../../../include/onions/world/manager.h"

namespace onion
{
	namespace world
	{

		ObjectManager::~ObjectManager()
		{
			for (auto iter = m_Blocks.begin(); iter != m_Blocks.end(); ++iter)
				delete iter->second;
		}


		vec3i ObjectManager::m_BlockDimensions{ UNITS_PER_PIXEL * 200, UNITS_PER_PIXEL * 200, UNITS_PER_PIXEL * 2000 };

		ObjectManager::Block::Block(const vec3i& pos, LightObject* light) : cube(pos, m_BlockDimensions)
		{
			lights.insert(light);
		}

		ObjectManager::Block* ObjectManager::get_block(const vec3i& index)
		{
			auto iter = m_Blocks.find(index);
			if (iter != m_Blocks.end())
				return iter->second;
			return nullptr;
		}


		template <typename T, int N>
		bool ObjectManager::__insert(const vec3i& base_index, T* obj)
		{
			if (!__insert<T, N - 1>(base_index, obj))
				return false;

			// Iterate in the positive nth-direction
			vec3i index = base_index;
			do
			{
				++index(N);
			} 
			while (__insert<T, N - 1>(index, obj));

			// Iterate in the negative nth-direction
			index = base_index;
			do
			{
				--index(N);
			} 
			while (!(N == 2 && index.get(N) < 0) // Ignore negative z-indices
				&& __insert<T, N - 1>(index, obj));

			if (base_index == index)
				return false;

			return true;
		}

		template <typename T>
		void ObjectManager::add(T* obj)
		{
			// Calculates the index of the block that the position of the object is in
			vec3i base_index = obj->get_bounds()->get_position();
			for (int k = 2; k >= 0; --k)
				base_index(k) /= m_BlockDimensions.get(k);

			// Starting from the base index and radiating outwards, inserts the object into all blocks that it affects
			__insert<T, 2>(base_index, obj);
		}

		template <>
		void ObjectManager::add<Object>(Object* obj)
		{
			// Calculates the index of the block that the position of the object is in
			vec3i base_index = obj->get_bounds()->get_position();
			for (int k = 2; k >= 0; --k)
				base_index(k) /= m_BlockDimensions.get(k);

			// Starting from the base index and radiating outwards, inserts the object into all blocks that it affects
			__insert<Object, 2>(base_index, obj);

			// Add the object's lighting data, if it has any
			if (LightObject* light = static_cast<LightObject*>(obj))
			{
				add<LightObject>(light);
			}
		}


		template <>
		bool ObjectManager::__insert<Object, -1>(const vec3i& index, Object* obj)
		{
			return false;
		}


		template <>
		bool ObjectManager::__insert<LightObject, -1>(const vec3i& index, LightObject* obj)
		{
			// Calculates the minimum corner of the block
			vec3i mins = index * m_BlockDimensions;
			vec3i maxs = mins + m_BlockDimensions;

			// Calculate the center of the block
			vec3i center = mins;
			for (int k = 2; k >= 0; --k)
				center(k) += (m_BlockDimensions.get(k) / 2);

			// Find the closest point on the light to the block
			vec3i closest;
			obj->get_bounds()->get_closest_point(center, closest);

			// Get the minimal difference vector between the light and the block
			vec3i diff;
			for (int k = 2; k >= 0; --k)
			{
				if (closest.get(k) < mins.get(k))
					diff(k) = mins.get(k) - closest.get(k);
				else if (closest.get(k) > maxs.get(k))
					diff(k) = maxs.get(k) - closest.get(k);
				else
					diff(k) = 0;
			}

			// If the (squared) length of the minimal difference vector is less than or equal to the (squared) radius of the light, then add it to the block
			Int radius = obj->get_light()->radius;
			if (diff.square_sum() < radius * radius)
			{
				if (Block* block = get_block(index))
				{
					// Insert the light into an existing block
					block->lights.insert(obj);
				}
				else
				{
					// Construct a new block, then insert the light
					m_Blocks.emplace(index, new Block(mins, obj));
				}
				return true;
			}

			return false;
		}



		void ObjectManager::reset_visible(const WorldCamera::View& view)
		{
			// A set of all blocks within view.
			std::unordered_set<Block*> active_blocks;

			// Determine which blocks are in view
			// TODO do this more efficiently??
			for (auto iter = m_Blocks.begin(); iter != m_Blocks.end(); ++iter) // Iterate over every block to check if each is visible
			{
				Block* block = iter->second;
				
				// Get the closest point to the ray through the center of the screen
				vec3i closest;
				block->cube.get_closest_point(view.center, closest);

				// Check if the closest point in the block to the center of the screen is visible, and add it to the list of active blocks if it is
				if (view.is_visible(closest))
					active_blocks.insert(block);
			}

			std::unordered_set<LightObject*> active_lights; // A set of all lights illuminating a block within view
			for (auto iter = active_blocks.begin(); iter != active_blocks.end(); ++iter)
			{
				// Add all lights illuminating the block to the set of active lights
				active_lights.insert((*iter)->lights.begin(), (*iter)->lights.end());
			}

			// Turn off all lights no longer in view
			for (auto iter = m_ActiveLights.begin(); iter != m_ActiveLights.end(); ++iter)
				if (active_lights.count(*iter) < 1)
					(*iter)->toggle(false);
			// Turn on all lights that just came into view, and update all other lights
			for (auto iter = active_lights.begin(); iter != active_lights.end(); ++iter)
				(*iter)->toggle(true);
			// Update the list of active lights
			m_ActiveLights = active_lights;
		}

		void ObjectManager::update_visible(const WorldCamera::View& view)
		{
			// TODO
		}

		void ObjectManager::display() const
		{
			// TODO
		}

	}
}