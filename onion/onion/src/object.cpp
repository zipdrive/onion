#include "..\include\object.h"

void LayeredFrame::add(Object* object)
{
	m_Objects.push_back(object);
}

void LayeredFrame::remove(Object* object)
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end(); ++iter)
	{
		if (*iter == object)
		{
			m_Objects.erase(iter);
			return;
		}
	}
}

void LayeredFrame::display()
{

}