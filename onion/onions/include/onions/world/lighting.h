#pragma once
#include "../graphics/shader.h"

namespace onion
{

	struct Light
	{
		struct Color
		{
			// The diffuse color of the light.
			vec3f color;

			// The intensity of the specular highlight.
			float intensity;
		};

		struct Brightness
		{
			// How bright the light is.
			float constant;

			// How much the brightness drops off proportional to the distance from the light.
			float linear;

			// How much the brightness drops off proportional to the distance squared from the light.
			float quadratic;
		};
	};



#define MAX_NUMBER_CUBE_LIGHTS 8
	
	struct CubeLight : public Light
	{
		// The corner of the light with minimum values.
		vec3f mins;

		// The corner of the light with maximum values.
		vec3f maxs;


		// The color of the light.
		Color color;

		// The maximum radius of the light.
		float radius;
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

}