#include "../../../include/onions/world/furry.h"

using namespace std;

namespace onion
{
	namespace world
	{

		template <>
		struct FurryAnimationVars<FurryAnimationID::STAND>
		{
			static constexpr int index = 0;
			static constexpr int size = 1;
		};

		template <>
		struct FurryAnimationVars<FurryAnimationID::WALK>
		{
			static constexpr int index = 
				FurryAnimationVars<FurryAnimationID::STAND>::index 
				+ FurryAnimationVars<FurryAnimationID::STAND>::size;
			static constexpr int size = 8;
		};

		template <>
		struct FurryAnimationVars<FurryAnimationID::_ALL>
		{
			static constexpr int index = 0;
			static constexpr int size = 
				FurryAnimationVars<FurryAnimationID::STAND>::size
				+ FurryAnimationVars<FurryAnimationID::WALK>::size;
		};



		Textured3DPixelSpriteSheet* FurrySprite::sprite_sheet{ nullptr };
		
		FurrySprite::FurrySprite(string id, bool flip_horizontally) : flip_horizontally(flip_horizontally)
		{
			sprite = sprite_sheet->get_sprite(id);
		}

		int FurrySprite::get_width() const
		{
			return sprite ? sprite->width : 0;
		}

		int FurrySprite::get_height() const
		{
			return sprite ? sprite->height : 0;
		}

		void FurrySprite::display(const Texture* texture, const Palette* palette) const
		{
			if (sprite)
			{
				sprite_sheet->display(sprite, flip_horizontally, texture, palette);
			}
		}


		unordered_map<string, FurryComponent*> FurryComponent::m_Components{};

		FurryComponent::FurryComponent(string id) 
		{
			m_Components.emplace(id, this);
		}

		FurryComponent::~FurryComponent() {}

		FurryComponent* FurryComponent::get(string id)
		{
			auto iter = m_Components.find(id);
			if (iter != m_Components.end())
				return iter->second;
			return nullptr;
		}



		// A component that doesn't change between frames.
		struct FurryStaticComponent : public FurryComponent
		{
			// The sprite as viewed from each direction.
			FurrySprite sprites[(int)FurryDirection::_ALL];


			FurryStaticComponent(string id, string prefix) : FurryComponent(id) 
			{
				sprites[(int)FurryDirection::FRONT] = FurrySprite(prefix + " front");
				sprites[(int)FurryDirection::BACK] = FurrySprite(prefix + " back");
				sprites[(int)FurryDirection::RIGHT] = FurrySprite(prefix + " right");
				sprites[(int)FurryDirection::LEFT] = FurrySprite(prefix + " right", true);
			}

			/// <summary>Retrieves the sprite at a given frame.</summary>
			/// <param name="facing">Which way the humanoid is facing, relative to the camera.</param>
			/// <param name="frame">The frame to retrieve.</param>
			/// <returns>The sprite at the given frame.</returns>
			const FurrySprite* get_frame(FurryDirection facing, int frame) const
			{
				return &sprites[(int)facing];
			}
		};
		
		// A component that changes between frames.
		struct FurryDynamicComponent : public FurryComponent
		{
			// The sprites for each frame of animation and direction.
			FurrySprite sprites[FurryAnimationVars<FurryAnimationID::_ALL>::size * (int)FurryDirection::_ALL];

			template <FurryDirection _Facing, FurryAnimationID _Anim>
			void load_sprites(string prefix)
			{
				static constexpr int base_index = (FurryAnimationVars<FurryAnimationID::_ALL>::size * (int)_Facing) + FurryAnimationVars<_Anim>::index;

				static constexpr int half_size = FurryAnimationVars<_Anim>::size / 2;
				static constexpr int quarter_size = FurryAnimationVars<_Anim>::size / 4;

				for (int k = 0; k < FurryAnimationVars<_Anim>::size; ++k)
				{
					int f = base_index + k;
					sprites[f] = FurrySprite(prefix + to_string(k));

					if (!sprites[f].sprite)
					{
						if (k >= quarter_size && k < half_size)
						{
							sprites[f] = sprites[base_index + half_size - k];
						}
						else if (k >= half_size && k < 3 * quarter_size / 4)
						{
							sprites[f] = sprites[base_index + (k % half_size)];
							sprites[f].flip_horizontally = true;
						}
						else
						{
							int m = quarter_size - (k % quarter_size);
							int f_sym = base_index + quarter_size - m; // The frame being copied if the last half of the animation is the first half but flipped horizontally
							int f_rev = base_index + half_size + m; // The frame being copied if the animation follows the pattern 0-1-2-1-0-5-6-5

							if (sprites[base_index + m].sprite != sprites[f_rev].sprite) // The animation is not symmetric
							{
								sprites[f] = sprites[f_rev];
							}
							else // The animation is symmetric
							{
								sprites[f] = sprites[f_sym];
								sprites[f].flip_horizontally = true;
							}
						}
					}
				}
			}
			
			template <FurryDirection _Facing>
			void load_sprites(string prefix)
			{
				load_sprites<_Facing, FurryAnimationID::STAND>(prefix + " walk");
				load_sprites<_Facing, FurryAnimationID::WALK>(prefix + " walk");
			}


			FurryDynamicComponent(string id, string prefix) : FurryComponent(id) 
			{
				load_sprites<FurryDirection::FRONT>(prefix + " front");
				load_sprites<FurryDirection::BACK>(prefix + " back");
				load_sprites<FurryDirection::RIGHT>(prefix + " right");
				load_sprites<FurryDirection::LEFT>(prefix + " left");

				for (int k = 0; k < FurryAnimationVars<FurryAnimationID::_ALL>::size; ++k)
				{
					int f = (FurryAnimationVars<FurryAnimationID::_ALL>::size * (int)FurryDirection::LEFT) + k;
					if (!sprites[f].sprite)
					{
						sprites[f].sprite = sprites[(FurryAnimationVars<FurryAnimationID::_ALL>::size * (int)FurryDirection::RIGHT) + k].sprite;
						sprites[f].flip_horizontally = true;
					}
				}
			}

			/// <summary>Retrieves the sprite at a given frame.</summary>
			/// <param name="facing">Which way the humanoid is facing, relative to the camera.</param>
			/// <param name="frame">The frame to retrieve.</param>
			/// <returns>The sprite at the given frame.</returns>
			const FurrySprite* get_frame(FurryDirection facing, int frame) const
			{
				return &sprites[frame * (int)facing];
			}
		};




		vector<string> g_BodyOptions;			// Options for the body type (lean or stocky)
		vector<string> g_ArmOptions;			// Options for the arms (paws or wings)
		vector<string> g_BodyMarkingOptions;	// Options for the body markings

		void load_furry_components()
		{
			static const regex texture_test("^texture\\s+(.*)");
			static const regex word_by_word("^(\\S+)\\s+(\\S.*)");
			static smatch match;

			// Load the sprite sheet
			FurrySprite::sprite_sheet = new Textured3DPixelSpriteSheet("sprites/hune.png");

			// Register the components
			LoadFile file("res/img/world/sprites/hune.meta");
			while (file.good())
			{
				StringData line;
				string id = file.load_data(line);

				if (!id.empty())
				{
					string remainder;

					if (regex_match(id, match, texture_test)) // Is a texture
					{
						remainder = match[1].str();

						// Determine what component the texture maps onto
						string component;
						if (regex_match(remainder, match, word_by_word))
						{
							component = match[1].str();
							remainder = match[2].str();
						}

						// Add it to the list of options for that component's textures
						vector<string>* opts = nullptr;

						if (component.compare("body") == 0)		opts = &g_BodyMarkingOptions;

						if (opts)
							opts->push_back(remainder);
					}
					else // Is a sprite
					{
						// Determine what component the sprite is part of
						string component;
						if (regex_match(id, match, word_by_word))
						{
							component = match[1].str();
							remainder = match[2].str();
						}

						// If the sprite represents an option that hasn't been seen before, add it to the list of options
						vector<string>* opts = nullptr; // A vector of options for the given component

						if (component.compare("body") == 0)			opts = &g_BodyOptions;
						else if (component.compare("arm") == 0)		opts = &g_ArmOptions;

						if (opts)
						{
							// Build the unique string for the option using the words in between the component identifier and the direction identifier
							string opt = "";
							while (regex_match(remainder, match, word_by_word))
							{
								string word = match[1].str();
								remainder = match[2].str();

								if (word.compare("front") == 0
									|| word.compare("back") == 0
									|| word.compare("right") == 0)
									break;
								else
									opt += opt.empty() ? word : (" " + word);
							}

							// Check if the option is new
							for (auto iter = opts->begin(); iter != opts->end(); ++iter)
							{
								if (iter->compare(opt) == 0)
								{
									opt = "";
									break;
								}
							}

							if (!opt.empty())
								opts->push_back(opt);
						}
					}
				}
			}

			// Set up all humanoid components
			for (auto iter = g_BodyOptions.begin(); iter != g_BodyOptions.end(); ++iter)
			{
				new FurryDynamicComponent("body " + *iter, "body " + *iter);
			}

			for (auto iter = g_ArmOptions.begin(); iter != g_ArmOptions.end(); ++iter)
			{
				new FurryDynamicComponent("left_arm " + *iter, "arm " + *iter);
				new FurryDynamicComponent("right_arm " + *iter, "arm " + *iter);
			}
		}



		FurryBody::FurryBody() 
			: palette(
				vec4f(1.f, 0.f, 0.f, 1.f),
				vec4f(0.f, 1.f, 0.f, 1.f),
				vec4f(0.f, 0.f, 1.f, 1.f)
			)
		{}

		void FurryBody::display(const vec3f& normal, FurryDirection facing, int frame) const
		{
			const FurrySprite* bspr = body->get_frame(facing, frame);
			//const FurrySprite* lspr = left_arm->get_frame(facing, frame);
			//const FurrySprite* rspr = right_arm->get_frame(facing, frame);

			bspr->display(texture, &palette);
		}


		FurryGraphic3D::FurryGraphic3D(
			string body,
			string body_markings
		) 
		{
			if (body.empty())
				body = g_BodyOptions[0];
			m_Body.body = FurryComponent::get("body " + body);

			if (body_markings.empty())
				body_markings = g_BodyMarkingOptions[0];
			m_Body.texture = FurrySprite::sprite_sheet->get_texture("body " + body_markings);
		}
		
		void FurryGraphic3D::display(const vec3i& normal) const
		{
			// Normalize the normal vector
			vec3f n;
			normal.normalize(n);

			// Set up the transform
			Transform::model.push();
			Transform::model.translate(n.get(0), n.get(1), 0.f);

			// Display the body
			m_Body.display(n, FurryDirection::FRONT, 0);

			// Clean up
			Transform::model.pop();
		}



		FurryGenerator::FurryGenerator(string id, const StringData& params) : ObjectGenerator(id)
		{
			if (!params.get("body", m_BodyOption))
				m_BodyOption = "";
			if (!params.get("body_markings", m_BodyMarkingsOption))
				m_BodyMarkingsOption = "";
		}

		FurryGraphic3D* FurryGenerator::generate_graphic(const StringData& params) const
		{
			// The body type
			std::string body;
			if (!params.get("body", body))
				body = m_BodyOption;

			// The body markings
			std::string body_markings;
			if (!params.get("body_markings", body_markings))
				body_markings = m_BodyMarkingsOption;

			// Generate the graphic
			return new FurryGraphic3D(
				body,
				body_markings
			);
		}

	}
}