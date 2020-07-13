#include <onion.h>
#include "../include/charactercreator.h"

#include <iostream>

using namespace std;


Font* get_label_font()
{
	static Font* font = Font::load_sprite_font("outline11.png");
	return font;
}

Palette* get_label_font_palette()
{
	static Palette* palette = new SinglePalette(vec4i(255, 255, 255, 255), vec4i(0, 0, 0, 255), vec4i(0, 0, 0, 255));
	return palette;
}


SpriteSheet* get_ui_sprite_sheet()
{
	static SpriteSheet* sprite_sheet = SpriteSheet::generate("ui/basic.png");
	return sprite_sheet;
}

Palette* get_ui_palette()
{
	static Palette* palette = new SinglePalette(vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), vec4i(0, 0, 0, 0));
	return palette;
}


Sprite* get_bg_diamonds()
{
	static Sprite* sprite = Sprite::get_sprite("bg diamonds");
	return sprite;
}



HuneCreator* g_HuneCreator = nullptr;


class HuneCreatorButton : public Button
{
protected:
	Graphic* m_Graphic;

	void (HuneCreator::*m_ClickFunc)();

	void click()
	{
		(g_HuneCreator->*m_ClickFunc)();
	}

	void __display() const
	{
		m_Graphic->display();
	}

public:
	HuneCreatorButton(Graphic* graphic, void (HuneCreator::*click_func)()) 
	{
		m_Graphic = graphic;
		m_ClickFunc = click_func;
	}
};

HuneCreatorButton* g_RotateLeftButton;
HuneCreatorButton* g_RotateRightButton;


class HunePlayStopButton : public Button
{
protected:
	DynamicSpriteGraphic* m_Graphic;

	bool m_Playing;

	void click()
	{
		if (m_Playing)
		{
			m_Graphic->set_frame(1);
			g_HuneCreator->stop_walking();
		}
		else
		{
			m_Graphic->set_frame(0);
			g_HuneCreator->start_walking();
		}

		m_Playing = !m_Playing;
	}

	void __display() const
	{
		m_Graphic->display();
	}

public:
	HunePlayStopButton(DynamicSpriteGraphic* graphic)
	{
		m_Playing = true;

		m_Graphic = graphic;
		m_Graphic->set_frame(0);
	}
};

HunePlayStopButton* g_PlayStopButton;



class HuneIndexButton : public Button
{
protected:
	Graphic* m_Graphic;

	HuneCreator::Index* m_Index;

	void (HuneCreator::Index::*m_ClickFunc)();

	void click()
	{
		(m_Index->*m_ClickFunc)();
	}

	void __display() const
	{
		m_Graphic->display();
	}

public:
	HuneIndexButton(Graphic* graphic, HuneCreator::Index* index, void (HuneCreator::Index::*click_func)())
	{
		m_Graphic = graphic;
		m_Index = index;
		m_ClickFunc = click_func;
	}
};

class HuneCreatorFeature : public Frame
{
protected:
	// Button that decrements the feature.
	HuneIndexButton m_LeftButton;

	// Button that increments the feature.
	HuneIndexButton m_RightButton;

	// The label for the feature.
	TextGraphic* m_Label;

	void __display() const
	{
		m_LeftButton.display();
		m_RightButton.display();

		mat_push();
		mat_translate(m_LeftButton.get_width(), m_Label->get_height() - 2, 0.f);
		m_Label->display();
		mat_pop();
	}

public:
	HuneCreatorFeature(string label, StaticSpriteGraphic* left_arrow, StaticSpriteGraphic* right_arrow, HuneCreator::Index* index, int width) :
		m_LeftButton(left_arrow, index, &HuneCreator::Index::decrement),
		m_RightButton(right_arrow, index, &HuneCreator::Index::increment)
	{
		m_LeftButton.set_bounds(0, 0, left_arrow->get_width(), left_arrow->get_height());
		m_LeftButton.set_parent(this);
		m_LeftButton.unfreeze();

		m_RightButton.set_bounds(width - right_arrow->get_width(), 0, right_arrow->get_width(), right_arrow->get_height());
		m_RightButton.set_parent(this);
		m_RightButton.unfreeze();

		Font* font = get_label_font();
		m_Label = new TextGraphic(font, get_label_font_palette(), label, width - left_arrow->get_width() - right_arrow->get_width(), TEXT_CENTER);
	}
};

HuneCreatorFeature* g_HeadFeature;
HuneCreatorFeature* g_SnoutFeature;
HuneCreatorFeature* g_UpperHeadFeature;
HuneCreatorFeature* g_HeadMarkings;

HuneCreatorFeature* g_BodyTypeFeature;
HuneCreatorFeature* g_BodyMarkings;

HuneCreatorFeature* g_JacketStyle;
HuneCreatorFeature* g_TopStyle;
HuneCreatorFeature* g_BottomsStyle;
HuneCreatorFeature* g_ShoesStyle;

HuneCreatorFeature* g_PrimaryBodyColorFeature;
HuneCreatorFeature* g_SecondaryBodyColorFeature;
HuneCreatorFeature* g_TertiaryBodyColorFeature;

HuneCreatorFeature* g_PrimaryJacketColorFeature;
HuneCreatorFeature* g_SecondaryJacketColorFeature;
HuneCreatorFeature* g_PrimaryTopColorFeature;
HuneCreatorFeature* g_SecondaryTopColorFeature;
HuneCreatorFeature* g_PrimaryBottomsColorFeature;
HuneCreatorFeature* g_SecondaryBottomsColorFeature;
HuneCreatorFeature* g_PrimaryShoesColorFeature;
HuneCreatorFeature* g_SecondaryShoesColorFeature;


void character_creator_setup()
{
	Application* app = get_application_settings();

	// Set up the hune
	g_HuneCreator = new HuneCreator();

	// Set up buttons
	SpriteSheet* ui = get_ui_sprite_sheet();
	SinglePalette* ui_palette = new SinglePalette(vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), vec4i(0, 0, 0, 0));

	g_RotateLeftButton = new HuneCreatorButton(
		new StaticSpriteGraphic(ui, Sprite::get_sprite("rotate left"), ui_palette), 
		&HuneCreator::rotate_left
	);
	g_RotateLeftButton->set_bounds(71, 149, 18, 15);
	g_RotateLeftButton->unfreeze();

	g_RotateRightButton = new HuneCreatorButton(
		new StaticSpriteGraphic(ui, Sprite::get_sprite("rotate right"), ui_palette), 
		&HuneCreator::rotate_right
	);
	g_RotateRightButton->set_bounds(111, 149, 18, 15);
	g_RotateRightButton->unfreeze();

	DynamicSpriteGraphic* play_stop = new DynamicSpriteGraphic(ui, ui_palette);
	play_stop->add_frame(Sprite::get_sprite("pause"));
	play_stop->add_frame(Sprite::get_sprite("play"));
	g_PlayStopButton = new HunePlayStopButton(play_stop);
	g_PlayStopButton->set_bounds(95, 150, 11, 12);
	g_PlayStopButton->unfreeze();


	StaticSpriteGraphic* left_arrow = new StaticSpriteGraphic(ui, Sprite::get_sprite("arrow left"), ui_palette);
	StaticSpriteGraphic* right_arrow = new StaticSpriteGraphic(ui, Sprite::get_sprite("arrow right"), ui_palette);

	const int left_column_features = 10; // Number of features in the left column
	const int left_column_width = 160;
	int y = 60;
	int dy = (app->height - (2 * y)) / (left_column_features - 1);

	g_ShoesStyle = new HuneCreatorFeature("Shoes", left_arrow, right_arrow, g_HuneCreator->get_feature("shoes style"), left_column_width);
	g_ShoesStyle->set_bounds(200, y, left_column_width, left_arrow->get_height());

	g_BottomsStyle = new HuneCreatorFeature("Bottoms", left_arrow, right_arrow, g_HuneCreator->get_feature("bottoms style"), left_column_width);
	g_BottomsStyle->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_TopStyle = new HuneCreatorFeature("Top", left_arrow, right_arrow, g_HuneCreator->get_feature("top style"), left_column_width);
	g_TopStyle->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_JacketStyle = new HuneCreatorFeature("Jacket", left_arrow, right_arrow, g_HuneCreator->get_feature("jacket style"), left_column_width);
	g_JacketStyle->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_HeadMarkings = new HuneCreatorFeature("Head Markings", left_arrow, right_arrow, g_HuneCreator->get_feature("head markings"), left_column_width);
	g_HeadMarkings->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_UpperHeadFeature = new HuneCreatorFeature("Upper Head", left_arrow, right_arrow, g_HuneCreator->get_feature("upper_head"), left_column_width);
	g_UpperHeadFeature->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_SnoutFeature = new HuneCreatorFeature("Snout", left_arrow, right_arrow, g_HuneCreator->get_feature("snout"), left_column_width);
	g_SnoutFeature->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_HeadFeature = new HuneCreatorFeature("Head", left_arrow, right_arrow, g_HuneCreator->get_feature("skull"), left_column_width);
	g_HeadFeature->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_BodyMarkings = new HuneCreatorFeature("Body Markings", left_arrow, right_arrow, g_HuneCreator->get_feature("body markings"), left_column_width);
	g_BodyMarkings->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	g_BodyTypeFeature = new HuneCreatorFeature("Body", left_arrow, right_arrow, g_HuneCreator->get_feature("body"), left_column_width);
	g_BodyTypeFeature->set_bounds(200, y += dy, left_column_width, left_arrow->get_height());

	const int right_column_features = 11; // Number of features in the right column
	const int right_column_left = 230 + left_column_width;
	const int right_column_width = 220;
	y = 40;
	dy = (app->height - (2 * y)) / (right_column_features - 1);

	g_SecondaryShoesColorFeature = new HuneCreatorFeature("Secondary Shoe Color", left_arrow, right_arrow, g_HuneCreator->get_feature("shoes secondary_color"), right_column_width);
	g_SecondaryShoesColorFeature->set_bounds(right_column_left, y, right_column_width, left_arrow->get_height());

	g_PrimaryShoesColorFeature = new HuneCreatorFeature("Primary Shoe Color", left_arrow, right_arrow, g_HuneCreator->get_feature("shoes primary_color"), right_column_width);
	g_PrimaryShoesColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_SecondaryBottomsColorFeature = new HuneCreatorFeature("Secondary Bottoms Color", left_arrow, right_arrow, g_HuneCreator->get_feature("bottoms secondary_color"), right_column_width);
	g_SecondaryBottomsColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_PrimaryBottomsColorFeature = new HuneCreatorFeature("Primary Bottoms Color", left_arrow, right_arrow, g_HuneCreator->get_feature("bottoms primary_color"), right_column_width);
	g_PrimaryBottomsColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_SecondaryTopColorFeature = new HuneCreatorFeature("Secondary Top Color", left_arrow, right_arrow, g_HuneCreator->get_feature("top secondary_color"), right_column_width);
	g_SecondaryTopColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_PrimaryTopColorFeature = new HuneCreatorFeature("Primary Top Color", left_arrow, right_arrow, g_HuneCreator->get_feature("top primary_color"), right_column_width);
	g_PrimaryTopColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_SecondaryJacketColorFeature = new HuneCreatorFeature("Secondary Jacket Color", left_arrow, right_arrow, g_HuneCreator->get_feature("jacket secondary_color"), right_column_width);
	g_SecondaryJacketColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_PrimaryJacketColorFeature = new HuneCreatorFeature("Primary Jacket Color", left_arrow, right_arrow, g_HuneCreator->get_feature("jacket primary_color"), right_column_width);
	g_PrimaryJacketColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_TertiaryBodyColorFeature = new HuneCreatorFeature("Tertiary Body Color", left_arrow, right_arrow, g_HuneCreator->get_feature("body tertiary_color"), right_column_width);
	g_TertiaryBodyColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_SecondaryBodyColorFeature = new HuneCreatorFeature("Secondary Body Color", left_arrow, right_arrow, g_HuneCreator->get_feature("body secondary_color"), right_column_width);
	g_SecondaryBodyColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	g_PrimaryBodyColorFeature = new HuneCreatorFeature("Primary Body Color", left_arrow, right_arrow, g_HuneCreator->get_feature("body primary_color"), right_column_width);
	g_PrimaryBodyColorFeature->set_bounds(right_column_left, y += dy, right_column_width, left_arrow->get_height());

	// Run the main function
	onion_main(character_creator_display_func);
}

void character_creator_display_func()
{
	static int dx = 0;
	static int dy = 0;

	static Palette* bg_palette = new SinglePalette(vec4i(219, 152, 152, 255), vec4i(212, 143, 143, 255), vec4i());

	static SpriteSheet* ui = get_ui_sprite_sheet();
	static Sprite* bg = get_bg_diamonds();

	static int last_frame = UpdateEvent::frame;
	static int bg_scroll_speed = UpdateEvent::frames_per_second / bg->width;

	Application* app = get_application_settings();
	mat_push();
	mat_translate(-1.f, -1.f, 0.f);
	mat_scale(2.f / app->width, 2.f / app->height, 1.f);

	int frame_diff = UpdateEvent::frame - last_frame;

	if (frame_diff > bg_scroll_speed)
	{
		dx = (dx + (frame_diff / bg_scroll_speed)) % bg->width;
		dy = (dy + (frame_diff / bg_scroll_speed)) % bg->height;

		last_frame = UpdateEvent::frame;
	}

	mat_push();
	mat_scale(-1.f, 1.f, 1.f);
	mat_translate(-(app->width + dx), -dy, 0.999f);
	for (int x = -dx; x < app->width; x += bg->width)
	{
		mat_push();
		for (int y = -dy; y < app->height; y += bg->height)
		{
			ui->display(bg->key, bg_palette);
			mat_translate(0.f, bg->height, 0.f);
		}
		mat_pop();
		mat_translate(bg->width, 0.f, 0.f);
	}
	mat_pop();

	mat_push();
	mat_translate(100.f, 170.f, -0.5f);
	g_HuneCreator->display();
	mat_pop();

	g_RotateLeftButton->display();
	g_PlayStopButton->display();
	g_RotateRightButton->display();

	g_BodyTypeFeature->display();
	g_BodyMarkings->display();
	g_HeadFeature->display();
	g_SnoutFeature->display();
	g_UpperHeadFeature->display();
	g_HeadMarkings->display();
	g_JacketStyle->display();
	g_TopStyle->display();
	g_BottomsStyle->display();
	g_ShoesStyle->display();

	g_PrimaryBodyColorFeature->display();
	g_SecondaryBodyColorFeature->display();
	g_TertiaryBodyColorFeature->display();
	g_PrimaryJacketColorFeature->display();
	g_SecondaryJacketColorFeature->display();
	g_PrimaryTopColorFeature->display();
	g_SecondaryTopColorFeature->display();
	g_PrimaryBottomsColorFeature->display();
	g_SecondaryBottomsColorFeature->display();
	g_PrimaryShoesColorFeature->display();
	g_SecondaryShoesColorFeature->display();

	mat_pop();
}