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