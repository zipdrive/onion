#include <algorithm>
#include "../../../include/onions/world/manager.h"

namespace onion
{
	namespace world
	{

		ObjectManager::ObjectManager() : m_ActiveObjects(ObjectManager::ObjectComparer()) {}
		
		ObjectManager::~ObjectManager()
		{
			for (auto iter = m_Blocks.begin(); iter != m_Blocks.end(); ++iter)
				delete iter->second;

			for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
				delete *iter;
		}


		vec3i ObjectManager::m_BlockDimensions{ UNITS_PER_PIXEL * 320, UNITS_PER_PIXEL * 320, UNITS_PER_PIXEL * 320 };
		Int ObjectManager::m_UpperBound = UNITS_PER_PIXEL * 320;

		ObjectManager::Block::Block(const vec3i& pos, Object* obj) : cube(pos, m_BlockDimensions)
		{
			objects.insert(obj);
		}
		
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

		bool ObjectManager::Block::collision(Object* obj)
		{
			for (auto iter = objects.begin(); iter != objects.end(); ++iter)
				if ((*iter)->collision(obj))
					return true;
			return false;
		}

		bool ObjectManager::contains(const vec3i& index, Object* obj) const
		{
			// Calculates the minimum and maximum corner of the block
			vec3i mins = index * m_BlockDimensions;
			vec3i maxs = mins + m_BlockDimensions;

			// Calculate the center of the block
			vec3i center = mins;
			for (int k = 2; k >= 0; --k)
				center(k) += (m_BlockDimensions.get(k) / 2);

			// Find the closest point on the object to the block
			vec3i closest;
			obj->get_bounds()->get_closest_point(center, closest);

			// Check whether the closest point lies within the block
			for (int k = 2; k >= 0; --k)
			{
				if (closest.get(k) < mins.get(k) || closest.get(k) > maxs.get(k))
					return false;
			}

			return true;
		}

		bool ObjectManager::collision(Object* obj)
		{
			const Shape* bounds = obj->get_bounds();
			vec3i base_index = bounds->get_position();
			for (int k = 2; k >= 0; --k)
				base_index(k) /= m_BlockDimensions.get(k);

			for (Int i = -1; i <= 1; ++i)
			{
				for (Int j = -1; j <= 1; ++j)
				{
					for (Int k = -1; k <= 1; ++k)
					{
						vec3i index = base_index + vec3i(i, j, k);
						if (index.get(2) >= 0)
						{
							if (Block* block = get_block(index))
							{
								if (contains(index, obj))
								{
									if (block->collision(obj))
										return true;
								}
							}
						}
					}
				}
			}

			return false;
		}


		template <typename T, int N>
		bool ObjectManager::__insert(const vec3i& base_index, T* obj)
		{
			if (N == 2)
			{
				Int z = base_index.get(2) * m_BlockDimensions.get(2);
				if (z >= 0 && z < m_UpperBound)
				{
					__insert<T, N - 1>(base_index, obj);
				}

				vec3i index = base_index;
				do
				{
					++index(2);
				} 
				while (index.get(N) * m_BlockDimensions.get(N) < m_UpperBound
					&& (index.get(N) < 0 || __insert<T, 1>(index, obj)));

				index = base_index;
				do
				{
					--index(2);
				} 
				while (index.get(N) >= 0
					&& (index.get(N) * m_BlockDimensions.get(N) >= m_UpperBound || __insert<T, 1>(index, obj)));

				return true;
			}
			else
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
				while (__insert<T, N - 1>(index, obj));

				if (base_index == index)
					return false;

				return true;
			}
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
			if (Actor* actor = dynamic_cast<Actor*>(obj))
			{
				m_Actors.insert(actor);
			}
			else
			{
				// Calculates the index of the block that the position of the object is in
				vec3i base_index = obj->get_bounds()->get_position();
				for (int k = 2; k >= 0; --k)
					base_index(k) /= m_BlockDimensions.get(k);

				// Starting from the base index and radiating outwards, inserts the object into all blocks that it affects
				__insert<Object, 2>(base_index, obj);

				// Add the object's lighting data, if it has any
				if (LightObject* light = dynamic_cast<LightObject*>(obj))
				{
					add<LightObject>(light);
				}
			}
		}


		template <>
		bool ObjectManager::__insert<Object, -1>(const vec3i& index, Object* obj)
		{
			// Check that the object lies inside the block
			if (!contains(index, obj))
				return false;

			// Add the object to the block
			if (Block* block = get_block(index))
			{
				// Insert the light into an existing block
				block->objects.insert(obj);
			}
			else
			{
				// Construct a new block, then insert the light
				m_Blocks.emplace(index, new Block(index * m_BlockDimensions, obj));
			}

			return true;
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
			Int radius = obj->get_light()->radius * UNITS_PER_PIXEL;
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



		WorldCamera::View ObjectManager::ObjectComparer::view{};
		
		bool ObjectManager::ObjectComparer::compare(const Object* lhs, const Object* rhs)
		{
			// TODO
			// this is a heuristic for which should be displayed in front, but there are edge case exceptions
			
			// Calculate the point in the bottom-left of the screen at the upper bound for z-coordinates
			const vec3i& n1 = view.edges[BOTTOM_VIEW_EDGE].normal;
			Int d1 = view.edges[BOTTOM_VIEW_EDGE].dot;
			const vec3i& n2 = view.edges[LEFT_VIEW_EDGE].normal;
			Int d2 = view.edges[LEFT_VIEW_EDGE].dot;

			Int x, y, z = m_UpperBound;

			if (n1.get(0) != 0)
			{
				Int denom = (n1.get(0) * n2.get(1)) - (n1.get(1) * n2.get(0)); // Only 0 if both normals are parallel
				y = ((n1.get(0) * (d2 - (n2.get(2) * z))) - (n2.get(0) * (d1 - (n1.get(2) * z)))) / denom;
				x = (d1 - (n1.get(1) * y) - (n1.get(2) * z)) / n1.get(0);
			}
			else // TODO maybe check in case n1 = (0, 0, 1)?
			{
				y = (d1 - (n1.get(2) * z)) / n1.get(1);
				x = (d2 - (n2.get(1) * y) - (n2.get(2) * z)) / n2.get(0);
			}

			// Create a ray spanning the bottom of the screen
			Ray bottom_near = { vec3i(x, y, z), view.edges[LEFT_VIEW_EDGE].normal };
			
			// Get the point on each object's bounds that is closest to the screen depth-wise and farthest towards the bottom of the screen
			vec3i closest_lhs, closest_rhs;
			lhs->get_bounds()->get_closest_point(bottom_near, closest_lhs);
			rhs->get_bounds()->get_closest_point(bottom_near, closest_rhs);

			// Compare dot products with the to see which is closer to the bottom of the screen
			Int d_lhs = n1.dot(closest_lhs) - d1;
			Int d_rhs = n1.dot(closest_rhs) - d1;
			if (d_lhs != d_rhs)
				return d_lhs > d_rhs; // True if LHS is closer to top of screen
			else if (closest_lhs.get(2) != closest_rhs.get(2))
				return closest_lhs.get(2) < closest_rhs.get(2); // True if LHS is lower down than RHS
			else
			{
				d_lhs = n2.dot(closest_lhs) - d2;
				d_rhs = n2.dot(closest_rhs) - d2;
				return d_lhs > d_rhs;
			}
		}

		bool ObjectManager::ObjectComparer::operator()(const Object* lhs, const Object* rhs) const
		{
			return compare(lhs, rhs);
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

			std::unordered_set<Object*> active_objects; // A set of all visible objects
			std::unordered_set<LightObject*> active_lights; // A set of all lights illuminating a block within view
			for (auto iter = active_blocks.begin(); iter != active_blocks.end(); ++iter)
			{
				// Add all static objects in active blocks
				active_objects.insert((*iter)->objects.begin(), (*iter)->objects.end());

				// Add all lights illuminating the block to the set of active lights
				active_lights.insert((*iter)->lights.begin(), (*iter)->lights.end());
			}

			// Reset which static objects are visible and in which order
			m_ActiveObjects.clear();
			ObjectManager::ObjectComparer::view = view;
			for (auto iter = active_objects.begin(); iter != active_objects.end(); ++iter)
			{
				Object* obj = *iter;

				// Check if the object is visible
				if (view.is_visible(obj->get_bounds()))
				{
					m_ActiveObjects.insert(obj);
				}
			}

			// Add any dynamic objects that are visible
			for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
			{
				Object* obj = *iter;

				// Check if the actor is visible
				if (view.is_visible(obj->get_bounds()))
				{
					m_ActiveObjects.insert(obj);
				}
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

		void ObjectManager::update_visible(const WorldCamera::View& view, int frames_passed)
		{
			for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
			{
				Actor* actor = *iter;
				Shape* bounds = actor->get_bounds();
				vec3i original = bounds->get_position();

				// Calculate how the actor should (ideally) be translated
				vec3i trans = actor->update(view, frames_passed);

				if (trans.square_sum() > 0)
				{
					// Test whether there are any collisions if the actor is translated as it is supposed to be
					bounds->translate(trans);
					if (collision(actor))
					{
						// If there was a collision, reset the position
						bounds->set_position(original);

						// TODO test whether there isn't a collision if the actor is only translated on one axis, or if it is pushed upwards
					}

					// Check if the actor is visible
					if (view.is_visible(bounds))
					{
						// Add the actor to the set of visible objects, if it isn't already included
						m_ActiveObjects.insert(actor);
					}
					else
					{
						// Remove the actor from the set of visible objects, if it is currently in the set
						m_ActiveObjects.erase(actor);
					}
				}
			}
		}

		void ObjectManager::display(const Ray& center) const
		{
			// Display all static objects
			for (auto iter = m_ActiveObjects.begin(); iter != m_ActiveObjects.end(); ++iter)
				(*iter)->display(center);
		}

	}
}