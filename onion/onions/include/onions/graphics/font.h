#pragma once
#include "graphic.h"

namespace onion
{

	// Displays a line of text.
	class Font
	{
	protected:
		// True if the font is loaded and ready to use, false otherwise.
		bool m_IsLoaded = false;

		// The height of the line.
		int m_LineHeight;

	public:
		/// <summary>Virtual deconstructor.</summary>
		virtual ~Font() {}

		/// <summary>Checks whether the font has been loaded yet.</summary>
		/// <returns>True if the font has been loaded and is ready to use, false if it hasn't been.</returns>
		bool is_loaded() const;

		/// <summary>Calculates the width of a line of text.</summary>
		/// <param name="text">The line of text.</param>
		/// <returns>The width of the line, as displayed in the font.</returns>
		virtual int get_line_width(std::string line) const = 0;

		/// <summary>Retrieves the height of the font.</summary>
		/// <returns>The height of text, as displayed by the font.</returns>
		int get_line_height() const;

		/// <summary>Displays a line of text.</summary>
		/// <param name="text">The line of text to display.</param>
		/// <param name="palette">The color palette of the text.</param>
		virtual void display_line(std::string line, const Palette* palette) const = 0;
	};

	
	// A font loaded from a sprite sheet.
	class SpriteFont : public Font
	{
	private:
		// Used to display the sprites.
		opengl::_VertexBufferDisplayer* m_Displayer;

		// A font character.
		struct Character : public Sprite
		{
			// The x-distance to draw lowercase letters at, so they are flush with F, J, T.
			int flush_width;

			/// <summary>Constructs sprite information.</summary>
			/// <param name="key">The key of the sprite.</param>
			/// <param name="width">The width of the sprite.</param>
			/// <param name="height">The height of the sprite.</param>
			/// <param name="flush_width">The flush width of the sprite.</param>
			Character(SPRITE_KEY key, int width, int height, int flush_width);
		};

		typedef _Manager<char, Character> CharacterManager;

		// Manages the font characters.
		CharacterManager m_CharacterManager;

		// The separation between individual characters.
		int m_Kerning = 0;

		// The width of spaces between words.
		int m_Spacing = 0;

		/// <summary>Gets the horizontal difference between the previous and current character.</summary>
		/// <param name="prev">The previous character in the string.</param>
		/// <param name="current">The current character in the string.</param>
		int get_character_dx(char prev, char current) const;

	public:
		/// <summary>Creates an empty font.</summary>
		SpriteFont();

		/// <summary>Loads a font from an image and meta file.</summary>
		/// <param name="path">The path to the image file, from the res/img/fonts/ folder.</param>
		SpriteFont(const char* path);

		/// <summary>Destroys the buffer displayer.</summary>
		~SpriteFont();

		/// <summary>Loads a font from an image and meta file.</summary>
		/// <param name="path">The path to the image file, from the res/img/fonts/ folder.</param>
		void load(const char* path);

		/// <summary>Calculates the width of a line of text.</summary>
		/// <param name="text">The line of text.</param>
		/// <returns>The width of the line, as displayed in the font.</returns>
		int get_line_width(std::string line) const;

		/// <summary>Displays a line of text.</summary>
		/// <param name="text">The line of text to display.</param>
		/// <param name="palette">The color palette of the text.</param>
		void display_line(std::string line, const Palette* palette) const;
	};


	
	// The horizontal alignment of text.
	enum TextHorizontalAlignment
	{
		TEXT_HORIZONTAL_LEFT,
		TEXT_HORIZONTAL_RIGHT,
		TEXT_HORIZONTAL_CENTER
	};

	// The vertical alignment of text.
	enum TextVerticalAlignment
	{
		TEXT_VERTICAL_TOP,
		TEXT_VERTICAL_BOTTOM,
		TEXT_VERTICAL_CENTER
	};

	// A graphic that displays text.
	class TextGraphic
	{
	protected:
		// The font used to display the text.
		Font* m_Font;

		// The color palette of the text.
		Palette* m_Palette;


		// The text to display.
		std::string m_Text;

		// The horizontal alignment of the text.
		TextHorizontalAlignment m_HorizontalAlignment;

		// The vertical alignment of the text.
		TextVerticalAlignment m_VerticalAlignment;

		// The width of the text graphic.
		int m_Width;

		// The spacing between each line.
		int m_LineSpacing;


		// A line of text.
		struct Line
		{
			// The text of the line.
			std::string text;

			// The x-position of the line.
			int xpos;
		};

		// The separate lines of text to display.
		std::vector<Line> m_Lines;

	public:
		/// <summary>Constructs a graphic that displays text.</summary>
		/// <param name="font">The font used to display the text.</param>
		/// <param name="palette">The color palette used to display the text.</param>
		/// <param name="horizontal">The horizontal alignment of the text.</param>
		/// <param name="vertical">The vertical alignment of the text.</param>
		/// <param name="width">The maximum width of a single line of text.</param>
		/// <param name="line_spacing">The spacing between each line of text.</param>
		TextGraphic(Font* font, Palette* palette, TextHorizontalAlignment horizontal, TextVerticalAlignment vertical, int width, int line_spacing);

		/// <summary>Retrieves the text displayed by the graphic.</summary>
		/// <returns>The text displayed by the graphic.</returns>
		std::string get_text() const;

		/// <summary>Sets the text displayed by the graphic.</summary>
		/// <param name="text">The text to be displayed by the graphic.</param>
		void set_text(std::string text);

		/// <summary>Retrieves the width of the graphic.</summary>
		/// <returns>The width of the graphic.</returns>
		int get_width() const;

		/// <summary>Sets the width of the graphic.</summary>
		/// <param name="width">The new width of the graphic.</param>
		void set_width(int width);

		/// <summary>Retrieves the height of the graphic.</summary>
		/// <returns>The height of the graphic.</returns>
		int get_height() const;

		/// <summary>Draws a solid color rectangle to the screen.</summary>
		void display() const;
	};



}