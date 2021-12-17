#include "../../../include/onions/world/anthro.h"

using namespace std;

namespace onion
{
	namespace world
	{

		template <>
		struct AnthroAnimationVars<AnthroAnimationID::STAND>
		{
			static constexpr int index = 0;
			static constexpr int size = 1;
		};

		template <>
		struct AnthroAnimationVars<AnthroAnimationID::WALK>
		{
			static constexpr int index = 
				AnthroAnimationVars<AnthroAnimationID::STAND>::index 
				+ AnthroAnimationVars<AnthroAnimationID::STAND>::size;
			static constexpr int size = 8;
		};

		template <>
		struct AnthroAnimationVars<AnthroAnimationID::_ALL>
		{
			static constexpr int index = 0;
			static constexpr int size = 
				AnthroAnimationVars<AnthroAnimationID::STAND>::size
				+ AnthroAnimationVars<AnthroAnimationID::WALK>::size;
		};



		NormalMapped3DPixelSpriteSheet* AnthroSprite::sprite_sheet{ nullptr };
		
		AnthroSprite::AnthroSprite(string id, bool flip_horizontally) : flip_horizontally(flip_horizontally)
		{
			sprite = sprite_sheet->get_sprite(id);
		}

		int AnthroSprite::get_width() const
		{
			return sprite ? sprite->width : 0;
		}

		int AnthroSprite::get_height() const
		{
			return sprite ? sprite->height : 0;
		}

		void AnthroSprite::display(const Texture* texture, const Palette* palette) const
		{
			if (sprite)
			{
				sprite_sheet->display(sprite, flip_horizontally, texture, palette);
			}
		}


		unordered_map<string, AnthroComponent*> AnthroComponent::m_Components{};

		AnthroComponent::AnthroComponent(string id) 
		{
			m_Components.emplace(id, this);
		}

		AnthroComponent::~AnthroComponent() {}

		AnthroComponent* AnthroComponent::get(string id)
		{
			auto iter = m_Components.find(id);
			if (iter != m_Components.end())
				return iter->second;
			return nullptr;
		}



		// A component that doesn't change between frames.
		struct AnthroStaticComponent : public AnthroComponent
		{
			// The sprite as viewed from each direction.
			AnthroSprite sprites[(int)AnthroDirection::_ALL];


			AnthroStaticComponent(string id, string prefix) : AnthroComponent(id) 
			{
				sprites[(int)AnthroDirection::FRONT] = AnthroSprite(prefix + " front");
				sprites[(int)AnthroDirection::BACK] = AnthroSprite(prefix + " back");
				sprites[(int)AnthroDirection::RIGHT] = AnthroSprite(prefix + " right");
				sprites[(int)AnthroDirection::LEFT] = AnthroSprite(prefix + " right", true);
			}

			/// <summary>Retrieves the sprite at a given frame.</summary>
			/// <param name="facing">Which way the humanoid is facing, relative to the camera.</param>
			/// <param name="frame">The frame to retrieve.</param>
			/// <returns>The sprite at the given frame.</returns>
			const AnthroSprite* get_frame(AnthroDirection facing, int frame) const
			{
				return &sprites[(int)facing];
			}
		};
		
		// A component that changes between frames.
		struct AnthroDynamicComponent : public AnthroComponent
		{
			// The sprites for each frame of animation and direction.
			AnthroSprite sprites[AnthroAnimationVars<AnthroAnimationID::_ALL>::size * (int)AnthroDirection::_ALL];

			template <AnthroDirection _Facing, AnthroAnimationID _Anim>
			void load_sprites(string prefix)
			{
				static constexpr int base_index = (AnthroAnimationVars<AnthroAnimationID::_ALL>::size * (int)_Facing) + AnthroAnimationVars<_Anim>::index;

				static constexpr int half_size = AnthroAnimationVars<_Anim>::size / 2;
				static constexpr int quarter_size = AnthroAnimationVars<_Anim>::size / 4;

				for (int k = 0; k < AnthroAnimationVars<_Anim>::size; ++k)
				{
					int f = base_index + k;
					sprites[f] = AnthroSprite(prefix + to_string(k));

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
			
			template <AnthroDirection _Facing>
			void load_sprites(string prefix)
			{
				load_sprites<_Facing, AnthroAnimationID::STAND>(prefix + " walk");
				load_sprites<_Facing, AnthroAnimationID::WALK>(prefix + " walk");
			}


			AnthroDynamicComponent(string id, string prefix) : AnthroComponent(id) 
			{
				load_sprites<AnthroDirection::FRONT>(prefix + " front");
				load_sprites<AnthroDirection::BACK>(prefix + " back");
				load_sprites<AnthroDirection::RIGHT>(prefix + " right");
				load_sprites<AnthroDirection::LEFT>(prefix + " left");

				for (int k = 0; k < AnthroAnimationVars<AnthroAnimationID::_ALL>::size; ++k)
				{
					int f = (AnthroAnimationVars<AnthroAnimationID::_ALL>::size * (int)AnthroDirection::LEFT) + k;
					if (!sprites[f].sprite)
					{
						sprites[f].sprite = sprites[(AnthroAnimationVars<AnthroAnimationID::_ALL>::size * (int)AnthroDirection::RIGHT) + k].sprite;
						sprites[f].flip_horizontally = true;
					}
				}
			}

			/// <summary>Retrieves the sprite at a given frame.</summary>
			/// <param name="facing">Which way the humanoid is facing, relative to the camera.</param>
			/// <param name="frame">The frame to retrieve.</param>
			/// <returns>The sprite at the given frame.</returns>
			const AnthroSprite* get_frame(AnthroDirection facing, int frame) const
			{
				return &sprites[frame * (int)facing];
			}
		};




		vector<string> g_TorsoOptions;			// Options for the torso (lean or stocky, nude/clothed/hoodie)
		vector<string> g_ArmOptions;			// Options for the arms (paws or wings)
		vector<string> g_LegOptions;			// Options for the legs (nude/pants/skirt)
		vector<string> g_BodyMarkingOptions;	// Options for the body markings

		void load_anthro_components()
		{
			static const regex word_by_word("^(\\S+)\\s+(\\S.*)");
			smatch match;

			// Load the sprite sheet
			AnthroSprite::sprite_sheet = new NormalMapped3DPixelSpriteSheet("sprites/anthro.png");

			// Register the components
			LoadFile file("res/img/world/sprites/anthro.meta");
			while (file.good())
			{
				StringData line;
				string id = file.load_data(line);

				if (!id.empty())
				{
					string remainder;

					if (id.substr(0, 7) == "texture") // Is a texture
					{
						remainder = id.substr(8);

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
					else if (id.substr(6) == "sprite") // Is a sprite
					{
						remainder = id.substr(7);

						// Determine what component the sprite is part of
						string component;
						if (regex_match(remainder, match, word_by_word))
						{
							component = match[1].str();
							remainder = match[2].str();
						}

						// If the sprite represents an option that hasn't been seen before, add it to the list of options
						vector<string>* opts = nullptr; // A vector of options for the given component

						if (component.compare("torso") == 0)		opts = &g_TorsoOptions;
						else if (component.compare("arm") == 0)		opts = &g_ArmOptions;
						else if (component.compare("leg") == 0)		opts = &g_LegOptions;

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
			for (auto iter = g_TorsoOptions.begin(); iter != g_TorsoOptions.end(); ++iter)
			{
				new AnthroDynamicComponent("body " + *iter, "body " + *iter);
			}

			for (auto iter = g_ArmOptions.begin(); iter != g_ArmOptions.end(); ++iter)
			{
				new AnthroDynamicComponent("left_arm " + *iter, "arm " + *iter);
				new AnthroDynamicComponent("right_arm " + *iter, "arm " + *iter);
			}
		}



		AnthroBody::AnthroBody() 
			: palette(
				vec4f(1.f, 0.f, 0.f, 1.f),
				vec4f(0.f, 1.f, 0.f, 1.f),
				vec4f(0.f, 0.f, 1.f, 1.f)
			)
		{}

		void AnthroBody::display(const vec3f& normal, AnthroDirection facing, int frame) const
		{
			const AnthroSprite* bspr = body->get_frame(facing, frame);
			//const AnthroSprite* lspr = left_arm->get_frame(facing, frame);
			//const AnthroSprite* rspr = right_arm->get_frame(facing, frame);

			bspr->display(texture, &palette);
		}


		AnthroGraphic3D::AnthroGraphic3D(
			string body,
			string body_markings
		) 
		{
			if (body.empty())
				body = g_BodyOptions[0];
			m_Body.body = AnthroComponent::get("body " + body);

			if (body_markings.empty())
				body_markings = g_BodyMarkingOptions[0];
			m_Body.texture = nullptr;// AnthroSprite::sprite_sheet->get_texture("body " + body_markings); TODO
		}
		
		void AnthroGraphic3D::display(const vec3i& normal) const
		{
			// Normalize the normal vector
			vec3f n;
			normal.normalize(n);

			// Set up the transform
			Transform::model.push();
			Transform::model.translate(n.get(0), n.get(1), 0.f);

			// Display the body
			m_Body.display(n, AnthroDirection::FRONT, 0);

			// Clean up
			Transform::model.pop();
		}



		AnthroGenerator::AnthroGenerator(string id, const StringData& params) : ObjectGenerator(id)
		{
			if (!params.get("body", m_BodyOption))
				m_BodyOption = "";
			if (!params.get("body_markings", m_BodyMarkingsOption))
				m_BodyMarkingsOption = "";
		}

		AnthroGraphic3D* AnthroGenerator::generate_graphic(const StringData& params) const
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
			return new AnthroGraphic3D(
				body,
				body_markings
			);
		}

	}
}