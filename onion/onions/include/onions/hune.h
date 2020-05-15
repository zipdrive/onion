#pragma once
#include "graphics.h"
#include "event.h"


#define HUNE_STANDING_INDEX			0
#define HUNE_STANDING_SIZE			1

#define HUNE_WALKING_INDEX			1
#define HUNE_WALKING_SIZE			8

#define HUNE_SIZE					(HUNE_STANDING_SIZE + HUNE_WALKING_SIZE)



// The way that the hune is facing.
enum HuneDirection
{
	FACING_FRONT = 0,
	FACING_LEFT = 1,
	FACING_BACK = 2,
	FACING_RIGHT = 3
};

#define HUNE_NUM_DIRECTIONS			4


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
	static TextureMapSpriteSheet* sprite_sheet;

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



class HuneGraphic : public Graphic
{
protected:
	// The palette for the whole body.
	MultiplePalette m_BodyPalette;

	// The textures, palettes, and sprites of the lower body.
	HuneLowerBody m_BaseLowerBody;

	// The textures, palettes, and sprites of the upper body.
	HuneUpperBody m_BaseUpperBody;


	// The base shading for the head.
	HuneShading* m_Skull;

	// The base shading for the snout.
	HuneShading* m_Snout;

	// The base shading for the right ear.
	HuneShading* m_RightEar;

	// The base shading for the left ear.
	HuneShading* m_LeftEar;

	// The texture for the head.
	Texture* m_HeadTexture;


	// The current animation.
	HuneAnimation* m_Animation;

public:
	// The direction that the hune is facing.
	HuneDirection facing;

	/// <summary>Constructs a hune graphic.</summary>
	HuneGraphic();

	/// <summary>Sets the body type.</summary>
	/// <param name="body_type">A string identifier for the body type.</param>
	void set_body_type(std::string body_type);

	/// <summary>Sets the shape of the head.</summary>
	/// <param name="body_type">A string identifier for the head shape.</param>
	void set_head_shape(std::string head_shape);

	/// <summary>Sets the shape of the snout on the head.</summary>
	/// <param name="body_type">A string identifier for the shape of the snout.</param>
	void set_snout_shape(std::string head_shape);

	/// <summary>Sets the shape of the ears.</summary>
	/// <param name="body_type">A string identifier for the shape of the ears.</param>
	void set_ear_shape(std::string ear_shape);

	/// <summary>Sets the textures for the body.</summary>
	/// <param name="head">The texture ID of the head. Set to "" for no change.</param>
	/// <param name="arms">The texture ID of the arms. Set to "" for no change.</param>
	/// <param name="torso">The texture ID of the torso. Set to "" for no change.</param>
	/// <param name="legs">The texture ID of the legs. Set to "" for no change.</param>
	void set_textures(std::string head, std::string arms, std::string torso, std::string legs);

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

	/// <summary>Sets the animation of the hune graphic.</summary>
	/// <param name="animation">The animation to play.</param>
	void set_animation(HuneAnimation* animation);

	/// <summary>Retrieves the width of the hune graphic.</summary>
	int get_width() const;

	/// <summary>Retrieves the height of the hune graphic.</summary>
	int get_height() const;

	/// <summary>Displays the hune.</summary>
	virtual void display() const;
};

