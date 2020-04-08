#pragma once
#include "gui.h"


// A dialog pane that receives user input.
class Dialog : public Frame
{
protected:
	class ConfirmButton : public GUITextButton
	{
	protected:
		/// <summary>Triggers when the button is clicked.</summary>
		void click();

	public:
		/// <summary>Constructs a cancel button.</summary>
		ConfirmButton();

	} m_ConfirmButton;

	// Background of the dialog pane. TEMPORARY
	SolidColorGraphic* m_Background;

	/// <summary>Displays the content of the dialog pane.</summary>
	virtual void display_content() const;

public:
	/// <summary>Creates a dialog pane.</summary>
	/// <param name="width">The width of the dialog pane.</param>
	/// <param name="height">The height of the dialog pane.</param>
	Dialog(int width, int height);

	/// <summary>Displays the dialog pane.</summary>
	void display() const;

	/// <summary>Freezes input for the dialog pane.</summary>
	virtual void freeze();

	/// <summary>Unfreezes input for the dialog pane.</summary>
	virtual void unfreeze();

	/// <summary>Confirms the dialog.</summary>
	virtual void confirm();
};


// A dialog pane that can be canceled out of.
class CancelableDialog : public Dialog
{
protected:
	class CancelButton : public GUITextButton
	{
	protected:
		/// <summary>Triggers when the button is clicked.</summary>
		void click();

	public:
		/// <summary>Constructs a cancel button.</summary>
		CancelButton();

	} m_CancelButton;

	/// <summary>Displays the content of the dialog pane.</summary>
	virtual void display_content() const;

public:
	/// <summary>Creates a dialog pane that can be canceled out of.</summary>
	/// <param name="width">The width of the dialog pane.</param>
	/// <param name="height">The height of the dialog pane.</param>
	CancelableDialog(int width, int height);

	/// <summary>Freezes input for the dialog pane.</summary>
	virtual void freeze();

	/// <summary>Unfreezes input for the dialog pane.</summary>
	virtual void unfreeze();

	/// <summary>Cancels the dialog pane.</summary>
	virtual void cancel();
};


// A dialog pane that receives text input.
class TextInputDialog : public CancelableDialog
{
protected:
	class DialogTextInput : public TextInput
	{
	protected:
		// the background TEMPORARY
		SolidColorGraphic* m_Background;

	public:
		DialogTextInput();

		void display() const;

	} m_TextInput;

	virtual void display_content() const;

public:
	TextInputDialog(std::string desc);

	virtual void freeze();

	virtual void unfreeze();
};



/// <summary>Pushes a dialog pane to the screen.</summary>
/// <param name="dialog">The dialog pane to display.</param>
void push_dialog(Dialog* dialog);

/// <summary>Pops the top dialog pane from the screen.</summary>
Dialog* pop_dialog();