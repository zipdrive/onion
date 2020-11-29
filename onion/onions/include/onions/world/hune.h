#pragma once
#include "graphic.h"
#include "agent.h"
#include "../graphics/sprite.h"
#include "../event.h"


namespace onion
{
	namespace world
	{

		// The way that the hune is facing.
		enum HuneDirection
		{
			FACING_FRONT = 0,
			FACING_LEFT = 1,
			FACING_BACK = 2,
			FACING_RIGHT = 3
		};

#define HUNE_NUM_DIRECTIONS			4


		// The possible animations for a hune.
		enum HuneAnimationID
		{
			HUNE_ALL = -1,
			HUNE_STANDING = 0,
			HUNE_WALKING = 1
		};

		template <HuneAnimationID _ID>
		struct HuneAnimationValues
		{
			// The index of the animation.
			static constexpr int index = 0;

			// The size of the animation.
			static constexpr int size = 0;
		};



		// The body parts of a hune.
		enum HuneBodyPart
		{
			RIGHT_EAR,
			LEFT_EAR,
			HEAD,
			RIGHT_ARM,
			LEFT_ARM,
			TORSO,
			RIGHT_LEG,
			LEFT_LEG
		};


		// A texture-mapped sprite that represents part of a hune.
		struct HuneSprite
		{
			// The sprite sheet that sprites are drawn from.
			static onion::ShadedTexturePixelSpriteSheet* sprite_sheet;

			// The sprite for the hune.
			Sprite* sprite;

			// Whether the sprite needs to be flipped horizontally or not.
			bool flip_horizontally;

			/// <summary>Retrieves the width of the sprite.</summary>
			int get_width() const;

			/// <summary>Retrieves the height of the sprite.</summary>
			int get_height() const;

			/// <summary>Displays the sprite.</summary>
			/// <param name="texture">The texture to map onto the sprite.</param>
			/// <param name="palette">The palette for the sprite.</param>
			void display(const Texture* texture, const Palette* palette) const;
		};

		// A collection of hune sprites.
		class HuneShading
		{
		protected:
			// A collection of all shading sprites.
			static std::unordered_map<std::string, HuneShading*> m_Shading;

			// Protected so it can't be called from outside.
			HuneShading();

		public:
			// Virtual deconstructor.
			virtual ~HuneShading();

			/// <summary>Retrieves a shading collection with a given ID.</summary>
			/// <param name="id">The ID of the shading collection.</param>
			/// <returns>The shading collection with that ID.</returns>
			static HuneShading* get_shading(std::string id);

			/// <summary>Generates a shading collection for the given ID/prefix and body part.</summary>
			/// <param name="id">The ID for the shading.</param>
			/// <param name="body_part">The body part of the shading.</param>
			/// <param name="prefix">The prefix of the IDs used to retrieve sprites for the shading.</param>
			static void set_shading(std::string id, HuneBodyPart body_part, std::string prefix);

			/// <summary>Retrieves the sprite at a given frame.</summary>
			/// <param name="facing">Which way the hune is facing, relative to the camera.</param>
			/// <param name="frame">The frame to retrieve.</param>
			/// <returns>The sprite at the given frame.</returns>
			virtual const HuneSprite* get_frame(HuneDirection facing, int frame) const = 0;
		};





		struct HuneLowerBody
		{
			// The shading for the left leg.
			HuneShading* left_leg = nullptr;

			// The shading for the right leg.
			HuneShading* right_leg = nullptr;

			// The texture for the lower body.
			Texture* texture = nullptr;

			// The palette for the lower body.
			Palette* palette = nullptr;

			/// <summary>Displays the lower body.</summary>
			/// <param name="facing">The direction that the hune is facing.</param>
			/// <param name="frame">The frame to display.</param>
			/// <returns>The height of the lower body.</returns>
			int display(HuneDirection facing, int frame) const;
		};

		struct HuneUpperBody
		{
			// The shading for the torso.
			HuneShading* torso = nullptr;

			// The shading for the left arm.
			HuneShading* left_arm = nullptr;

			// The shading for the right arm.
			HuneShading* right_arm = nullptr;

			// The texture for the torso on the upper body.
			Texture* torso_texture = nullptr;

			// The texture for the arms on the upper body.
			Texture* arm_texture = nullptr;

			// The palette for the whole upper body.
			Palette* palette = nullptr;

			/// <summary>Displays the upper body.</summary>
			/// <param name="facing">The direction that the hune is facing.</param>
			/// <param name="frame">The frame to display.</param>
			/// <returns>The height of the upper body.</returns>
			int display(HuneDirection facing, int frame) const;
		};


		class HuneAnimation
		{
		public:
			// Virtual deconstructor.
			virtual ~HuneAnimation();

			/// <summary>Retrieves the current frame of animation.</summary>
			/// <returns>The current frame of animation.</returns>
			virtual int get_frame() const = 0;
		};

		HuneAnimation* generate_hune_standing_animation();
		HuneAnimation* generate_hune_walking_animation();



		class HuneGraphic : public Graphic3D
		{
		protected:
			// The palette for the whole body.
			MultiplePalette m_BodyPalette;

			// The textures, palettes, and sprites of the upper body.
			HuneUpperBody m_BaseUpperBody;

			// The textures, palettes, and sprites of the lower body.
			HuneLowerBody m_BaseLowerBody;

			// The body type of the hune.
			std::string m_BodyType;



			// The palette for the jacket.
			MultiplePalette m_JacketPalette;

			// The textures, palettes, and sprites of the jacket.
			HuneUpperBody m_Jacket;

			// The palette for the top.
			MultiplePalette m_TopPalette;

			// The textures, palettes, and sprites of the top (shirt/sweater/etc).
			HuneUpperBody m_Top;

			// The palette for the bottoms.
			MultiplePalette m_BottomPalette;

			// The textures, palettes, and sprites of the bottoms (pants/shorts/skirt/etc).
			HuneLowerBody m_Bottom;

			// The palette for the shoes.
			MultiplePalette m_ShoesPalette;

			// The textures, palettes, and sprites of the shoes.
			HuneLowerBody m_Shoes;



			// The base shading for the head.
			HuneShading* m_Skull;

			// The base shading for the snout.
			HuneShading* m_Snout;

			// The base shading for the right upper head.
			HuneShading* m_RightUpperHead;

			// The base shading for the left upper head.
			HuneShading* m_LeftUpperHead;

			// The base shading for the right lower head.
			HuneShading* m_RightLowerHead;

			// The base shading for the left lower head.
			HuneShading* m_LeftLowerHead;

			// The texture for the head.
			Texture* m_HeadTexture;


			// The current animation.
			HuneAnimation* m_Animation;

		public:
			// The direction that the hune is facing.
			vec2i direction;

			/// <summary>Constructs a hune graphic.</summary>
			HuneGraphic();

			/// <summary>Sets the body type.</summary>
			/// <param name="body_type">A string identifier for the body type.</param>
			void set_body_type(std::string body_type);

			/// <summary>Sets the arm type.</summary>
			/// <param name="body_type">A string identifier for the arm type.</param>
			void set_arm_type(std::string arm_type);

			/// <summary>Sets the shape of the head.</summary>
			/// <param name="body_type">A string identifier for the head shape.</param>
			void set_head_shape(std::string head_shape);

			/// <summary>Sets the shape of the snout on the head.</summary>
			/// <param name="body_type">A string identifier for the shape of the snout.</param>
			void set_snout_shape(std::string head_shape);

			/// <summary>Sets the type of upper head feature.</summary>
			/// <param name="body_type">A string identifier for the shape of the upper head feature.</param>
			void set_upper_head_feature(std::string upper_head_feature);

			/// <summary>Sets the type of lower head feature.</summary>
			/// <param name="body_type">A string identifier for the shape of the lower head feature.</param>
			void set_lower_head_feature(std::string lower_head_feature);

			/// <summary>Sets the textures for the body.</summary>
			/// <param name="head">The texture ID of the head. Set to "" for no change.</param>
			/// <param name="arms">The texture ID of the arms. Set to "" for no change.</param>
			/// <param name="torso">The texture ID of the torso. Set to "" for no change.</param>
			/// <param name="legs">The texture ID of the legs. Set to "" for no change.</param>
			void set_textures(std::string head, std::string arms, std::string torso, std::string legs);

			/// <summary>Sets the textures for the head.</summary>
			/// <param name="texture">The texture ID for the head.</param>
			void set_head_texture(std::string texture);

			/// <summary>Sets the textures for the body.</summary>
			/// <param name="texture">The texture ID for the body.</param>
			void set_body_texture(std::string texture);

			/// <summary>Sets the primary color for the body. (Mapped to by red in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the secondary color for the body. (Mapped to by green in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the tertiary color for the body. (Mapped to by blue in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_tertiary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the textures for the top.</summary>
			/// <param name="texture">The texture ID for the body.</param>
			void set_top_texture(std::string texture);

			/// <summary>Sets the primary color for the top. (Mapped to by red in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_top_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the secondary color for the top. (Mapped to by green in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_top_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the textures for the jacket.</summary>
			/// <param name="texture">The texture ID for the body.</param>
			void set_jacket_texture(std::string texture);

			/// <summary>Sets the primary color for the jacket. (Mapped to by red in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_jacket_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the secondary color for the jacket. (Mapped to by green in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_jacket_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the textures for the bottoms.</summary>
			/// <param name="texture">The texture ID for the body.</param>
			void set_bottom_texture(std::string texture);

			/// <summary>Sets the primary color for the bottoms. (Mapped to by red in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_bottom_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the secondary color for the bottoms. (Mapped to by green in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_bottom_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the textures for the shoes.</summary>
			/// <param name="texture">The texture ID for the body.</param>
			void set_shoe_texture(std::string texture);

			/// <summary>Sets the primary color for the shoes. (Mapped to by red in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_shoe_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the secondary color for the shoes. (Mapped to by green in the texture.)</summary>
			/// <param name="color">The main color. (Mapped to by red in the shading sprite.)</param>
			/// <param name="highlight">The highlight color. (Mapped to by green in the shading sprite.)</param>
			/// <param name="shading">The shading color. (Mapped to by blue in the shading sprite.)</param>
			void set_shoe_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading);

			/// <summary>Sets the animation of the hune graphic.</summary>
			/// <param name="animation">The animation to play.</param>
			void set_animation(HuneAnimation* animation);

			/// <summary>Retrieves the width of the hune graphic.</summary>
			int get_width() const;

			/// <summary>Retrieves the height of the hune graphic.</summary>
			int get_height() const;

			/// <summary>Displays the hune.</summary>
			/// <param name="normal">A vector pointing towards the camera.</param>
			virtual void display(const vec3i& normal) const;
		};



		class HuneCreator : public HuneGraphic
		{
		public:
			class Index
			{
			protected:
				// The current index value held.
				int m_Index = 0;

				// The hune graphic to change whenever the index changes.
				HuneGraphic* m_HuneGraphic;

				/// <summary>Sets the index value.</summary>
				/// <param name="index">The value to set the index to.</param>
				virtual void set(int index) = 0;

			public:
				/// <summary>Constructs the index.</summary>
				/// <param name="hune">The hune graphic to change whenever the index changes.</param>
				/// <param name="values">The possible values for the index to take.</param>
				Index(HuneGraphic* hune);

				virtual ~Index();

				/// <summary>Increments the index value.</summary>
				virtual void increment() = 0;

				/// <summary>Decrements the index value.</summary>
				virtual void decrement() = 0;
			};

		protected:
			// A mapping from a string ID to a feature that can be altered.
			std::unordered_map<std::string, Index*> m_Features;

		public:
			HuneCreator();

			~HuneCreator();


			/// <summary>Play the animation of the hune walking.</summary>
			void start_walking();

			/// <summary>Stop the animation of the hune walking.</summary>
			void stop_walking();


			/// <summary>Rotate the hune counter-clockwise.</summary>
			void rotate_right();

			/// <summary>Rotate the hune clockwise.</summary>
			void rotate_left();


			/// <summary>Retrieves an alterable feature of the hune graphic.</summary>
			/// <param name="id">The string ID of the feature to retrieve.</param>
			/// <returns>The index of the feature. NULL if the feature does not exist.</returns>
			Index* get_feature(std::string id);
		};




		template <typename T>
		class _HuneActor : public T
		{
		protected:
			// The animation currently being played.
			HuneAnimationID m_CurrentAnimation;

		public:
			template <typename... _Args>
			_HuneActor(_Args... args) : T(args...)
			{
				m_CurrentAnimation = HUNE_STANDING;
			}

			/// <summary>Updates the actor.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			/// <param name="frames_passed">The number of frames since the last update.</param>
			/// <returns>The desired translation of the actor, in subpixel units.</returns>
			virtual vec3i update(const WorldCamera* view, int frames_passed)
			{
				vec3i trans = T::update(view, frames_passed);

				if (HuneGraphic* h = dynamic_cast<HuneGraphic*>(m_Graphic))
				{
					if (trans.get(0) == 0 && trans.get(1) == 0)
					{
						if (m_CurrentAnimation != HUNE_STANDING)
						{						
							h->set_animation(generate_hune_standing_animation());
							m_CurrentAnimation = HUNE_STANDING;
						}
					}
					else
					{
						if (m_CurrentAnimation != HUNE_WALKING)
						{
							h->direction = trans;
							h->set_animation(generate_hune_walking_animation());
							m_CurrentAnimation = HUNE_WALKING;
						}
					}
				}

				return trans;
			}
		};

		typedef _HuneActor<Actor> HuneActor;


	}
}