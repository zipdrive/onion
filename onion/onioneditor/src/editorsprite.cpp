#include "../include/dialog.h"
#include "../include/editorsprite.h"


void SpriteSheetEditor::display()
{
	// Display the sprite sheet
	mat_push();
	mat_translate(0.f, 0.f, 1.f);
	m_SpriteSheet->display(0, mat4x4f());
	mat_pop();
	
	// Display the scroll bars
	m_HorizontalScrollBar->display();
	m_VerticalScrollBar->display();
}

void SpriteSheetEditor::freeze()
{
	m_HorizontalScrollBar->freeze();
	m_VerticalScrollBar->freeze();
}

void SpriteSheetEditor::unfreeze()
{
	m_HorizontalScrollBar->unfreeze();
	m_VerticalScrollBar->unfreeze();
}



class NewSpriteSheetButton : public GUITextButton
{
private:
	class NewSpriteSheetDialog : public TextInputDialog
	{
	public:
		NewSpriteSheetDialog(std::string desc) : TextInputDialog(desc) {}

		void confirm()
		{
			if (Data::sprite_sheets.empty())
			{
				SpriteSheetsEditor::get_highlighted_sprite_sheet() = m_TextInput.get_input();
			}

			Data::sprite_sheets.emplace(m_TextInput.get_input(), SpriteSheet::generate(m_TextInput.get_input().c_str()));
			Data::save();
		}
	};

protected:
	void click()
	{
		push_dialog(new NewSpriteSheetDialog("TYPE THE PATH TO THE SPRITE SHEET, INCLUDING EXTENSIONS, FROM THE res/img/ FOLDER."));
	}

public:
	NewSpriteSheetButton(int x, int y, int width, int height) : GUITextButton("REGISTER")
	{
		set_bounds(x, y, width, height);
	}
};

class DeleteSpriteSheetButton : public GUITextButton
{
private:
	class DeleteSpriteSheetDialog : public ConfirmationDialog
	{
	public:
		DeleteSpriteSheetDialog(std::string desc) : ConfirmationDialog(desc) {}

		void confirm()
		{
			std::string next;

			auto iter = Data::sprite_sheets.find(SpriteSheetsEditor::get_highlighted_sprite_sheet());
			if (iter != Data::sprite_sheets.begin())
			{
				next = (--iter)->first;
			}
			else if (++iter != Data::sprite_sheets.end())
			{
				next = iter->first;
			}
			else
			{
				next = "";
			}

			Data::sprite_sheets.erase(SpriteSheetsEditor::get_highlighted_sprite_sheet());
			Data::save();

			SpriteSheetsEditor::get_highlighted_sprite_sheet() = next;
		}
	};

protected:
	void click()
	{
		push_dialog(new DeleteSpriteSheetDialog("ARE YOU SURE YOU WANT TO DEREGISTER res/img/" + SpriteSheetsEditor::get_highlighted_sprite_sheet() + "?"));
	}

public:
	DeleteSpriteSheetButton(int x, int y, int width, int height) : GUITextButton("DEREGISTER")
	{
		set_bounds(x, y, width, height);
	}
};

class EditSpriteSheetButton : public GUITextButton
{
protected:
	void click()
	{
		// TODO open sprite sheet editor
	}

public:
	EditSpriteSheetButton(int x, int y, int width, int height) : GUITextButton("EDIT")
	{
		set_bounds(x, y, width, height);
	}
};



std::string SpriteSheetsEditor::SpriteSheetsList::m_Highlighted{ "" };

std::string& SpriteSheetsEditor::get_highlighted_sprite_sheet()
{
	return SpriteSheetsList::m_Highlighted;
}

int SpriteSheetsEditor::SpriteSheetsList::get_height() const
{
	return Data::sprite_sheets.empty() ? 28 : (20 * Data::sprite_sheets.size()) + 8;
}

void SpriteSheetsEditor::SpriteSheetsList::display() const
{
	Font* gui = get_gui_font();
	const mat4x4f& palette = get_gui_font_palette();

	if (Data::sprite_sheets.empty())
	{
		// Display that no sprite sheets have been registered
		mat_push();
		mat_translate(12.f, 8.f, 0.f);
		gui->display_line("NO SPRITE SHEETS HAVE BEEN REGISTERED.", palette);
		mat_pop();
	}
	else
	{
		// Display a list of all sprite sheets
		mat_push();
		mat_translate(12.f, -12.f, 0.f);
		for (auto iter = Data::sprite_sheets.begin(); iter != Data::sprite_sheets.end(); ++iter)
		{
			mat_translate(0.f, 20.f, 0.f);

			if (iter->first == m_Highlighted)
			{
				// TODO
			}

			get_gui_font()->display_line(iter->first, palette);
		}
		mat_pop();
	}
}


SpriteSheetsEditor::SpriteSheetsEditor()
{
	m_NewSpriteSheetButton = new NewSpriteSheetButton(0, 0, 72, 24);
	m_DeleteSpriteSheetButton = new DeleteSpriteSheetButton(72, 0, 72, 24);
	m_EditSpriteSheetButton = new EditSpriteSheetButton(144, 0, 72, 24);
}

void SpriteSheetsEditor::display()
{
	m_NewSpriteSheetButton->display();
	m_EditSpriteSheetButton->display();
	m_DeleteSpriteSheetButton->display();
}

void SpriteSheetsEditor::freeze()
{
	// Freeze options
	m_NewSpriteSheetButton->freeze();
	m_EditSpriteSheetButton->freeze();
	m_DeleteSpriteSheetButton->freeze();
}

void SpriteSheetsEditor::unfreeze()
{
	// Unfreeze options
	m_NewSpriteSheetButton->unfreeze();
	m_EditSpriteSheetButton->unfreeze();
	m_DeleteSpriteSheetButton->unfreeze();
}