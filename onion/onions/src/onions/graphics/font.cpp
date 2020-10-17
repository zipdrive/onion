#include <regex>
#include "../../../include/onions/graphics/font.h"
#include "../../../include/onions/fileio.h"

using namespace std;

namespace onion
{

	bool Font::is_loaded() const
	{
		return m_IsLoaded;
	}

	int Font::get_line_height() const
	{
		return m_LineHeight;
	}



	SpriteFont::Character::Character(SPRITE_KEY key, int width, int height, int flush_width) : Sprite(key, width, height), flush_width(flush_width) {}

	SpriteFont::SpriteFont()
	{
		m_Displayer = new opengl::_SquareBufferDisplayer();
	}

	SpriteFont::SpriteFont(const char* path) : SpriteFont()
	{
		load(path);
	}

	SpriteFont::~SpriteFont()
	{
		delete m_Displayer;
	}

	void SpriteFont::load(const char* path)
	{
		m_IsLoaded = false;
		m_LineHeight = -1;

		// Construct the path to the image file
		string fpath("fonts/");
		fpath.append(path);

		// Load the image
		opengl::_Image* image = new opengl::_Image(fpath.c_str());

		// Construct the path to the meta file
		fpath = "res/img/" + fpath;
		regex fext_finder("(.*)\\.[^\\.]+"); // Regex to find the path excluding file extension
		fpath = regex_replace(fpath, fext_finder, "$1.meta");

		// Set up the data vector
		VertexBufferData<FLOAT_VEC2, FLOAT_VEC2> data;

		// Load the file
		LoadFile file(fpath);
		while (file.good())
		{
			IntegerData line;
			string id = file.load_data(line);

			line.get("height", m_LineHeight);
			line.get("kerning", m_Kerning);
			line.get("spacing", m_Spacing);

			int left, top, width;
			if (line.get("left", left) && line.get("top", top) && line.get("width", width))
			{
				// Retrieve the flush width
				int flush;
				if (!line.get("flush", flush))
					flush = width;

				// Create a sprite data object
				m_CharacterManager.set(id[0], new Character(data.buffer_size(), width, m_LineHeight, flush));

				// Calculate texcoord numbers
				Float l = (Float)left / image->get_width(); // left texcoord
				Float r = (Float)(left + width) / image->get_width(); // right texcoord
				Float w = (Float)width;

				Float t = (Float)top / image->get_height(); // top texcoord
				Float b = (Float)(top + m_LineHeight) / image->get_height(); // bottom texcoord
				Float h = (Float)m_LineHeight;

				// Calculate the position and UV-coordinates of each corner
				vec2f pos[4], uv[4];
				for (int k = 3; k >= 0; --k)
				{
					pos[k](0) = k % 2 == 0 ? 0.f : w;
					pos[k](1) = k / 2 == 0 ? 0.f : h;

					uv[k](0) = k % 2 == 0 ? l : r;
					uv[k](1) = k / 2 == 0 ? b : t;
				}

				// Insert the vertices into the data buffer
				int index = data.buffer_size();
				data.push(6);

				for (int k = 5; k >= 0; --k)
				{
					int corner = k % 3 == 0 ? 0 : (k % 3 == 1 ? 3 : (k / 3 == 0 ? 1 : 2));
					data.set<0>(index + k, pos[corner]);
					data.set<1>(index + k, uv[corner]);
				}
			}
		}

		// Set the buffer
		m_Displayer->set_buffer(
			// Set an image buffer that sets the values of two vertex attributes, each with length 2
			new ImageBuffer(

				// The array of data
				&data,

				// The shader used by the sprite sheet
				SimplePixelSpriteSheet::get_shader()->get_attribs(),

				// The previously loaded image
				image
			)
		);

		m_IsLoaded = true;
	}

	int SpriteFont::get_character_dx(char prev, char current) const
	{
		if (prev == ' ') return m_Spacing + m_Kerning;

		if (const SpriteFont::Character* c = m_CharacterManager.get(prev))
		{
			if (current == 'a' || current == 'c' || current == 'e' ||
				current == 'g' || current == 'i' || current == 'j' ||
				current == 'm' || current == 'n' || current == 'o' ||
				current == 'p' || current == 'q' || current == 'r' ||
				current == 's' || current == 'u' || current == 'v' ||
				current == 'w' || current == 'x' || current == 'y' ||
				current == 'z' || current == ',' || current == '.' ||
				current == '+' || current == '=' || current == '-' ||
				current == '_')
			{
				return c->flush_width + m_Kerning;
			}

			return c->width + m_Kerning;
		}

		return 0;
	}

	int SpriteFont::get_line_width(std::string line) const
	{
		if (line.size() == 0)
			return 0;

		int width = 0;
		char prev = line.at(0);

		for (int k = 1; k < line.size(); ++k)
		{
			char current = line.at(k);
			width += get_character_dx(prev, current);
			prev = current;
		}

		return width + get_character_dx(prev, '\0');
	}

	void SpriteFont::display_line(std::string line, const Palette* palette) const
	{
		if (line.size() == 0)
			return;

		// Display the characters of the string
		Transform::model.push();
		char prev = line.at(0);

		for (int k = 1; k <= line.size(); ++k)
		{
			if (const SpriteFont::Character* c = m_CharacterManager.get(prev))
			{
				// Activate the shader
				SimplePixelSpriteSheet::get_shader()->activate(Transform::model.get(), palette->get_red_palette_matrix(), 0);

				// Display the buffer
				m_Displayer->display(c->key);
			}

			char current = (k == line.size() ? '\0' : line.at(k));
			int dx = get_character_dx(prev, current);
			prev = current;
			Transform::model.translate(dx, 0.f, -0.001f);
		}

		Transform::model.pop();
	}



	TextGraphic::TextGraphic(Font* font, Palette* palette, TextHorizontalAlignment horizontal, TextVerticalAlignment vertical, int width, int line_spacing)
	{
		m_Font = font;
		m_Palette = palette;
		m_HorizontalAlignment = horizontal;
		m_VerticalAlignment = vertical;
		m_Width = width;
		m_LineSpacing = line_spacing;
	}
	
	string TextGraphic::get_text() const
	{
		return m_Text;
	}

	void TextGraphic::set_text(string text)
	{
		m_Text = text;
		m_Lines.clear();

		string remainder = text;
		string line;
		int line_width = 0;

		while (remainder.size() > 0)
		{
			size_t next_space = remainder.find_first_of(' ');
			string word = remainder.substr(0, next_space);
			remainder = next_space == string::npos ? "" : remainder.substr(next_space + 1);

			string word_with_leading_space = " " + word;
			int word_width = m_Font->get_line_width(word_with_leading_space);
			if (line_width + word_width > m_Width)
			{
				int xpos;
				switch (m_HorizontalAlignment)
				{
				case TEXT_HORIZONTAL_LEFT:
					xpos = 0;
					break;
				case TEXT_HORIZONTAL_RIGHT:
					xpos = m_Width - line_width;
					break;
				case TEXT_HORIZONTAL_CENTER:
					xpos = (m_Width - line_width) / 2;
					break;
				}

				m_Lines.push_back({ line, xpos });
				line = word;
				line_width = m_Font->get_line_width(word);
			}
			else
			{
				if (line.empty())
				{
					line = word;
					line_width = m_Font->get_line_width(word);
				}
				else
				{
					line += word_with_leading_space;
					line_width += word_width;
				}
			}
		}

		if (!line.empty())
		{
			int xpos;
			switch (m_HorizontalAlignment)
			{
			case TEXT_HORIZONTAL_LEFT:
				xpos = 0;
				break;
			case TEXT_HORIZONTAL_RIGHT:
				xpos = m_Width - line_width;
				break;
			case TEXT_HORIZONTAL_CENTER:
				xpos = (m_Width - line_width) / 2;
				break;
			}

			m_Lines.push_back({ line, xpos });
		}
	}

	int TextGraphic::get_width() const 
	{
		return m_Width;
	}

	void TextGraphic::set_width(int width)
	{
		m_Width = width;
		set_text(m_Text);
	}

	int TextGraphic::get_height() const
	{
		int h = (m_Lines.size() * (m_Font->get_line_height() + m_LineSpacing)) - m_LineSpacing;
		return h;
	}

	void TextGraphic::display() const
	{
		if (!m_Lines.empty())
		{
			Transform::model.push();
			if (m_VerticalAlignment == TEXT_VERTICAL_TOP)
				Transform::model.translate(0.f, -get_height());
			else if (m_VerticalAlignment == TEXT_VERTICAL_CENTER)
				Transform::model.translate(0.f, -(get_height() / 2));

			auto iter = m_Lines.rbegin();
			while (true)
			{
				Transform::model.push();
				Transform::model.translate(iter->xpos);
				m_Font->display_line(iter->text, m_Palette);
				Transform::model.pop();

				if (++iter == m_Lines.rend())
				{
					break;
				}
				else
				{
					Transform::model.translate(0.f, m_Font->get_line_height() + m_LineSpacing);
				}
			}

			Transform::model.pop();
		}
	}
}