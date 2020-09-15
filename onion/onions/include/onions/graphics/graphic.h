#pragma once
#include "../matrix.h"
#include "sprite.h"

namespace onion
{

	// Something that's displayed on the screen.
	class Graphic
	{
	public:
		/// <summary>Destroys the graphic.</summary>
		virtual ~Graphic() {}

		/// <summary>Retrieves the width of the graphic.</summary>
		/// <returns>The width of the graphic.</returns>
		virtual int get_width() const = 0;

		/// <summary>Retrieves the height of the graphic.</summary>
		/// <returns>The height of the graphic.</returns>
		virtual int get_height() const = 0;

		/// <summary>Draws the graphic to the screen.</summary>
		virtual void display() const = 0;
	};


	// A graphic that displays a solid color rectangle.
	class SolidColorGraphic : public Graphic
	{
	private:
		typedef Shader<const vec4f*> SolidColorShader;
		typedef Buffer<2> SolidColorBuffer;

		// The shader for all solid color graphics.
		static SolidColorShader* m_Shader;

		// Used to display the buffer data.
		opengl::_BufferDisplayer* m_Displayer;

	public:
		// The color of the graphic.
		vec4f color;

		// The width of the graphic.
		int width;

		// The height of the graphic.
		int height;

		/// <summary>Protected so it can't be called from the outside.</summary>
		/// <param name="r">The red value, from 0 to 255.</param>
		/// <param name="g">The green value, from 0 to 255.</param>
		/// <param name="b">The blue value, from 0 to 255.</param>
		/// <param name="a">The alpha value, from 0 to 255.</param>
		/// <param name="width">The width of the graphic.</param>
		/// <param name="height">The height of the graphic.</param>
		SolidColorGraphic(int r, int g, int b, int a, int width, int height);

		/// <summary>Creates a solid color graphic.</summary>
		/// <param name="r">The red value, from 0 to 1.</param>
		/// <param name="g">The green value, from 0 to 1.</param>
		/// <param name="b">The blue value, from 0 to 1.</param>
		/// <param name="a">The alpha value, from 0 to 1.</param>
		/// <param name="width">The width of the graphic.</param>
		/// <param name="height">The height of the graphic.</param>
		SolidColorGraphic(float r, float g, float b, float a, int width, int height);

		/// <summary>Destroys the graphic.</summary>
		~SolidColorGraphic();

		/// <summary>Retrieves the width of the graphic.</summary>
		/// <returns>The width of the graphic.</returns>
		int get_width() const;

		/// <summary>Retrieves the height of the graphic.</summary>
		/// <returns>The height of the graphic.</returns>
		int get_height() const;

		/// <summary>Draws a solid color rectangle to the screen.</summary>
		void display() const;
	};



	template <typename... _Args>
	class SpriteGraphic : public Graphic
	{
	protected:
		// The sprite sheet that the sprite is on.
		SpriteSheet<_Args...>* m_SpriteSheet;

		// Any additional arguments to the display call.
		std::tuple<_Args...> m_Args;

		/// <summary>Retrieves the current sprite.</summary>
		virtual Sprite* get_sprite() const = 0;


		// Used to unpack arguments from the tuple for displaying.
		template <unsigned int N>
		struct SpriteDisplayer
		{
			template <typename... _ArgsUnpacked>
			static void display(const SpriteGraphic<_Args...>* graphic, _ArgsUnpacked... args)
			{
				SpriteDisplayer<N - 1>::display(graphic, std::get<N - 1>(graphic->m_Args), args...);
			}
		};

		// End case for the SpriteDisplayer recursion.
		template <>
		struct SpriteDisplayer<0>
		{
			template <typename... _ArgsUnpacked>
			static void display(const SpriteGraphic<_Args...>* graphic, _ArgsUnpacked... args)
			{
				graphic->m_SpriteSheet->display(graphic->get_sprite(), args...);
			}
		};

	public:
		/// <summary>Constructs a graphic that displays a sprite.</summary>
		/// <param name="sprite_sheet">The sprite sheet that the sprite comes from.</param>
		/// <param name="args">Any additional arguments to the display call.</param>
		SpriteGraphic(SpriteSheet<_Args...>* sprite_sheet, _Args... args) : m_Args(args...)
		{
			m_SpriteSheet = sprite_sheet;
		}

		/// <summary>Retrieves the width of the graphic.</summary>
		int get_width() const
		{
			return get_sprite()->width;
		}

		/// <summary>Retrieves the height of the graphic.</summary>
		int get_height() const
		{
			return get_sprite()->height;
		}

		/// <summary>Displays the sprite.</summary>
		void display() const
		{
			SpriteGraphic<_Args...>::SpriteDisplayer<sizeof...(_Args)>::display(this);
		}
	};

	template <typename... _Args>
	class StaticSpriteGraphic : public SpriteGraphic<_Args...>
	{
	protected:
		// The sprite to display.
		Sprite* m_Sprite;

		/// <summary>Retrieves the current sprite.</summary>
		Sprite* get_sprite() const
		{
			return m_Sprite;
		}

	public:
		/// <summary>Constructs a graphic that displays a sprite.</summary>
		/// <param name="sprite_sheet">The sprite sheet that the sprite comes from.</param>
		/// <param name="args">Any additional arguments to the display call.</param>
		StaticSpriteGraphic(SpriteSheet<_Args...>* sprite_sheet, std::string id, _Args... args) : SpriteGraphic<_Args...>(sprite_sheet, args...)
		{
			m_Sprite = sprite_sheet->get_sprite(id);
		}
	};


	typedef StaticSpriteGraphic<const PALETTE_MATRIX*> SimpleStaticSpriteGraphic;

}