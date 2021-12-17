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


		vec3i ObjectManager::m_BlockDimensions{ 320, 320, 320 };
		Int ObjectManager::m_UpperBound = 320;

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
			// Construct a rectangular prism representing the block
			OrthogonalPrism rect(index * m_BlockDimensions, m_BlockDimensions);

			// Check if the distance between the object and the block is 0
			return obj->get_bounds()->get_distance(&rect) == 0;
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
			// Construct a rectangular prism representing the block
			OrthogonalPrism rect(index * m_BlockDimensions, m_BlockDimensions);

			// Find the minimum (squared) difference from the light to the block
			Int dist = obj->get_bounds()->get_distance(&rect);

			// If the (squared) length of the minimal difference vector is less than or equal to the (squared) radius of the light, then add it to the block
			Int radius = obj->get_light()->radius;
			if (dist < radius)
			{
				if (Block* block = get_block(index))
				{
					// Insert the light into an existing block
					block->lights.insert(obj);
				}
				else
				{
					// Construct a new block, then insert the light
					m_Blocks.emplace(index, new Block(rect.get_position(), obj));
				}
				return true;
			}

			return false;
		}



		const WorldCamera* ObjectManager::ObjectComparer::view{ nullptr };

		bool ObjectManager::ObjectComparer::operator()(const Object* lhs, const Object* rhs) const
		{
			return view->compare(lhs->get_bounds(), rhs->get_bounds());
		}


		void ObjectManager::reset_visible(const WorldCamera* view)
		{
			// A set of all blocks within view.
			std::unordered_set<Block*> active_blocks;

			// Determine which blocks are in view
			// TODO do this more efficiently??
			for (auto iter = m_Blocks.begin(); iter != m_Blocks.end(); ++iter) // Iterate over every block to check if each is visible
			{
				Block* block = iter->second;

				// Check if the block is visible, and add it to the list of active blocks if it is
				if (view->get_distance(&block->cube) == 0)
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
				if (view->get_distance(obj->get_bounds()) == 0)
				{
					m_ActiveObjects.insert(obj);
				}
			}

			// Add any dynamic objects that are visible
			for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
			{
				Object* obj = *iter;

				// Check if the actor is visible
				if (view->get_distance(obj->get_bounds()) == 0)
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

		void ObjectManager::update_visible(const WorldCamera* view, int frames_passed)
		{
			for (auto iter = m_Actors.begin(); iter != m_Actors.end(); ++iter)
			{
				Actor* actor = *iter;

				// Calculate how the actor should (ideally) be translated
				vec3i trans = actor->update(view, frames_passed);

				if (trans.square_sum() > 0)
				{
					// Test whether there are any collisions if the actor is translated as it is supposed to be
					actor->translate(trans);
					if (collision(actor))
					{
						// If there was a collision, reset the position
						actor->translate(-1 * trans);

						vec3i norm = view->get_normal();
						
						// Translate by only the vertical axis
						vec3i dy = vec3i(norm.get(0), norm.get(1), 0);
						dy = dy * Frac(trans.get(1), dy.square_sum());
						actor->translate(dy);
						if (collision(actor))
						{
							actor->translate(-1 * dy);

							// Translate by only the horizontal axis
							vec3i dx = vec3i(vec2i(trans - dy), 0);
							actor->translate(dx);
							if (collision(actor))
							{
								actor->translate(-1 * dx);

								// TODO test other translations?
							}
						}
					}

					// Check if the actor is visible
					if (view->get_distance(actor->get_bounds()) == 0)
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

		void ObjectManager::display(const vec3i& normal) const
		{
			// Display all static objects
			for (auto iter = m_ActiveObjects.rbegin(); iter != m_ActiveObjects.rend(); ++iter)
				(*iter)->display(normal);
		}

	}
}