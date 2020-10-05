#pragma once
#include "../graphics/shader.h"

namespace onion
{


#define MAX_NUMBER_POINT_LIGHTS

	struct PointLight
	{
		// The position of the light.
		vec3f position;

		// The diffuse color of the light.
		vec3f diffuse;

		// The specular color of the light.
		vec3f specular;


		// How much the brightness drops off proportional to the distance from the light.
		float linear;

		// How much the brightness drops off proportional to the distance squared from the light.
		float quadratic;
	};


	class Lighting
	{
	private:
		// The uniform buffer that stores data used to calculate lights.
		static opengl::_UniformBuffer* m_Buffer;


		// A vector of all currently set point lights.
		static std::vector<PointLight*> m_PointLights;

		/// <summary>Sets the point light at the specified index.</summary>
		/// <param name="index">The index of the light.</param>
		/// <param name="light">The data of the light.</param>
		static void set(int index, PointLight* light);

	public:
		/// <summary>Initializes the lighting.</summary>
		static void init();


		/// <summary>Lights up the scene with the given light.</summary>
		/// <param name="light">The data about the light.</param>
		static void add(PointLight* light);

		/// <summary>Removes the given light from the scene.</summary>
		/// <param name="light">A pointer to the light.</param>
		static void remove(PointLight* light);
	};

}