#include <algorithm>
#include <onion.h>
#include "../include/charactercreator.h"

#include <iostream>

using namespace std;
using namespace onion;

Font* get_label_font()
{
	static Font* font = new SpriteFont("outline11.png");
	return font;
}

Palette* get_label_font_palette()
{
	static Palette* palette = new SinglePalette(vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), vec4i(0, 0, 0, 0));
	return palette;
}


SimplePixelSpriteSheet* get_ui_sprite_sheet()
{
	static SimplePixelSpriteSheet* sprite_sheet = new SimplePixelSpriteSheet("ui/basic.png");
	return sprite_sheet;
}

Palette* get_ui_palette()
{
	static Palette* palette = new SinglePalette(vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), vec4i(0, 0, 0, 0));
	return palette;
}


Sprite* get_bg_diamonds()
{
	static Sprite* sprite = get_ui_sprite_sheet()->get_sprite("bg diamonds");
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
	// The sprite when the button plays the animation.
	Sprite* m_PlayButton;

	// The sprite when the button stops the animation.
	Sprite* m_StopButton;

	bool m_Playing;

	void click()
	{
		if (m_Playing)
		{
			g_HuneCreator->stop_walking();
		}
		else
		{
			g_HuneCreator->start_walking();
		}

		m_Playing = !m_Playing;
	}

	void __display() const
	{
		get_ui_sprite_sheet()->display(m_Playing ? m_StopButton : m_PlayButton, get_ui_palette());
	}

public:
	HunePlayStopButton(Sprite* play_button, Sprite* stop_button)
	{
		m_Playing = true;

		m_PlayButton = play_button;
		m_StopButton = stop_button;
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

	void __set_bounds()
	{
		int width = get_width();

		m_RightButton.set_bounds(width - m_RightButton.get_width(), 0, 0, m_RightButton.get_width(), m_RightButton.get_height(), 0);
		m_Label->set_width(width - m_LeftButton.get_width() - m_RightButton.get_width());
	}

	void __display() const
	{
		m_LeftButton.display();
		m_RightButton.display();

		Transform::model.push();
		Transform::model.translate(m_LeftButton.get_width(), m_Label->get_height() - 2);
		m_Label->display();
		Transform::model.pop();
	}

public:
	HuneCreatorFeature(string label, Graphic* left_arrow, Graphic* right_arrow, HuneCreator::Index* index) :
		m_LeftButton(left_arrow, index, &HuneCreator::Index::decrement),
		m_RightButton(right_arrow, index, &HuneCreator::Index::increment)
	{
		m_LeftButton.set_bounds(0, 0, 0, left_arrow->get_width(), left_arrow->get_height(), 0);
		m_LeftButton.set_parent(this);

		m_RightButton.set_bounds(0, 0, 0, right_arrow->get_width(), right_arrow->get_height(), 0);
		m_RightButton.set_parent(this);

		Font* font = get_label_font();
		m_Label = new TextGraphic(font, get_label_font_palette(), TEXT_HORIZONTAL_CENTER, TEXT_VERTICAL_TOP, 0, 0);
		m_Label->set_text(label);
	}

	void freeze()
	{
		m_LeftButton.freeze();
		m_RightButton.freeze();
	}

	void unfreeze()
	{
		m_LeftButton.unfreeze();
		m_RightButton.unfreeze();
	}
};




class HuneCreatorColumn : public Frame
{
protected:
	vector<HuneCreatorFeature*> m_Features;

	void __set_bounds()
	{
		if (m_Features.size() > 0)
		{
			int width = get_width();
			int height = m_Features[0]->get_height();

			int dy = 0;
			if (m_Features.size() > 1)
				dy = (get_height() - height) / (m_Features.size() - 1);
			if (dy > 40)
				dy = 40;

			int y = (get_height() - height - (dy * (m_Features.size() - 1))) / 2;

			for (auto iter = m_Features.rbegin(); iter != m_Features.rend(); ++iter)
			{
				(*iter)->set_bounds(0, y, 0, width, height, 0);
				y += dy;
			}
		}
	}

	void __display() const
	{
		for (auto iter = m_Features.begin(); iter != m_Features.end(); ++iter)
			(*iter)->display();
	}

public:
	~HuneCreatorColumn()
	{
		for (auto iter = m_Features.begin(); iter != m_Features.end(); ++iter)
			delete *iter;
	}

	void add(HuneCreatorFeature* setting)
	{
		m_Features.push_back(setting);
		setting->set_parent(this);

		__set_bounds();
	}

	void freeze()
	{
		for (auto iter = m_Features.begin(); iter != m_Features.end(); ++iter)
			(*iter)->freeze();
	}

	void unfreeze()
	{
		for (auto iter = m_Features.begin(); iter != m_Features.end(); ++iter)
			(*iter)->unfreeze();
	}
};

class HuneCreatorPage : public Frame
{
protected:
	vector<HuneCreatorColumn*> m_Columns;

	void __set_bounds()
	{
		int padding = 20;

#define MAX_COLUMN_WIDTH 300

		int column_width = (get_width() - (padding * (m_Columns.size() - 1))) / m_Columns.size();
		column_width = column_width > MAX_COLUMN_WIDTH ? MAX_COLUMN_WIDTH : column_width;

		int x = (get_width() - (m_Columns.size() * column_width) - (padding * (m_Columns.size() - 1))) / 2;

		for (auto iter = m_Columns.begin(); iter != m_Columns.end(); ++iter)
		{
			(*iter)->set_bounds(x, 0, 0, column_width, get_height(), 0);
			x += column_width + padding;
		}
	}

	void __display() const
	{
		for (auto iter = m_Columns.begin(); iter != m_Columns.end(); ++iter)
			(*iter)->display();
	}

public:
	string name;

	HuneCreatorPage(string name) : name(name) {}

	~HuneCreatorPage()
	{
		for (auto iter = m_Columns.begin(); iter != m_Columns.end(); ++iter)
			delete *iter;
	}

	void add(int column, HuneCreatorFeature* setting)
	{
		if (column >= 0)
		{
			while (column >= m_Columns.size())
			{
				HuneCreatorColumn* c = new HuneCreatorColumn();
				c->set_parent(this);
				m_Columns.push_back(c);
			}

			m_Columns[column]->add(setting);
		}
	}

	void freeze()
	{
		for (auto iter = m_Columns.begin(); iter != m_Columns.end(); ++iter)
			(*iter)->freeze();
	}

	void unfreeze()
	{
		for (auto iter = m_Columns.begin(); iter != m_Columns.end(); ++iter)
			(*iter)->unfreeze();
	}
};

class HuneCreatorPageContainer : public Frame
{
protected:
	class PageSwapper : public Frame
	{
	protected:
		class PageButton : public Button
		{
		protected:
			Graphic* m_Graphic;

			HuneCreatorPageContainer* m_Pages;

			void (HuneCreatorPageContainer::*m_ClickFunc)();

			void click()
			{
				(m_Pages->*m_ClickFunc)();
			}

			void __display() const
			{
				m_Graphic->display();
			}

		public:
			PageButton(Graphic* graphic, HuneCreatorPageContainer* pages, void (HuneCreatorPageContainer::*click_func)())
			{
				m_Graphic = graphic;
				m_Pages = pages;
				m_ClickFunc = click_func;
			}
		};

		// Button that decrements the feature.
		PageButton m_LeftButton;

		// Button that increments the feature.
		PageButton m_RightButton;

		// The label for the feature.
		TextGraphic* m_Label;

		void __set_bounds()
		{
			int width = get_width();

			m_RightButton.set_bounds(width - m_RightButton.get_width(), 0, 0, m_RightButton.get_width(), m_RightButton.get_height(), 0);
			m_Label->set_width(width - m_LeftButton.get_width() - m_RightButton.get_width());

			if (get_height() != m_Label->get_height())
			{
				set_bounds(m_Bounds.get(0, 0), m_Bounds.get(1, 0), m_Bounds.get(2, 0), width, m_Label->get_height(), get_depth());
			}
		}

		void __display() const
		{
			m_LeftButton.display();
			m_RightButton.display();

			Transform::model.push();
			Transform::model.translate(m_LeftButton.get_width(), m_Label->get_height() - 2, 0.f);
			m_Label->display();
			Transform::model.pop();
		}

	public:
		PageSwapper(HuneCreatorPageContainer* pages, Graphic* left_arrow, Graphic* right_arrow) :
			m_LeftButton(left_arrow, pages, &HuneCreatorPageContainer::decrement),
			m_RightButton(right_arrow, pages, &HuneCreatorPageContainer::increment)
		{
			set_parent(pages);

			m_LeftButton.set_bounds(0, 0, 0, left_arrow->get_width(), left_arrow->get_height(), 0);
			m_LeftButton.set_parent(this);
			m_LeftButton.unfreeze();

			m_RightButton.set_bounds(0, 0, 0, right_arrow->get_width(), right_arrow->get_height(), 0);
			m_RightButton.set_parent(this);
			m_RightButton.unfreeze();

			Font* font = get_label_font();
			m_Label = new TextGraphic(font, get_label_font_palette(), TEXT_HORIZONTAL_CENTER, TEXT_VERTICAL_TOP, 0, 0);
		}

		void set_label(string label)
		{
			m_Label->set_text(label);
		}
	} m_PageSwapper;

	HuneCreator* m_Hune;

	SimpleStaticSpriteGraphic* m_LeftArrow;

	SimpleStaticSpriteGraphic* m_RightArrow;

	vector<HuneCreatorPage*> m_Pages;

	int m_Index;

	void __set_bounds()
	{
		int w = get_width();
		m_PageSwapper.set_bounds(0, 0, 0, w, 0, 0);

		int h = get_height() - m_PageSwapper.get_height();
		m_PageSwapper.set_bounds(0, h, 0, w, m_PageSwapper.get_height(), 0);

		for (auto iter = m_Pages.begin(); iter != m_Pages.end(); ++iter)
			(*iter)->set_bounds(0, 0, 0, w, h, 0);
	}

	void __display() const
	{
		m_PageSwapper.display();

		m_Pages[m_Index]->display();
	}

public:
	HuneCreatorPageContainer(HuneCreator* hune, SimpleStaticSpriteGraphic* left_arrow, SimpleStaticSpriteGraphic* right_arrow) :
		m_PageSwapper(this, left_arrow, right_arrow)
	{
		m_Hune = hune;

		m_LeftArrow = left_arrow;
		m_RightArrow = right_arrow;

		m_Index = 0;
	}

	~HuneCreatorPageContainer()
	{
		for (auto iter = m_Pages.begin(); iter != m_Pages.end(); ++iter)
			delete *iter;
	}

	void increment()
	{
		m_Pages[m_Index]->freeze();
		m_Index = (m_Index + 1) % m_Pages.size();
		m_PageSwapper.set_label(m_Pages[m_Index]->name);
		m_Pages[m_Index]->unfreeze();
	}

	void decrement()
	{
		m_Pages[m_Index]->freeze();
		m_Index = (m_Index + m_Pages.size() - 1) % m_Pages.size();
		m_PageSwapper.set_label(m_Pages[m_Index]->name);
		m_Pages[m_Index]->unfreeze();
	}

	void add(string page)
	{
		HuneCreatorPage* p = new HuneCreatorPage(page);
		p->set_parent(this);
		m_Pages.push_back(p);

		if (m_Pages.size() == 1)
		{
			m_PageSwapper.set_label(page);
			p->unfreeze();
		}
	}

	void add(string page, int column, string label, string feature)
	{
		HuneCreatorFeature* setting = new HuneCreatorFeature(label, m_LeftArrow, m_RightArrow, m_Hune->get_feature(feature));

		for (auto iter = m_Pages.begin(); iter != m_Pages.end(); ++iter)
		{
			if ((*iter)->name.compare(page) == 0)
			{
				(*iter)->add(column, setting);
			}
		}
	}
};



HuneCreatorPageContainer* g_Pages;


void character_creator_setup()
{
	Application* app = get_application_settings();

	// Set up the hune
	g_HuneCreator = new HuneCreator();

	// Set up buttons
	SimplePixelSpriteSheet* ui = get_ui_sprite_sheet();
	SinglePalette* ui_palette = new SinglePalette(vec4i(255, 255, 255, 0), vec4i(0, 0, 0, 0), vec4i(0, 0, 0, 0));

	g_RotateLeftButton = new HuneCreatorButton(
		new SimpleStaticSpriteGraphic(ui, "rotate left", ui_palette->get_red_palette_matrix(), 0), 
		&HuneCreator::rotate_left
	);
	g_RotateLeftButton->set_bounds(71, 149, -1, 18, 15, 0);
	g_RotateLeftButton->unfreeze();

	g_RotateRightButton = new HuneCreatorButton(
		new SimpleStaticSpriteGraphic(ui, "rotate right", ui_palette->get_red_palette_matrix(), 0), 
		&HuneCreator::rotate_right
	);
	g_RotateRightButton->set_bounds(111, 149, -1, 18, 15, 0);
	g_RotateRightButton->unfreeze();

	g_PlayStopButton = new HunePlayStopButton(ui->get_sprite("play"), ui->get_sprite("pause"));
	g_PlayStopButton->set_bounds(95, 150, -1, 11, 12, 0);
	g_PlayStopButton->unfreeze();


	SimpleStaticSpriteGraphic* left_arrow = new SimpleStaticSpriteGraphic(ui, "arrow left", ui_palette->get_red_palette_matrix(), 0);
	SimpleStaticSpriteGraphic* right_arrow = new SimpleStaticSpriteGraphic(ui, "arrow right", ui_palette->get_red_palette_matrix(), 0);

	g_Pages = new HuneCreatorPageContainer(g_HuneCreator, left_arrow, right_arrow);

	g_Pages->add("Body");
	g_Pages->add("Body", 0, "Weight", "body");
	g_Pages->add("Body", 0, "Markings", "body markings");
	g_Pages->add("Body", 0, "Primary Color", "body primary_color");
	g_Pages->add("Body", 0, "Secondary Color", "body secondary_color");
	g_Pages->add("Body", 0, "Tertiary Color", "body tertiary_color");

	g_Pages->add("Head");
	g_Pages->add("Head", 0, "Head", "skull");
	g_Pages->add("Head", 0, "Snout", "snout");
	g_Pages->add("Head", 0, "Top of the Head", "upper_head");
	g_Pages->add("Head", 0, "Sides of the Head", "lower_head");
	g_Pages->add("Head", 0, "Markings", "head markings");

	g_Pages->add("Top");
	g_Pages->add("Top", 0, "Top", "top style");
	g_Pages->add("Top", 0, "Primary Color", "top primary_color");
	g_Pages->add("Top", 0, "Secondary Color", "top secondary_color");

	g_Pages->add("Jacket");
	g_Pages->add("Jacket", 0, "Jacket", "jacket style");
	g_Pages->add("Jacket", 0, "Primary Color", "jacket primary_color");
	g_Pages->add("Jacket", 0, "Secondary Color", "jacket secondary_color");

	g_Pages->add("Bottoms");
	g_Pages->add("Bottoms", 0, "Bottoms", "bottoms style");
	g_Pages->add("Bottoms", 0, "Primary Color", "bottoms primary_color");
	g_Pages->add("Bottoms", 0, "Secondary Color", "bottoms secondary_color");

	g_Pages->add("Shoes");
	g_Pages->add("Shoes", 0, "Shoes", "shoes style");
	g_Pages->add("Shoes", 0, "Primary Color", "shoes primary_color");
	g_Pages->add("Shoes", 0, "Secondary Color", "shoes secondary_color");

	g_Pages->set_bounds(200, 40, -1, app->width - 240, app->height - 80, 0);

	// Run the main function
	main(character_creator_display_func);
}

void character_creator_display_func()
{
	static int dx = 0;
	static int dy = 0;

	static Palette* bg_palette = new SinglePalette(vec4i(219, 152, 152, 255), vec4i(212, 143, 143, 255), vec4i());

	static SimplePixelSpriteSheet* ui = get_ui_sprite_sheet();
	static Sprite* bg = get_bg_diamonds();

	static int last_frame = UpdateEvent::frame;
	static int bg_scroll_speed = UpdateEvent::frames_per_second / bg->width;

	int frame_diff = UpdateEvent::frame - last_frame;

	if (frame_diff > bg_scroll_speed)
	{
		dx = (dx + (frame_diff / bg_scroll_speed)) % bg->width;
		dy = (dy + (frame_diff / bg_scroll_speed)) % bg->height;

		last_frame = UpdateEvent::frame;
	}

	Application* app = get_application_settings();
	Transform::model.reset();
	Transform::model.scale(-1.f);
	Transform::model.translate(-(app->width + dx), -dy, 100.f);
	for (int x = -dx; x < app->width; x += bg->width)
	{
		Transform::model.push();
		for (int y = -dy; y < app->height; y += bg->height)
		{
			ui->display(bg, bg_palette);
			Transform::model.translate(0.f, bg->height);
		}
		Transform::model.pop();
		Transform::model.translate(bg->width);
	}

	Transform::model.reset();
	Transform::model.translate(100.f, 170.f, -100.f);
	g_HuneCreator->display();

	Transform::model.reset();
	g_RotateLeftButton->display();
	g_PlayStopButton->display();
	g_RotateRightButton->display();

	g_Pages->display();
}