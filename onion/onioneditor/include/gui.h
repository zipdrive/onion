#pragma once

#include <onion.h>


#define UP_ARROW_SPRITE			1
#define LEFT_ARROW_SPRITE		2
#define SCROLL_BAR				3


SpriteSheet* get_gui_sprite_sheet();

const Palette* get_gui_palette();

Font* get_gui_font();

const Palette* get_gui_font_palette();


SlicedGraphic* get_gui_pane();



// A button that displays text
class GUITextButton : public Button
{
protected:
	// The text of the button
	std::string m_Text;

	// Background of the button. TEMPORARY
	SolidColorGraphic* m_Background;

public:
	/// <summary>Constructs a button that displays text.</summary>
	/// <param name="text">The text of the button.</param>
	GUITextButton(std::string text);

	/// <summary>Displays the button.</summary>
	void display() const;
};