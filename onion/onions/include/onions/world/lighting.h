#pragma once
#include <unordered_set>
#include "../graphics/shader.h"
#include "camera.h"
#include "object.h"

namespace onion
{

	struct Light
	{
		struct Color
		{
			// The diffuse color of the light.
			vec3f color;

			// The intensity of the specular highlight.
			Float intensity;

		} color;

		// The maximum distance from the light source that light still illuminates.
		Int radius;
	};



#define MAX_NUMBER_CUBE_LIGHTS 8
	
	struct CubeLight : public Light
	{
		// The corner of the light with minimum values.
		vec3f mins;

		// The corner of the light with maximum values.
		vec3f maxs;
	};


	class Lighting
	{
	private:
		// The uniform buffer that stores data used to calculate lights.
		static opengl::_UniformBuffer* m_Buffer;


		// A vector of all currently set point lights.
		static std::vector<CubeLight*> m_CubeLights;

		/// <summary>Sets the point light at the specified index.</summary>
		/// <param name="index">The index of the light.</param>
		/// <param name="light">The data of the light.</param>
		static void set(int index, CubeLight* light);

	public:
		/// <summary>Initializes the lighting.</summary>
		static void init();


		/// <summary>Sets the ambient lighting.</summary>
		/// <param name="ambient">The ambient color.</param>
		static void set_ambient_light(const vec3f& ambient);

		
		/// <summary>Lights up the scene with the given light.</summary>
		/// <param name="light">The data about the light.</param>
		static void add(CubeLight* light);

		/// <summary>Removes the given light from the scene.</summary>
		/// <param name="light">A pointer to the light.</param>
		static void remove(CubeLight* light);
	};


	/// <summary>Retrieves the image used to sample for blue noise dithering.</summary>
	/// <returns>An image that can be used to sample for blue noise dithering.</returns>
	opengl::_Image* get_bluenoise_image();



	namespace world
	{

		// An interface for objects that cast light on their surroundings.
		class LightObject : public Object
		{
		public:
			/// <summary>Constructs an object that projects light.</summary>
			/// <param name="bounds">The bounds of the object.</param>
			LightObject(Shape* bounds);

			/// <summary>Retrieves a pointer to the data about the light.</summary>
			/// <returns>A pointer to the data about the light.</returns>
			virtual const Light* get_light() const = 0;

			/// <summary>Toggles whether the light is being used or not.<summary>
			/// <param name="on">True if the light should be activated, false if it should be turned off.</param>
			virtual void toggle(bool on) = 0;
		};


		// An object shaped like a rectangular prism that projects light.
		class CubeLightObject : public LightObject
		{
		protected:
			// The light's data.
			CubeLight m_Light;

		public:
			/// <summary>Constructs a light object.</summary>
			/// <param name="position">The position of the light object.</param>
			/// <param name="dimensions">The dimensions of the light object.</param>
			/// <param name="color">The color of the light.</param>
			/// <param name="intensity">The intensity of the light's highlight. A number from -1 (for no light) to 0 (for no highlight) to 1 (for a very intense highlight).<param>
			/// <param name="radius">The maximum radius of the light.</param>
			CubeLightObject(const vec3i& position, const vec3i& dimensions, const vec3f& color, Float intensity, Int radius);

			/// <summary>Retrieves a pointer to the data about the light.</summary>
			/// <returns>A pointer to the data about the light.</returns>
			const Light* get_light() const;

			/// <summary>Toggles whether the light is being used or not.<summary>
			/// <param name="on">True if the light should be activated, false if it should be turned off.</param>
			virtual void toggle(bool on);
		};

		class CubeLightObjectGenerator : public ObjectGenerator
		{
		protected:
			// The dimensions of the light.
			vec3i m_Dimensions;

			// The color of the light.
			vec3f m_Color;

			// The intensity of the light.
			Float m_Intensity;

			// The radius of the light.
			Int m_Radius;

		public:
			CubeLightObjectGenerator(std::string id, const _StringData& params);

			Object* generate(const _StringData& params) const;
		};

	}

}