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


// Editor for the list of sprite sheets
class SpriteSheetsEditor : public Editor
{
private:
	class SpriteSheetsList : public Frame
	{
	public:
		// The currently highlighted sprite sheet.
		static std::string m_Highlighted;

		int get_height() const;

		void display() const;

	} m_SpriteSheetsList;

	// The button to create a new sprite sheet.
	GUITextButton* m_NewSpriteSheetButton;

	// The button to edit the currently selected sprite sheet.
	GUITextButton* m_EditSpriteSheetButton;

	// The button to delete the currently selected sprite sheet.
	GUITextButton* m_DeleteSpriteSheetButton;

public:
	/// <summary>Constructs the sprite sheets editor.</summary>
	SpriteSheetsEditor();

	static std::string& get_highlighted_sprite_sheet();

	/// <summary>Displays the chunk editor.</summary>
	void display();

	/// <summary>Freezes input for the editor.</summary>
	void freeze();

	/// <summary>Unfreezes input for the editor.</summary>
	void unfreeze();
};