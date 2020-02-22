#include "..\include\onion.h"

void ONIONobject::draw()
{
	m_Visual->draw();
}

int ONIONobject::update()
{
	return ONION_PASS;
}