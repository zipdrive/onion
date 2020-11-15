#include "../../../include/onions/world/interactable.h"

namespace onion
{
	namespace world
	{

		Object* _Interactable::m_Interactor{ nullptr };

		void _Interactable::set_interactor(Object* obj)
		{
			m_Interactor = obj;
		}

	}
}