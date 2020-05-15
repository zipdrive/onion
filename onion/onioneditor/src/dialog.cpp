#include "../include/dialog.h"


Dialog::ConfirmButton::ConfirmButton() : GUITextButton("OK") {}

void Dialog::ConfirmButton::click()
{
	Dialog* dialog = pop_dialog();
	dialog->confirm();
}


Dialog::Dialog(int width, int height, std::string message) : m_Message(get_gui_font(), get_gui_font_palette(), message, width - 24)
{
	int message_height = m_Message.get_height();
	int h = height + 48 + (message_height == 0 ? 0 : 8 + m_Message.get_height());

	Application* app = get_application_settings();
	set_bounds((app->width - width) / 2, (app->height - h) / 2, width, h);

	// Yes I am aware this will cause a memory leak, but it's just a placeholder so I don't care
	m_Background = SolidColorGraphic::generate(1.f, 1.f, 0.f, 1.f, width, h);

	m_ConfirmButton.set_bounds((width / 2) - 36, 8, 72, 24);
	m_ConfirmButton.set_parent(this);
}

void Dialog::display_content() const
{
	m_ConfirmButton.display();

	mat_push();
	mat_translate(12.f, get_height() - 12, 0.f);
	m_Message.display();
	mat_pop();
}

void Dialog::display() const
{
	mat_push();
	mat_translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

	SlicedGraphic* pane = get_gui_pane();
	pane->width = get_width();
	pane->height = get_height();
	pane->display();

	m_Background->display();

	display_content();

	mat_pop();
}

void Dialog::freeze()
{
	m_ConfirmButton.freeze();
}

void Dialog::unfreeze()
{
	m_ConfirmButton.unfreeze();
}

void Dialog::confirm()
{
	delete this;
}


CancelableDialog::CancelButton::CancelButton() : GUITextButton("CANCEL") {}

void CancelableDialog::CancelButton::click()
{
	Dialog* dialog = pop_dialog();
	if (dialog)
	{
		if (CancelableDialog* cancelDialog = dynamic_cast<CancelableDialog*>(dialog))
		{
			cancelDialog->cancel();
		}
		else
		{
			delete dialog;
		}
	}
}

CancelableDialog::CancelableDialog(int width, int height, std::string message) : Dialog(width, height, message)
{
	m_ConfirmButton.set_bounds((width / 2) - 84, 8, 72, 24);
	m_CancelButton.set_bounds((width / 2) + 12, 8, 72, 24);

	m_CancelButton.set_parent(this);
}

void CancelableDialog::freeze()
{
	Dialog::freeze();
	m_CancelButton.freeze();
}

void CancelableDialog::unfreeze()
{
	Dialog::unfreeze();
	m_CancelButton.unfreeze();
}

void CancelableDialog::cancel()
{
	delete this;
}

void CancelableDialog::display_content() const
{
	Dialog::display_content();
	m_CancelButton.display();
}



ConfirmationDialog::ConfirmationDialog(std::string message) : CancelableDialog(320, 0, message) {}



TextInputDialog::DialogTextInput::DialogTextInput()
{
	m_Background = SolidColorGraphic::generate(1.f, 0.f, 1.f, 1.f, 1, 1);
}

Font* TextInputDialog::DialogTextInput::get_font() const
{
	return get_gui_font();
}

const Palette* TextInputDialog::DialogTextInput::get_font_palette() const
{
	return get_gui_font_palette();
}

void TextInputDialog::DialogTextInput::display() const
{
	mat_push();
	mat_translate(m_Bounds.get(0, 0), m_Bounds.get(1, 0), 0.f);

	m_Background->width = get_width();
	m_Background->height = get_height();
	m_Background->display();

	mat_pop();

	TextInput::display();
}

TextInputDialog::TextInputDialog(std::string message) : CancelableDialog(420, 32, message)
{
	m_TextInput.set_bounds(36, 40, 348, 24);
	m_TextInput.set_parent(this);
}

void TextInputDialog::freeze()
{
	CancelableDialog::freeze();
	m_TextInput.freeze();
}

void TextInputDialog::unfreeze()
{
	CancelableDialog::unfreeze();
	m_TextInput.unfreeze();
}

void TextInputDialog::display_content() const
{
	CancelableDialog::display_content();
	m_TextInput.display();
}