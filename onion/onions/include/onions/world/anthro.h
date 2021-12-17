#pragma once
#include "graphic.h"
#include "agent.h"

namespace onion
{
	namespace world
	{

		// The direction that a humanoid is facing, relative to the camera view.
		enum class AnthroDirection
		{
			// Facing towards the camera.
			FRONT = 0,

			// Facing right from the camera perspective.
			RIGHT = 1,

			// Facing away from the camera.
			BACK = 2,

			// Facing left from the camera perspective.
			LEFT = 3,


			// Used to retrieve the number of directions.
			_ALL = 4
		};


		// The ID for each of a humanoid's animations.
		enum class AnthroAnimationID
		{
			// Animation for standing idle.
			STAND = 0,

			// Animation for walking.
			WALK = 1,


			// Used to retrieve the size of all animations composited together.
			_ALL = 2
		};

		// The index and size of each animation in the frame array.
		template <AnthroAnimationID _Anim>
		struct AnthroAnimationVars
		{
			// The index of the start of the animation in the frame array.
			static constexpr int index = 0;

			// The size of the animation, in frames.
			static constexpr int size = 0;
		};

		
		// A sprite that represents part of a humanoid.
		struct AnthroSprite
		{
			// The sprite sheet containing the furry sprites and textures.
			static NormalMapped3DPixelSpriteSheet* sprite_sheet;

			// The actual sprite to display.
			Sprite* sprite;

			// True if the sprite should be flipped horizontally, false otherwise.
			bool flip_horizontally;


			// Default constructor.
			AnthroSprite() = default;

			/// <summary>Loads the sprite with the given ID.</summary>
			/// <param name="id">The ID of the sprite.</param>
			/// <param name="flip_horizontally">True if the sprite should be flipped horizontally when displayed, false otherwise.</param>
			AnthroSprite(std::string id, bool flip_horizontally = false);


			/// <summary>Retrieves the width of the sprite.</summary>
			int get_width() const;

			/// <summary>Retrieves the height of the sprite.</summary>
			int get_height() const;

			/// <summary>Displays the sprite.</summary>
			/// <param name="texture">The texture to map onto the sprite.</param>
			/// <param name="palette">The palette for the sprite.</param>
			void display(const Texture* texture, const Palette* palette) const;
		};


		// A component of the humanoid.
		struct AnthroComponent
		{
		protected:
			// A collection of all components.
			static std::unordered_map<std::string, AnthroComponent*> m_Components;

			/// <summary>Protected so it can't be called from outside.</summary>
			/// <param name="id">The ID for the component.</param>
			AnthroComponent(std::string id);

		public:
			// Virtual deconstructor.
			virtual ~AnthroComponent();

			/// <summary>Retrieves a component with a given ID.</summary>
			/// <param name="id">The ID of the humanoid component.</param>
			/// <returns>The component with that ID.</returns>
			static AnthroComponent* get(std::string id);


			/// <summary>Retrieves the sprite at a given frame.</summary>
			/// <param name="facing">Which way the humanoid is facing, relative to the camera.</param>
			/// <param name="frame">The frame to retrieve.</param>
			/// <returns>The sprite at the given frame.</returns>
			virtual const AnthroSprite* get_frame(AnthroDirection facing, int frame) const = 0;
		};

		/// <summary>Loads all components for humanoids.</summary>
		void load_anthro_components();



		// The body of the humanoid.
		struct AnthroBody
		{
			// The sprites for the chest and legs.
			AnthroComponent* body;

			// The sprites for the left arm.
			AnthroComponent* left_arm;

			// The sprites for the right arm.
			AnthroComponent* right_arm;


			// The texture to overlay onto the body sprites.
			Texture* texture;

			// The palette used to color the texture.
			SinglePalette palette;


			AnthroBody();

			/// <summary>Displays the component of the humanoid.</summary>
			/// <param name="normal">The direction facing towards the camera, normalized.</param>
			/// <param name="facing">The direction that the humanoid is facing relative to the camera.</param>
			/// <param name="frame">The frame of animation.</param>
			void display(const vec3f& normal, AnthroDirection facing, int frame) const;
		};



		// A 3D graphic that displays a humanoid sprite.
		class AnthroGraphic3D : public Graphic3D
		{
		protected:
			// The body of the humanoid.
			AnthroBody m_Body;

		public:
			// The direction that the humanoid is facing, in world space.
			vec2i direction;


			/// <summary>Constructs a humanoid sprite.</summary>
			/// <param name="body">The option for the body type. "" for default.</param>
			/// <param name="body_markings">The option for the body markings. "" for default.</param>
			AnthroGraphic3D(
				std::string body,
				std::string body_markings
			);

			/// <summary>Displays the humanoid.</summary>
			/// <param name="normal">A vector pointing towards the camera.</param>
			virtual void display(const vec3i& normal) const;
		};


		template <typename T>
		class _AnthroActor : public T
		{
		protected:
			// The animation currently being played.
			AnthroAnimationID m_CurrentAnimation;

		public:
			template <typename... _Args>
			_AnthroActor(_Args... args) : T(args...)
			{
				m_CurrentAnimation = AnthroAnimationID::STAND;
			}

			/// <summary>Updates the actor.</summary>
			/// <param name="view">The geometry of what is visible.</param>
			/// <param name="frames_passed">The number of frames since the last update.</param>
			/// <returns>The desired translation of the actor, in subpixel units.</returns>
			virtual vec3i update(const WorldCamera* view, int frames_passed)
			{
				vec3i trans = T::update(view, frames_passed);

				if (AnthroGraphic3D* g = dynamic_cast<AnthroGraphic3D*>(m_Graphic))
				{
					if (trans.get(0) == 0 && trans.get(1) == 0)
					{
						if (m_CurrentAnimation != AnthroAnimationID::STAND)
						{
							//g->set_animation(generate_hune_standing_animation());
							m_CurrentAnimation = AnthroAnimationID::STAND;
						}
					}
					else
					{
						if (m_CurrentAnimation != AnthroAnimationID::WALK)
						{
							g->direction = trans;
							//g->set_animation(generate_hune_walking_animation());
							m_CurrentAnimation = AnthroAnimationID::WALK;
						}
					}
				}

				return trans;
			}
		};

		typedef _AnthroActor<Actor> AnthroActor;


		// An object generator for an object that uses a humanoid sprite.
		class AnthroGenerator : public ObjectGenerator
		{
		protected:
			// The body type of the humanoid.
			std::string m_BodyOption;

			// The markings on the body of the humanoid.
			std::string m_BodyMarkingsOption;

		public:
			AnthroGenerator(std::string id, const StringData& params);

			/// <summary>Generates a humanoid sprite for the object to use.</summary>
			/// <param name="params">The parameters for the object.</param>
			AnthroGraphic3D* generate_graphic(const StringData& params) const;
		};
		

	}
}