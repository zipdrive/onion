#pragma once

#include "editor.h"


class SpriteSheetScrollBar;


class SpriteSheetEditor : public Editor
{
private:
	class SpriteSheetScrollBar : public ScrollBar
	{

	};

	// The sprite sheet
	SpriteSheet* m_SpriteSheet;



	// Horizontal scroll bar
	SpriteSheetScrollBar* m_HorizontalScrollBar;

	// Vertical scroll bar
	SpriteSheetScrollBar* m_VerticalScrollBar;

public:
	//SpriteSheetEditor();

	/// <summary>Displays the chunk editor.</summary>
	void display();

	/// <summary>Freezes input for the editor.</summary>
	void freeze();

	/// <summary>Unfreezes input for the editor.</summary>
	void unfreeze();
};