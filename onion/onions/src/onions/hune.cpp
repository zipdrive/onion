#include <string>
#include <algorithm>
#include "../../include/onions/hune.h"
#include "../../include/onions/application.h"


#include <iostream>

using namespace std;



TextureMapSpriteSheet* HuneSprite::sprite_sheet{ nullptr };

int HuneSprite::get_width() const
{
	return sprite ? sprite->width : 0;
}

int HuneSprite::get_height() const
{
	return sprite ? sprite->height : 0;
}

void HuneSprite::display(const Texture* texture, const Palette* palette) const
{
	if (!sprite) return;

	if (flip_horizontally)
	{
		// Flip the sprite with a transformation
		mat4x4f trans;
		trans.set(0, 0, -1.f);
		trans.set(0, 3, sprite->width);

		// Flip where red and green map to on the texture
		const mat2x4f& tex = texture->transform;
		mat2x4f trans_tex(
			-tex.get(0, 0), -tex.get(0, 1), tex.get(0, 2), tex.get(0, 3),
			-tex.get(1, 0), -tex.get(1, 1), tex.get(1, 2), tex.get(1, 3)
		);

		// Display the textured sprite
		mat_push();
		mat_custom_transform(trans);
		sprite_sheet->display(sprite->key, trans_tex, palette);
		mat_pop();
	}
	else
	{
		// Display the textured sprite
		sprite_sheet->display(sprite->key, texture->transform, palette);
	}
}




// A collection of hune sprites that only change depending on the way they face.
class HuneStaticShading : public HuneShading
{
protected:
	HuneSprite m_Sprites[HUNE_NUM_DIRECTIONS];

public:
	HuneStaticShading(HuneBodyPart body_part, string prefix)
	{
		bool flip_horizontally = body_part == LEFT_EAR;

		m_Sprites[FACING_FRONT].sprite = Sprite::get_sprite(prefix + " front");
		m_Sprites[FACING_FRONT].flip_horizontally = body_part == LEFT_EAR;

		m_Sprites[FACING_BACK].sprite = Sprite::get_sprite(prefix + " back");
		m_Sprites[FACING_BACK].flip_horizontally = body_part == RIGHT_EAR;

		m_Sprites[FACING_RIGHT].sprite = body_part == LEFT_EAR ? nullptr : Sprite::get_sprite(prefix + " right");
		m_Sprites[FACING_RIGHT].flip_horizontally = false;

		m_Sprites[FACING_LEFT].sprite = body_part == RIGHT_EAR ? nullptr : Sprite::get_sprite(prefix + " right");
		m_Sprites[FACING_LEFT].flip_horizontally = true;
	}

	/// <summary>Retrieves the sprite at a given frame.</summary>
	/// <param name="facing">Which way the hune is facing, relative to the camera.</param>
	/// <param name="frame">The frame to retrieve.</param>
	/// <returns>The sprite at the given frame.</returns>
	const HuneSprite* get_frame(HuneDirection facing, int frame) const
	{
		if (frame >= HUNE_SIZE) return nullptr;
		return m_Sprites + facing;
	}
};


// A collection of hune sprites that changes depending on the frame.
class HuneDynamicShading : public HuneShading
{
protected:
	// The collected sprites.
	HuneSprite m_Sprites[HUNE_NUM_DIRECTIONS * HUNE_SIZE];

	/// <summary>Loads a set of sprites that uses the pattern 0-1-2-1, then repeats flipped horizontally.</summary>
	/// <param name="facing">The direction that the sprites face.</param>
	/// <param name="prefix">The prefix to use to retrieve the sprites from their IDs.</param>
	void load_symmetric(HuneDirection facing, std::string prefix)
	{
		set_frame(facing, HUNE_STANDING_INDEX, prefix + "walk0", false);

		int mid_index = HUNE_WALKING_SIZE / 2;
		for (int refl = mid_index; refl >= 0; refl -= mid_index)
		{
			for (int k = mid_index - 1; k >= 0; --k)
				set_frame(facing, HUNE_WALKING_INDEX + refl + k, prefix + "walk" + to_string(k > HUNE_WALKING_SIZE / 4 ? mid_index - k : k), refl > 0);
		}
	}

	/// <summary>Loads a set of sprites that uses the pattern 0-1-2-1-0-3-4-3.</summary>
	/// <param name="facing">The direction that the sprites face.</param>
	/// <param name="prefix">The prefix to use to retrieve the sprites from their IDs.</param>
	/// <param name="flip_horizontally">Whether to flip every sprite horizontally or not.</param>
	/// <param name="half_displacement">Whether to displace the animation by half the animation size.</param>
	void load_semi_asymmetric(HuneDirection facing, std::string prefix, bool flip_horizontally, bool half_displacement)
	{
		set_frame(facing, HUNE_STANDING_INDEX, prefix + "walk0", flip_horizontally);

		if (half_displacement)
		{
			int quarter_size = HUNE_WALKING_SIZE / 4;
			int half_size = HUNE_WALKING_SIZE / 2;

			for (int k = half_size - 1; k > 0; --k)
				set_frame(facing, HUNE_WALKING_INDEX + k, prefix + "walk" + to_string(k > quarter_size ? (3 * quarter_size) - k : k + quarter_size), flip_horizontally);
			for (int k = HUNE_WALKING_SIZE; k >= half_size; --k)
				set_frame(facing, HUNE_WALKING_INDEX + (k % HUNE_WALKING_SIZE), prefix + "walk" + to_string(k > 3 * quarter_size ? HUNE_WALKING_SIZE - k : k - half_size), flip_horizontally);
		}
		else
		{
			int quarter_size = HUNE_WALKING_SIZE / 4;
			int half_size = HUNE_WALKING_SIZE / 2;

			for (int k = half_size; k >= 0; --k)
				set_frame(facing, HUNE_WALKING_INDEX + k, prefix + "walk" + to_string(k > quarter_size ? half_size - k : k), flip_horizontally);
			for (int k = HUNE_WALKING_SIZE - 1; k > half_size; --k)
				set_frame(facing, HUNE_WALKING_INDEX + k, prefix + "walk" + to_string(k > 3 * quarter_size ? HUNE_WALKING_SIZE + quarter_size - k : k - quarter_size), flip_horizontally);
		}
	}

	/// <summary>Loads a set of sprites that uses the pattern 0-1-2-3-4-5-6-7.</summary>
	/// <param name="facing">The direction that the sprites face.</param>
	/// <param name="prefix">The prefix to use to retrieve the sprites from their IDs.</param>
	/// <param name="flip_horizontally">Whether to flip every sprite horizontally or not.</param>
	/// <param name="half_displacement">Whether to displace the animation by half the animation size.</param>
	void load_asymmetric(HuneDirection facing, std::string prefix, bool flip_horizontally, bool half_displacement, int displacement = 0)
	{
		set_frame(facing, HUNE_STANDING_INDEX, prefix + "walk0", flip_horizontally);

		if (half_displacement)
		{
			int mid_index = HUNE_WALKING_SIZE / 2;
			for (int k = HUNE_WALKING_SIZE - 1; k >= 0; --k)
				set_frame(facing, HUNE_WALKING_INDEX + ((k + mid_index) % HUNE_WALKING_SIZE), prefix + "walk" + to_string(k + displacement), flip_horizontally);
		}
		else
		{
			for (int k = HUNE_WALKING_SIZE - 1; k >= 0; --k)
				set_frame(facing, HUNE_WALKING_INDEX + k, prefix + "walk" + to_string(k + displacement), flip_horizontally);
		}
	}


	/// <summary>Sets the sprite at the given frame.</summary>
	/// <param name="facing">Which way the hune is facing, relative to the camera.</param>
	/// <param name="frame">The frame to retrieve.</param>
	/// <param name="sprite_id">The ID of the sprite to display at that frame.</param>
	/// <param name="flip_horizontally">Whether to flip the sprite horizontally or not.</param>
	void set_frame(HuneDirection facing, int frame, std::string sprite_id, bool flip_horizontally)
	{
		set_frame(facing, frame, Sprite::get_sprite(sprite_id), flip_horizontally);
	}

	/// <summary>Sets the sprite at the given frame.</summary>
	/// <param name="facing">Which way the hune is facing, relative to the camera.</param>
	/// <param name="frame">The frame to retrieve.</param>
	/// <param name="sprite_id">The sprite to display at that frame.</param>
	/// <param name="flip_horizontally">Whether to flip the sprite horizontally or not.</param>
	void set_frame(HuneDirection facing, int frame, Sprite* sprite, bool flip_horizontally)
	{
		if (frame >= HUNE_SIZE) return;

		HuneSprite* index = m_Sprites + frame + (HUNE_SIZE * (int)facing);
		index->sprite = sprite;
		index->flip_horizontally = flip_horizontally;
	}

public:
	HuneDynamicShading(HuneBodyPart body_part, std::string prefix)
	{
		switch (body_part)
		{
		case TORSO:
			// Load front sprites
			load_symmetric(FACING_FRONT, prefix + " front torso ");

			// Load back sprites
			load_symmetric(FACING_BACK, prefix + " back torso ");

			// Load left sprites
			load_semi_asymmetric(FACING_LEFT, prefix + " right torso ", true, false);

			// Load right sprites
			load_semi_asymmetric(FACING_RIGHT, prefix + " right torso ", false, false);
			break;
		case RIGHT_LEG:
			// Load front sprites
			load_asymmetric(FACING_FRONT, prefix + " front leg ", false, true);

			// Load back sprites
			load_asymmetric(FACING_BACK, prefix + " back leg ", true, true);

			// Load left sprites
			load_asymmetric(FACING_LEFT, prefix + " right back_leg ", true, false, 1);

			// Load right sprites
			load_asymmetric(FACING_RIGHT, prefix + " right front_leg ", false, false, 1);
			break;
		case LEFT_LEG:
			// Load front sprites
			load_asymmetric(FACING_FRONT, prefix + " front leg ", true, false);

			// Load back sprites
			load_asymmetric(FACING_BACK, prefix + " back leg ", false, false);

			// Load left sprites
			load_asymmetric(FACING_LEFT, prefix + " right front_leg ", true, false, 1);

			// Load right sprites
			load_asymmetric(FACING_RIGHT, prefix + " right back_leg ", false, false, 1);
			break;
		case RIGHT_ARM:
			// Load front sprites
			load_semi_asymmetric(FACING_FRONT, prefix + " front arm ", false, false);

			// Load back sprites
			load_semi_asymmetric(FACING_BACK, prefix + " back arm ", true, false);

			// Load left sprites
			load_semi_asymmetric(FACING_LEFT, prefix + " right back_arm ", true, true);

			// Load right sprites
			load_semi_asymmetric(FACING_RIGHT, prefix + " right front_arm ", false, true);
			break;
		case LEFT_ARM:
			// Load front sprites
			load_semi_asymmetric(FACING_FRONT, prefix + " front arm ", true, true);

			// Load back sprites
			load_semi_asymmetric(FACING_BACK, prefix + " back arm ", false, true);

			// Load left sprites
			load_semi_asymmetric(FACING_LEFT, prefix + " right front_arm ", true, true);

			// Load right sprites
			load_semi_asymmetric(FACING_RIGHT, prefix + " right back_arm ", false, true);
			break;
		}
	}

	/// <summary>Retrieves the sprite at a given frame.</summary>
	/// <param name="facing">Which way the hune is facing, relative to the camera.</param>
	/// <param name="frame">The frame to retrieve.</param>
	/// <returns>The sprite at the given frame.</returns>
	const HuneSprite* get_frame(HuneDirection facing, int frame) const
	{
		if (frame >= HUNE_SIZE) return nullptr;
		return m_Sprites + frame + (HUNE_SIZE * (int)facing);
	}
};



unordered_map<string, HuneShading*> HuneShading::m_Shading{};

HuneShading::HuneShading() {}
HuneShading::~HuneShading() {}

HuneShading* HuneShading::get_shading(string id)
{
	auto iter = m_Shading.find(id);
	if (iter != m_Shading.end())
		return iter->second;
	return nullptr;
}

void HuneShading::set_shading(string id, HuneBodyPart body_part, string prefix)
{
	HuneShading* generated_shading;
	if (body_part == HEAD || body_part == RIGHT_EAR || body_part == LEFT_EAR)
		generated_shading = new HuneStaticShading(body_part, prefix);
	else
		generated_shading = new HuneDynamicShading(body_part, prefix);

	auto iter = m_Shading.find(id);
	if (iter != m_Shading.end())
	{
		m_Shading.erase(iter);
		m_Shading.emplace_hint(iter, id, generated_shading);
	}
	else
	{
		m_Shading.emplace(id, generated_shading);
	}
}




int HuneLowerBody::display(HuneDirection facing, int frame) const
{
	const HuneSprite* left = left_leg->get_frame(facing, frame);
	const HuneSprite* right = right_leg->get_frame(facing, frame);

	int height = max(left->get_height(), right->get_height());

	mat_push();
	switch (facing)
	{
	case FACING_FRONT:
		mat_translate(0.f, -1.f, 0.02f);
		left->display(texture, palette);
		mat_translate(-8.f, 0.f, frame < HUNE_WALKING_INDEX + (HUNE_WALKING_SIZE / 2) ? -0.01f : 0.01f);
		right->display(texture, palette);
		break;
	case FACING_BACK:
		mat_translate(0.f, -1.f, 0.02f);
		right->display(texture, palette);
		mat_translate(-8.f, 0.f, frame < HUNE_WALKING_INDEX + (HUNE_WALKING_SIZE / 2) ? -0.01f : 0.01f);
		left->display(texture, palette);
		break;
	case FACING_LEFT:
		mat_translate(12 - left->get_width(), height - left->get_height() - 1, 0.01f);
		left->display(texture, palette);
		mat_translate(left->get_width() - right->get_width(), left->get_height() - right->get_height(), 0.01f);
		right->display(texture, palette);
		break;
	case FACING_RIGHT:
		mat_translate(-10.f, height - right->get_height() - 1, 0.01f);
		right->display(texture, palette);
		mat_translate(0.f, right->get_height() - left->get_height(), 0.01f);
		left->display(texture, palette);
		break;
	}
	mat_pop();

	return height;
}

int HuneUpperBody::display(HuneDirection facing, int frame) const
{
	const HuneSprite* left = left_arm->get_frame(facing, frame);
	const HuneSprite* right = right_arm->get_frame(facing, frame);
	const HuneSprite* mid = torso->get_frame(facing, frame);

	mat_push();
	switch (facing)
	{
	case FACING_FRONT:
	{
		float left_dz = frame < HUNE_WALKING_INDEX + (HUNE_WALKING_SIZE / 2) ? -0.01f : 0.01f;

		// Display the torso
		mat_translate(-mid->get_width() / 2, 0.f, 0.f);
		mid->display(torso_texture, palette);

		// Display the arms
		mat_push();
		mat_translate(4 - right->get_width(), 22 - right->get_height(), -left_dz);
		right->display(arm_texture, palette);
		mat_pop();
		mat_translate(9.f, 22 - left->get_height(), left_dz);
		left->display(arm_texture, palette);
		break;
	}
	case FACING_BACK:
	{
		float left_dz = frame > HUNE_WALKING_INDEX + (HUNE_WALKING_SIZE / 2) ? -0.01f : 0.01f;

		// Display the torso
		mat_translate(-mid->get_width() / 2, 0.f, 0.f);
		mid->display(torso_texture, palette);

		// Display the arms
		mat_push();
		mat_translate(9.f, 22 - right->get_height(), -left_dz);
		right->display(arm_texture, palette);
		mat_pop();
		mat_translate(4 - left->get_width(), 22 - left->get_height(), left_dz);
		left->display(arm_texture, palette);
		break;
	}
	case FACING_RIGHT:
	{
		// Display the torso
		mat_push();
		mat_translate(-5.f, 0.f, 0.f);
		mid->display(torso_texture, palette);
		mat_pop();

		// Display the arms
		mat_translate(-9.f, 22 - right->get_height(), -0.01f);
		right->display(arm_texture, palette);
		mat_translate(0.f, right->get_height() - left->get_height(), 0.02f);
		left->display(arm_texture, palette);
		break;
	}
	case FACING_LEFT:
	{
		// Display the torso
		mat_push();
		mat_translate(-6.f, 0.f, 0.f);
		mid->display(torso_texture, palette);
		mat_pop();

		// Display the arms
		mat_translate(11 - right->get_width(), 22 - right->get_height(), 0.01f);
		right->display(arm_texture, palette);
		mat_translate(right->get_width() - left->get_width(), right->get_height() - left->get_height(), -0.02f);
		left->display(arm_texture, palette);
		break;
	}
	}
	mat_pop();

	return mid->get_height();
}




HuneAnimation::~HuneAnimation() {}


class HuneBasicAnimation : public HuneAnimation
{
protected:
	// The frame that the animation started on.
	int m_StartingFrame;

	// The base index of the animation.
	int m_BaseIndex;

	// The number of frames of animation.
	int m_Size;

public:
	HuneBasicAnimation(int index, int size)
	{
		m_StartingFrame = UpdateEvent::frame;
		m_BaseIndex = index;
		m_Size = size;
	}

	/// <summary>Retrieves the current frame of animation.</summary>
	/// <returns>The current frame of animation.</returns>
	int get_frame() const
	{
		return m_BaseIndex + (((UpdateEvent::frame - m_StartingFrame) * 5 / UpdateEvent::frames_per_second) % m_Size);
	}
};

HuneAnimation* generate_hune_standing_animation()
{
	return new HuneBasicAnimation(HUNE_STANDING_INDEX, HUNE_STANDING_SIZE);
}

HuneAnimation* generate_hune_walking_animation()
{
	return new HuneBasicAnimation(HUNE_WALKING_INDEX, HUNE_WALKING_SIZE);
}




HuneGraphic::HuneGraphic()
{
	if (!HuneSprite::sprite_sheet)
	{
		HuneSprite::sprite_sheet = TextureMapSpriteSheet::generate("sprites/hune.png");

		vector<string> body_types = { "stocky", "lean" };
		for (auto iter = body_types.begin(); iter != body_types.end(); ++iter)
		{
			string body_type = *iter;
			HuneShading::set_shading(body_type + " left arm", LEFT_ARM, body_type);
			HuneShading::set_shading(body_type + " right arm", RIGHT_ARM, body_type);
			HuneShading::set_shading(body_type + " torso", TORSO, body_type);
			HuneShading::set_shading(body_type + " left leg", LEFT_LEG, body_type);
			HuneShading::set_shading(body_type + " right leg", RIGHT_LEG, body_type);
		}

		vector<string> head_types = { "skull large", "skull furry", "skull thin", "skull flared", "nose short", "nose long", "nostrils short", "nostrils long" };
		for (auto iter = head_types.begin(); iter != head_types.end(); ++iter)
			HuneShading::set_shading(*iter, HEAD, *iter);

		vector<string> ear_types = { "ear peaked", "ear small", "ear round", "ear pointed", "ear tall" };
		for (auto iter = ear_types.begin(); iter != ear_types.end(); ++iter)
		{
			string ear_type = *iter;
			HuneShading::set_shading("left " + ear_type, LEFT_EAR, ear_type);
			HuneShading::set_shading("right " + ear_type, RIGHT_EAR, ear_type);
		}
	}

	m_BaseLowerBody.palette = &m_BodyPalette;
	m_BaseUpperBody.palette = &m_BodyPalette;

	facing = FACING_FRONT;

	m_Animation = generate_hune_walking_animation();
}

void HuneGraphic::set_body_type(string body_type)
{
	m_BaseLowerBody.left_leg = HuneShading::get_shading(body_type + " left leg");
	m_BaseLowerBody.right_leg = HuneShading::get_shading(body_type + " right leg");
	m_BaseUpperBody.torso = HuneShading::get_shading(body_type + " torso");
	m_BaseUpperBody.left_arm = HuneShading::get_shading(body_type + " left arm");
	m_BaseUpperBody.right_arm = HuneShading::get_shading(body_type + " right arm");

	// TODO change textures?
}

void HuneGraphic::set_head_shape(string head_shape)
{
	m_Skull = HuneShading::get_shading("skull " + head_shape);
}

void HuneGraphic::set_snout_shape(string snout_shape)
{
	m_Snout = HuneShading::get_shading(snout_shape);
}

void HuneGraphic::set_ear_shape(string ear_shape)
{
	m_LeftEar = HuneShading::get_shading("left ear " + ear_shape);
	m_RightEar = HuneShading::get_shading("right ear " + ear_shape);
}

void HuneGraphic::set_textures(string head, string arms, string torso, string legs)
{
	if (!head.empty()) m_HeadTexture = Texture::get_texture(head);
	if (!arms.empty()) m_BaseUpperBody.arm_texture = Texture::get_texture(arms);
	if (!torso.empty()) m_BaseUpperBody.torso_texture = Texture::get_texture(torso);
	if (!legs.empty()) m_BaseLowerBody.texture = Texture::get_texture(legs);
}

void HuneGraphic::set_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_BodyPalette.set_red_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_BodyPalette.set_green_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_tertiary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_BodyPalette.set_blue_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_animation(HuneAnimation* animation)
{
	if (m_Animation)
	{
		delete m_Animation;
		m_Animation = nullptr;
	}

	m_Animation = animation;
}

int HuneGraphic::get_width() const
{
	// TODO fix later
	return 0;
}

int HuneGraphic::get_height() const
{
	// TODO fix later
	return 0;
}

void HuneGraphic::display() const
{
	int frame = m_Animation->get_frame();

	mat_push();
	mat_translate(0.f, m_BaseLowerBody.display(facing, frame) - 4, 0.f);
	mat_translate(0.f, m_BaseUpperBody.display(facing, frame) - 2, 0.f);

	const HuneSprite* skull_sprite = m_Skull->get_frame(facing, frame);
	const HuneSprite* snout_sprite = m_Snout->get_frame(facing, frame);
	const HuneSprite* left_ear_sprite = m_LeftEar->get_frame(facing, frame);
	const HuneSprite* right_ear_sprite = m_RightEar->get_frame(facing, frame);

	switch (facing)
	{
	case FACING_FRONT:
	{
		int half_skull_width = skull_sprite->get_width() / 2;
		int half_snout_width = snout_sprite->get_width() / 2;

		// Display skull
		mat_translate(-half_skull_width, 0.f, -0.01f);
		skull_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display snout
		mat_translate(half_skull_width - half_snout_width, 0.f, -0.01f);
		snout_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display right ear
		mat_translate(half_snout_width, 8.f, -0.01f);
		right_ear_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display left ear
		mat_translate(1 - left_ear_sprite->get_width(), 0.f, 0.f);
		left_ear_sprite->display(m_HeadTexture, &m_BodyPalette);
		break;
	}
	case FACING_BACK:
	{
		int half_skull_width = skull_sprite->get_width() / 2;

		// Display skull
		mat_translate(-half_skull_width, 0.f, -0.01f);
		skull_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display left ear
		mat_translate(half_skull_width, 8.f, -0.01f);
		left_ear_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display right ear
		mat_translate(1 - right_ear_sprite->get_width(), 0.f, 0.f);
		right_ear_sprite->display(m_HeadTexture, &m_BodyPalette);
		break;
	}
	case FACING_RIGHT:
		// Display skull
		mat_translate(-5.f, 0.f, -0.01f);
		skull_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display snout
		mat_translate(9.f, 0.f, -0.01f);
		snout_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display right ear
		mat_translate(-3 - right_ear_sprite->get_width(), 8.f, -0.01f);
		right_ear_sprite->display(m_HeadTexture, &m_BodyPalette);
		break;
	case FACING_LEFT:
		// Display skull
		mat_translate(-4.f, 0.f, -0.01f);
		skull_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display left ear
		mat_translate(5.f, 8.f, -0.01f);
		left_ear_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display snout
		mat_translate(-3 - snout_sprite->get_width(), -8.f, -0.01f);
		snout_sprite->display(m_HeadTexture, &m_BodyPalette);
		break;
	}

	mat_pop();
}