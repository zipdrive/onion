#pragma once
#include "gui.h"


// Used to edit stuff.
class Editor
{
public:
	/// <summary>Displays the editor.</summary>
	virtual void display() = 0;

	/// <summary>Freezes input for the editor.</summary>
	virtual void freeze() = 0;

	/// <summary>Unfreezes input for the editor.</summary>
	virtual void unfreeze() = 0;
};