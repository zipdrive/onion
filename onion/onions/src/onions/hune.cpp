#include <string>
#include <regex>
#include <algorithm>
#include <set>
#include "../../include/onions/hune.h"
#include "../../include/onions/application.h"
#include "../../include/onions/fileio.h"


#include <iostream>

using namespace std;



vector<string> g_BodyTypes = { "lean", "stocky" };
vector<string> g_TorsoTypes;
vector<string> g_LegTypes;
vector<string> g_ArmTypes;
vector<string> g_SkullTypes;
vector<string> g_SnoutTypes;
vector<string> g_UpperHeadTypes;


vector<string> g_HeadMarkingTextures;
vector<string> g_BodyMarkingTextures;
vector<string> g_TopTextures = { "nude none" };
vector<string> g_JacketTextures = { "nude none" };
vector<string> g_BottomTextures = { "nude none" };
vector<string> g_ShoesTextures = { "nude none" };


bool vector_contains(vector<string>* v, string s)
{
	for (auto iter = v->rbegin(); iter != v->rend(); ++iter)
	{
		if (s.compare(*iter) == 0)
			return true;
	}

	return false;
}



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
			prefix = "torso " + prefix;

			// Load front sprites
			load_symmetric(FACING_FRONT, prefix + " front ");

			// Load back sprites
			load_symmetric(FACING_BACK, prefix + " back ");

			// Load left sprites
			load_semi_asymmetric(FACING_LEFT, prefix + " right ", true, false);

			// Load right sprites
			load_semi_asymmetric(FACING_RIGHT, prefix + " right ", false, false);
			break;
		case RIGHT_LEG:
			prefix = "leg " + prefix;

			// Load front sprites
			load_asymmetric(FACING_FRONT, prefix + " front ", false, true);

			// Load back sprites
			load_asymmetric(FACING_BACK, prefix + " back ", true, true);

			// Load left sprites
			load_asymmetric(FACING_LEFT, prefix + " right_back ", true, false, 1);

			// Load right sprites
			load_asymmetric(FACING_RIGHT, prefix + " right_front ", false, false, 1);
			break;
		case LEFT_LEG:
			prefix = "leg " + prefix;

			// Load front sprites
			load_asymmetric(FACING_FRONT, prefix + " front ", true, false);

			// Load back sprites
			load_asymmetric(FACING_BACK, prefix + " back ", false, false);

			// Load left sprites
			load_asymmetric(FACING_LEFT, prefix + " right_front ", true, false, 1);

			// Load right sprites
			load_asymmetric(FACING_RIGHT, prefix + " right_back ", false, false, 1);
			break;
		case RIGHT_ARM:
			prefix = "arm " + prefix;

			// Load front sprites
			load_semi_asymmetric(FACING_FRONT, prefix + " front ", false, false);

			// Load back sprites
			load_semi_asymmetric(FACING_BACK, prefix + " back ", true, false);

			// Load left sprites
			load_semi_asymmetric(FACING_LEFT, prefix + " right_back ", true, true);

			// Load right sprites
			load_semi_asymmetric(FACING_RIGHT, prefix + " right_front ", false, true);
			break;
		case LEFT_ARM:
			prefix = "arm " + prefix;

			// Load front sprites
			load_semi_asymmetric(FACING_FRONT, prefix + " front ", true, true);

			// Load back sprites
			load_semi_asymmetric(FACING_BACK, prefix + " back ", false, true);

			// Load left sprites
			load_semi_asymmetric(FACING_LEFT, prefix + " right_front ", true, true);

			// Load right sprites
			load_semi_asymmetric(FACING_RIGHT, prefix + " right_back ", false, true);
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
		height -= 1;

		mat_translate(0.f, -1.f, 0.02f);
		left->display(texture, palette);
		mat_translate(-8.f, 0.f, frame < HUNE_WALKING_INDEX + (HUNE_WALKING_SIZE / 2) ? -0.01f : 0.01f);
		right->display(texture, palette);
		break;
	case FACING_BACK:
		height -= 1;

		mat_translate(0.f, -1.f, 0.02f);
		right->display(texture, palette);
		mat_translate(-8.f, 0.f, frame < HUNE_WALKING_INDEX + (HUNE_WALKING_SIZE / 2) ? -0.01f : 0.01f);
		left->display(texture, palette);
		break;
	case FACING_LEFT:
		mat_translate(12 - left->get_width(), 0.f, 0.01f);
		left->display(texture, palette);
		mat_translate(left->get_width() - right->get_width(), 0.f, 0.01f);
		right->display(texture, palette);
		break;
	case FACING_RIGHT:
		mat_translate(-10.f, 0.f, 0.01f);
		right->display(texture, palette);
		mat_translate(0.f, 0.f, 0.01f);
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
		mat_translate(mid->get_width() - 4, 20 - left->get_height(), left_dz);
		left->display(arm_texture, palette);
		mat_pop();
		mat_translate(4 - right->get_width(), 20 - right->get_height(), -left_dz);
		right->display(arm_texture, palette);

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
		mat_translate(mid->get_width() - 4, 20 - right->get_height(), -left_dz);
		right->display(arm_texture, palette);
		mat_pop();
		mat_translate(4 - left->get_width(), 20 - left->get_height(), left_dz);
		left->display(arm_texture, palette);
		break;
	}
	case FACING_RIGHT:
	{
		// Display the torso
		mat_push();
		mat_translate(-6.f, 0.f, 0.f);
		mid->display(torso_texture, palette);
		mat_pop();

		// Display the arms
		mat_translate(-9.f, 20 - right->get_height(), -0.01f);
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
		mat_translate(11 - right->get_width(), 20 - right->get_height(), 0.01f);
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





const vector<vec3i> g_ColorsRGB =
{
	// Reds (Hue 12)
	vec3i(246, 131, 100),
	vec3i(246, 107, 69),
	vec3i(244, 74, 31),
	vec3i(208, 54, 13),
	vec3i(169, 42, 8),

	// Oranges (Hue 24)
	vec3i(246, 159, 100),
	vec3i(246, 140, 69),
	vec3i(243, 111, 24),
	vec3i(208, 91, 13),
	vec3i(169, 72, 8),

	// Golds (Hue 36)
	vec3i(246, 188, 100),
	vec3i(246, 175, 69),
	vec3i(243, 155, 24),
	vec3i(208, 130, 13),
	vec3i(169, 105, 8),

	// Yellows (Hue 48)
	vec3i(246, 217, 100),
	vec3i(246, 211, 69),
	vec3i(243, 199, 24),
	vec3i(208, 169, 13),
	vec3i(169, 137, 8),

	/*
	// Yellows (Hue 48)
	vec3i(),
	vec3i(),
	vec3i(),
	vec3i(),
	vec3i(),

	vec3i(244, 151, 31),
	vec3i(243, 236, 226),
	vec3i(142, 108, 17)
*/
};

vector<vec3i> g_Colors =
{
	// Reds
	vec3i(12, 59, 96),
	vec3i(12, 72, 96),
	vec3i(12, 90, 95),
	vec3i(12, 94, 82),
	vec3i(12, 95, 66),

	// Oranges
	vec3i(24, 59, 96),
	vec3i(24, 72, 96),
	vec3i(24, 90, 95),
	vec3i(24, 94, 82),
	vec3i(24, 95, 66),

	// Golds
	vec3i(36, 59, 96),
	vec3i(36, 72, 96),
	vec3i(36, 90, 95),
	vec3i(36, 94, 82),
	vec3i(36, 95, 66),

	// Yellows
	vec3i(48, 59, 96),
	vec3i(48, 72, 96),
	vec3i(48, 90, 95),
	vec3i(48, 94, 82),
	vec3i(48, 95, 66),

	// Yellow-greens
	vec3i(72, 59, 96),
	vec3i(72, 72, 96),
	vec3i(72, 90, 95),
	vec3i(72, 94, 82),
	vec3i(72, 95, 66),

	// Greens
	vec3i(96, 59, 96),
	vec3i(96, 72, 96),
	vec3i(96, 90, 95),
	vec3i(96, 94, 82),
	vec3i(96, 95, 66),

	// Sea-foam greens
	vec3i(156, 59, 96),
	vec3i(156, 72, 96),
	vec3i(156, 90, 95),
	vec3i(156, 94, 82),
	vec3i(156, 95, 66),

	// Cyans
	vec3i(168, 59, 96),
	vec3i(168, 72, 96),
	vec3i(168, 90, 95),
	vec3i(168, 94, 82),
	vec3i(168, 95, 66),

	// Ceruleans
	vec3i(192, 59, 96),
	vec3i(192, 72, 96),
	vec3i(192, 90, 95),
	vec3i(192, 94, 82),
	vec3i(192, 95, 66),

	// Blues
	vec3i(204, 59, 96),
	vec3i(204, 72, 96),
	vec3i(204, 90, 95),
	vec3i(204, 94, 82),
	vec3i(204, 95, 66),

	// Indigos*
	vec3i(252, 59, 96),
	vec3i(252, 72, 96),
	vec3i(252, 90, 95),

	// Violets*
	vec3i(264, 59, 96),
	vec3i(264, 72, 96),
	vec3i(264, 90, 95),

	// Purples*
	vec3i(278, 59, 96),
	vec3i(278, 72, 96),
	vec3i(278, 90, 95),
	vec3i(278, 94, 82),

	// Pinks
	vec3i(312, 59, 96),
	vec3i(312, 72, 96),
	vec3i(312, 90, 95),
	vec3i(312, 94, 82),
	vec3i(312, 95, 66),

	// Pinkish-reds
	vec3i(336, 59, 96),
	vec3i(336, 72, 96),
	vec3i(336, 90, 95),
	vec3i(336, 94, 82),
	vec3i(336, 95, 66),

	// Neutrals
	vec3i(36, 7, 95)
};


vec4i hsv_to_rgb(vec3i hsv)
{
	int kr = (300 + hsv.get(0)) % 360;
	if (kr > 60)
	{
		if (240 - kr < 60)
			kr = 240 - kr;
		else
			kr = 60;
	}
	if (kr < 0)
		kr = 0;

	int kg = (180 + hsv.get(0)) % 360;
	if (kg > 60)
	{
		if (240 - kg < 60)
			kg = 240 - kg;
		else
			kg = 60;
	}
	if (kg < 0)
		kg = 0;

	int kb = (60 + hsv.get(0)) % 360;
	if (kb > 60)
	{
		if (240 - kb < 60)
			kb = 240 - kb;
		else
			kb = 60;
	}
	if (kb < 0)
		kb = 0;

	vec4i rgb;
	rgb.set(0, 0, 255 * hsv.get(2) * (6000 - (hsv.get(1) * kr)) / 600000);
	rgb.set(1, 0, 255 * hsv.get(2) * (6000 - (hsv.get(1) * kg)) / 600000);
	rgb.set(2, 0, 255 * hsv.get(2) * (6000 - (hsv.get(1) * kb)) / 600000);
	rgb.set(3, 0, 255);

	return rgb;
}



HuneGraphic::HuneGraphic()
{
	if (!HuneSprite::sprite_sheet)
	{
		cout << "Now loading [res/img/sprites/hune.png]...\n";
		HuneSprite::sprite_sheet = TextureMapSpriteSheet::generate("sprites/hune.png");
		cout << "Finished loading [res/img/sprites/hune.png].\n";

		LoadFile meta("res/img/sprites/hune.meta");
		regex word_separator("(\\S+)\\s+(\\S.*)");

		while (meta.good())
		{
			unordered_map<string, int> file_data;
			string words = meta.load_data(file_data);

			smatch wordmatch;
			if (regex_match(words, wordmatch, word_separator))
			{
				string word = wordmatch[1].str();
				words = wordmatch[2].str();

				if (word.compare("texture") == 0)
				{
					if (regex_match(words, wordmatch, word_separator))
					{
						word = wordmatch[1].str();
						words = wordmatch[2].str();

						vector<string>* opts = nullptr;

						if (word.compare("jacket") == 0)																	opts = &g_JacketTextures;
						else if (word.compare("top") == 0)																	opts = &g_TopTextures;
						else if (word.compare("bottom") == 0)																opts = &g_BottomTextures;
						else if (word.compare("shoe") == 0)																	opts = &g_ShoesTextures;
						else if (word.compare("head") == 0)																	opts = &g_HeadMarkingTextures;
						else if (word.compare("torso") == 0 || word.compare("arm") == 0 || word.compare("leg") == 0)		opts = &g_BodyMarkingTextures;

						if (opts)
						{
							while (regex_match(words, wordmatch, word_separator))
							{
								word = wordmatch[1].str();

								if (word.compare("torso") == 0 || word.compare("arm") == 0 || word.compare("leg") == 0)
								{
									words = wordmatch[2].str();
								}
								else
								{
									break;
								}
							}

							if (!vector_contains(opts, words))
							{
								opts->push_back(words);
							}
						}
					}
				}
				else
				{
					vector<string>* opts = nullptr;

					if (word.compare("torso") == 0)				opts = &g_TorsoTypes;
					else if (word.compare("leg") == 0)			opts = &g_LegTypes;
					else if (word.compare("arm") == 0)			opts = &g_ArmTypes;
					else if (word.compare("skull") == 0)		opts = &g_SkullTypes;
					else if (word.compare("snout") == 0)		opts = &g_SnoutTypes;
					else if (word.compare("upper_head") == 0)	opts = &g_UpperHeadTypes;

					if (opts)
					{
						string id;

						while (regex_match(words, wordmatch, word_separator))
						{
							word = wordmatch[1].str();
							words = wordmatch[2].str();

							if (word.compare("front") == 0 || word.compare("back") == 0 || word.compare("right") == 0 || word.compare("right_front") == 0 || word.compare("right_back") == 0)
							{
								break;
							}
							else
							{
								id += (id.empty() ? "" : " ") + word;
							}
						}

						if (!vector_contains(opts, id))
						{
							opts->push_back(id);
						}
					}
				}
			}
		}


		// Construct sprite collections for all types of torsos
		for (auto iter = g_TorsoTypes.begin(); iter != g_TorsoTypes.end(); ++iter)
		{
			HuneShading::set_shading("torso " + *iter, TORSO, *iter);
		}

		// Construct sprite collections for all types of legs
		for (auto iter = g_LegTypes.begin(); iter != g_LegTypes.end(); ++iter)
		{
			HuneShading::set_shading("left leg " + *iter, LEFT_LEG, *iter);
			HuneShading::set_shading("right leg " + *iter, RIGHT_LEG, *iter);
		}

		// Construct sprite collections for all types of arms
		for (auto iter = g_ArmTypes.begin(); iter != g_ArmTypes.end(); ++iter)
		{
			HuneShading::set_shading("left arm " + *iter, LEFT_ARM, *iter);
			HuneShading::set_shading("right arm " + *iter, RIGHT_ARM, *iter);
		}

		// Construct sprite collections for all types of skulls
		for (auto iter = g_SkullTypes.begin(); iter != g_SkullTypes.end(); ++iter)
		{
			string id = "skull " + *iter;
			HuneShading::set_shading(id, HEAD, id);
		}

		// Construct sprite collections for all types of snouts
		for (auto iter = g_SnoutTypes.begin(); iter != g_SnoutTypes.end(); ++iter)
		{
			string id = "snout " + *iter;
			HuneShading::set_shading(id, HEAD, id);
		}

		// Construct sprite collections for all types of upper head features
		for (auto iter = g_UpperHeadTypes.begin(); iter != g_UpperHeadTypes.end(); ++iter)
		{
			string id = "upper_head " + *iter;
			HuneShading::set_shading("left " + id, LEFT_EAR, id);
			HuneShading::set_shading("right " + id, RIGHT_EAR, id);
		}
	}

	m_BaseLowerBody.palette = &m_BodyPalette;
	m_BaseUpperBody.palette = &m_BodyPalette;

	m_Jacket.palette = &m_JacketPalette;
	m_Top.palette = &m_TopPalette;
	m_Bottom.palette = &m_BottomPalette;
	m_Shoes.palette = &m_ShoesPalette;

	facing = FACING_FRONT;

	m_Animation = generate_hune_walking_animation();
}

void HuneGraphic::set_body_type(string body_type)
{
	m_BodyType = body_type;

	m_BaseLowerBody.left_leg = HuneShading::get_shading("left leg " + body_type + " nude");
	m_BaseLowerBody.right_leg = HuneShading::get_shading("right leg " + body_type + " nude");
	m_BaseUpperBody.torso = HuneShading::get_shading("torso " + body_type + " nude");

	// TODO change textures?
}

void HuneGraphic::set_arm_type(string arm_type)
{
	m_BaseUpperBody.left_arm = m_Top.left_arm = m_Jacket.left_arm = HuneShading::get_shading("left arm " + arm_type);
	m_BaseUpperBody.right_arm = m_Top.right_arm = m_Jacket.right_arm = HuneShading::get_shading("right arm " + arm_type);
}

void HuneGraphic::set_head_shape(string head_shape)
{
	m_Skull = HuneShading::get_shading("skull " + head_shape);
}

void HuneGraphic::set_snout_shape(string snout_shape)
{
	m_Snout = HuneShading::get_shading("snout " + snout_shape);
}

void HuneGraphic::set_upper_head_feature(string upper_head_feature)
{
	m_LeftUpperHead = HuneShading::get_shading("left upper_head " + upper_head_feature);
	m_RightUpperHead = HuneShading::get_shading("right upper_head " + upper_head_feature);
}

void HuneGraphic::set_textures(string head, string arms, string torso, string legs)
{
	if (!head.empty()) m_HeadTexture = Texture::get_texture(head);
	if (!arms.empty()) m_BaseUpperBody.arm_texture = Texture::get_texture(arms);
	if (!torso.empty()) m_BaseUpperBody.torso_texture = Texture::get_texture(torso);
	if (!legs.empty()) m_BaseLowerBody.texture = Texture::get_texture(legs);
}

void HuneGraphic::set_head_texture(string texture)
{
	Texture* temp = Texture::get_texture("head " + texture);
	m_HeadTexture = temp ? temp : Texture::get_texture("monochrome");
}

void HuneGraphic::set_body_texture(string texture)
{
	Texture* temp = Texture::get_texture("torso " + texture);
	m_BaseUpperBody.torso_texture = temp ? temp : Texture::get_texture("monochrome");

	temp = Texture::get_texture("arm " + texture);
	m_BaseUpperBody.arm_texture = temp ? temp : Texture::get_texture("monochrome");

	temp = Texture::get_texture("leg " + texture);
	m_BaseLowerBody.texture = temp ? temp : Texture::get_texture("monochrome");
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

void HuneGraphic::set_top_texture(string texture)
{
	regex word_separator("(\\S+)\\s+(\\S.*)");
	smatch match;

	if (regex_match(texture, match, word_separator))
	{
		string type = match[1];
		m_Top.torso = HuneShading::get_shading("torso " + m_BodyType + " " + type);

		Texture* temp = Texture::get_texture("top torso " + texture);
		m_Top.torso_texture = temp ? temp : Texture::get_texture("top torso nude none");

		temp = Texture::get_texture("top arm " + texture);
		m_Top.arm_texture = temp ? temp : Texture::get_texture("top arm nude none");
	}
}

void HuneGraphic::set_top_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_TopPalette.set_red_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_top_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_TopPalette.set_green_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_jacket_texture(string texture)
{
	regex word_separator("(\\S+)\\s+(\\S.*)");
	smatch match;

	if (regex_match(texture, match, word_separator))
	{
		string type = match[1];
		m_Jacket.torso = HuneShading::get_shading("torso " + m_BodyType + " " + type);

		Texture* temp = Texture::get_texture("jacket torso " + texture);
		m_Jacket.torso_texture = temp ? temp : Texture::get_texture("jacket torso nude none");

		temp = Texture::get_texture("jacket arm " + texture);
		m_Jacket.arm_texture = temp ? temp : Texture::get_texture("jacket arm nude none");
	}
}

void HuneGraphic::set_jacket_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_JacketPalette.set_red_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_jacket_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_JacketPalette.set_green_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_bottom_texture(string texture)
{
	regex word_separator("(\\S+)\\s+(\\S.*)");
	smatch match;

	if (regex_match(texture, match, word_separator))
	{
		string type = match[1];
		m_Bottom.left_leg = HuneShading::get_shading("left leg " + m_BodyType + " " + type);
		m_Bottom.right_leg = HuneShading::get_shading("right leg " + m_BodyType + " " + type);

		Texture* temp = Texture::get_texture("bottom leg " + texture);
		m_Bottom.texture = temp ? temp : Texture::get_texture("bottom leg nude none");
	}
}

void HuneGraphic::set_bottom_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_BottomPalette.set_red_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_bottom_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_BottomPalette.set_green_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_shoe_texture(string texture)
{
	regex word_separator("(\\S+)\\s+(\\S.*)");
	smatch match;

	if (regex_match(texture, match, word_separator))
	{
		string type = match[1];
		m_Shoes.left_leg = HuneShading::get_shading("left leg " + m_BodyType + " " + type);
		m_Shoes.right_leg = HuneShading::get_shading("right leg " + m_BodyType + " " + type);

		Texture* temp = Texture::get_texture("shoe leg " + texture);
		m_Shoes.texture = temp ? temp : Texture::get_texture("shoe leg nude none");
	}
}

void HuneGraphic::set_shoe_primary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_ShoesPalette.set_red_palette_matrix(color, highlight, shading);
}

void HuneGraphic::set_shoe_secondary_color(const vec4i& color, const vec4i& highlight, const vec4i& shading)
{
	m_ShoesPalette.set_green_palette_matrix(color, highlight, shading);
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

	int trans = m_BaseLowerBody.display(facing, frame) - 4;
	mat_push();
	mat_translate(0.f, 0.f, -0.001f);
	m_Shoes.display(facing, frame);
	mat_translate(0.f, 0.f, -0.001f);
	m_Bottom.display(facing, frame);
	mat_pop();
	mat_translate(0.f, trans, 0.f);

	trans = m_BaseUpperBody.display(facing, frame) - 2;
	mat_push();
	mat_translate(0.f, 0.f, -0.001f);
	m_Top.display(facing, frame);
	mat_translate(0.f, 0.f, -0.001f);
	m_Jacket.display(facing, frame);
	mat_pop();
	mat_translate(0.f, trans, 0.f);

	const HuneSprite* skull_sprite = m_Skull->get_frame(facing, frame);
	const HuneSprite* snout_sprite = m_Snout->get_frame(facing, frame);
	const HuneSprite* left_upper_head_sprite = m_LeftUpperHead->get_frame(facing, frame);
	const HuneSprite* right_upper_head_sprite = m_RightUpperHead->get_frame(facing, frame);

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
		right_upper_head_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display left ear
		mat_translate(1 - left_upper_head_sprite->get_width(), 0.f, 0.f);
		left_upper_head_sprite->display(m_HeadTexture, &m_BodyPalette);
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
		left_upper_head_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display right ear
		mat_translate(1 - right_upper_head_sprite->get_width(), 0.f, 0.f);
		right_upper_head_sprite->display(m_HeadTexture, &m_BodyPalette);
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
		mat_translate(-3 - right_upper_head_sprite->get_width(), 8.f, -0.01f);
		right_upper_head_sprite->display(m_HeadTexture, &m_BodyPalette);
		break;
	case FACING_LEFT:
		// Display skull
		mat_translate(-4.f, 0.f, -0.01f);
		skull_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display left ear
		mat_translate(5.f, 8.f, -0.01f);
		left_upper_head_sprite->display(m_HeadTexture, &m_BodyPalette);

		// Display snout
		mat_translate(-3 - snout_sprite->get_width(), -8.f, -0.01f);
		snout_sprite->display(m_HeadTexture, &m_BodyPalette);
		break;
	}

	mat_pop();
}



template <typename T>
class VectorIndex : public HuneCreator::Index
{
protected:
	// The possible values for the index.
	std::vector<T>& m_Values;

public:
	/// <summary>Constructs the index.</summary>
	/// <param name="hune">The hune graphic to change whenever the index changes.</param>
	/// <param name="values">The possible values for the index to take.</param>
	VectorIndex(HuneGraphic* hune, std::vector<T>& values) : HuneCreator::Index(hune), m_Values(values) {}

	/// <summary>Increments the index value.</summary>
	void increment()
	{
		set((m_Index + 1) % m_Values.size());
	}

	/// <summary>Decrements the index value.</summary>
	void decrement()
	{
		set((m_Index + m_Values.size() - 1) % m_Values.size());
	}

	/// <summary>Retrieves the current value pointed to by the index.</summary>
	/// <returns>A reference to the current value of the index.</returns>
	const T& get_value()
	{
		return m_Values[m_Index];
	}
};

class StrFuncIndex : public VectorIndex<std::string>
{
protected:
	// The function to call whenever the index changes.
	void (HuneGraphic::*m_ValueFunc)(std::string);

	/// <summary>Sets the index value.</summary>
	/// <param name="index">The value to set the index to.</param>
	void set(int index)
	{
		m_Index = index;
		(m_HuneGraphic->*m_ValueFunc)(get_value());
	}

public:
	/// <summary>Constructs the index.</summary>
	/// <param name="hune">The hune graphic to change whenever the index changes.</param>
	/// <param name="values">The possible values for the index to take.</param>
	/// <param name="value_func">The function to call whenever the index changes.</param>
	StrFuncIndex(HuneGraphic* hune, std::vector<std::string>& values, void (HuneGraphic::*value_func)(std::string)) : VectorIndex<string>(hune, values)
	{
		m_ValueFunc = value_func;
		set(0);
	}
};

class StrFuncIndexWithDependents : public StrFuncIndex
{
protected:
	// List of feature indices that are dependent on this index.
	vector<Index*> m_Dependents;

	/// <summary>Sets the index value.</summary>
	/// <param name="index">The value to set the index to.</param>
	void set(int index)
	{
		StrFuncIndex::set(index);

		for (auto iter = m_Dependents.begin(); iter != m_Dependents.end(); ++iter)
		{
			(*iter)->increment();
			(*iter)->decrement();
		}
	}

public:
	/// <summary>Constructs the index.</summary>
	/// <param name="hune">The hune graphic to change whenever the index changes.</param>
	/// <param name="values">The possible values for the index to take.</param>
	/// <param name="value_func">The function to call whenever the index changes.</param>
	StrFuncIndexWithDependents(HuneGraphic* hune, std::vector<std::string>& values, void (HuneGraphic::*value_func)(std::string), vector<Index*> dependents) : StrFuncIndex(hune, values, value_func)
	{
		m_Dependents = dependents;
		set(0);
	}
};

class ColorFuncIndex : public VectorIndex<vec3i>
{
protected:
	// The function to call whenever the index changes.
	void (HuneGraphic::*m_ValueFunc)(const vec4i&, const vec4i&, const vec4i&);

	/// <summary>Sets the index value.</summary>
	/// <param name="index">The value to set the index to.</param>
	void set(int index)
	{
		m_Index = index;
		(m_HuneGraphic->*m_ValueFunc)(hsv_to_rgb(get_value()), vec4i(255, 255, 220, 255), vec4i(0, 0, 25, 255));
	}

public:
	/// <summary>Constructs the index.</summary>
	/// <param name="hune">The hune graphic to change whenever the index changes.</param>
	/// <param name="values">The possible values for the index to take.</param>
	/// <param name="value_func">The function to call whenever the index changes.</param>
	ColorFuncIndex(HuneGraphic* hune, void (HuneGraphic::*value_func)(const vec4i&, const vec4i&, const vec4i&)) : VectorIndex<vec3i>(hune, g_Colors)
	{
		m_ValueFunc = value_func;
		set(0);
	}
};


HuneCreator::HuneCreator()
{
	Index* jacket_index = nullptr;
	m_Features.emplace("jacket style", jacket_index = new StrFuncIndex(this, g_JacketTextures, &HuneGraphic::set_jacket_texture));
	m_Features.emplace("jacket primary_color", new ColorFuncIndex(this, &HuneGraphic::set_jacket_primary_color));
	m_Features.emplace("jacket secondary_color", new ColorFuncIndex(this, &HuneGraphic::set_jacket_secondary_color));

	Index* top_index = nullptr;
	m_Features.emplace("top style", top_index = new StrFuncIndex(this, g_TopTextures, &HuneGraphic::set_top_texture));
	m_Features.emplace("top primary_color", new ColorFuncIndex(this, &HuneGraphic::set_top_primary_color));
	m_Features.emplace("top secondary_color", new ColorFuncIndex(this, &HuneGraphic::set_top_secondary_color));

	Index* bottom_index = nullptr;
	m_Features.emplace("bottoms style", bottom_index = new StrFuncIndex(this, g_BottomTextures, &HuneGraphic::set_bottom_texture));
	m_Features.emplace("bottoms primary_color", new ColorFuncIndex(this, &HuneGraphic::set_bottom_primary_color));
	m_Features.emplace("bottoms secondary_color", new ColorFuncIndex(this, &HuneGraphic::set_bottom_secondary_color));

	Index* shoes_index = nullptr;
	m_Features.emplace("shoes style", shoes_index = new StrFuncIndex(this, g_ShoesTextures, &HuneGraphic::set_shoe_texture));
	m_Features.emplace("shoes primary_color", new ColorFuncIndex(this, &HuneGraphic::set_shoe_primary_color));
	m_Features.emplace("shoes secondary_color", new ColorFuncIndex(this, &HuneGraphic::set_shoe_secondary_color));

	m_Features.emplace("skull", new StrFuncIndex(this, g_SkullTypes, &HuneGraphic::set_head_shape));
	m_Features.emplace("snout", new StrFuncIndex(this, g_SnoutTypes, &HuneGraphic::set_snout_shape));
	m_Features.emplace("upper_head", new StrFuncIndex(this, g_UpperHeadTypes, &HuneGraphic::set_upper_head_feature));
	m_Features.emplace("body", new StrFuncIndexWithDependents(this, g_BodyTypes, &HuneGraphic::set_body_type, 
		{ jacket_index, top_index, bottom_index, shoes_index }
	));
	m_Features.emplace("arms", new StrFuncIndexWithDependents(this, g_ArmTypes, &HuneGraphic::set_arm_type,
		{ jacket_index, top_index }
	));

	m_Features.emplace("head markings", new StrFuncIndex(this, g_HeadMarkingTextures, &HuneGraphic::set_head_texture));
	m_Features.emplace("body markings", new StrFuncIndex(this, g_BodyMarkingTextures, &HuneGraphic::set_body_texture));

	m_Features.emplace("body primary_color", new ColorFuncIndex(this, &HuneGraphic::set_primary_color));
	m_Features.emplace("body secondary_color", new ColorFuncIndex(this, &HuneGraphic::set_secondary_color));
	m_Features.emplace("body tertiary_color", new ColorFuncIndex(this, &HuneGraphic::set_tertiary_color));
}

HuneCreator::~HuneCreator()
{
	for (auto iter = m_Features.begin(); iter != m_Features.end(); ++iter)
	{
		delete iter->second;
	}
}

void HuneCreator::start_walking()
{
	set_animation(generate_hune_walking_animation());
}

void HuneCreator::stop_walking()
{
	set_animation(generate_hune_standing_animation());
}

void HuneCreator::rotate_right()
{
	facing = (HuneDirection)(((int)facing + HUNE_NUM_DIRECTIONS - 1) % HUNE_NUM_DIRECTIONS);
}

void HuneCreator::rotate_left()
{
	facing = (HuneDirection)(((int)facing + 1) % HUNE_NUM_DIRECTIONS);
}

HuneCreator::Index* HuneCreator::get_feature(string id)
{
	auto iter = m_Features.find(id);
	if (iter != m_Features.end())
		return iter->second;
	return nullptr;
}


HuneCreator::Index::Index(HuneGraphic* hune)
{
	m_HuneGraphic = hune;
}

HuneCreator::Index::~Index() {}