#include "../../../include/onions/world/lighting.h"

namespace onion
{

	opengl::_UniformBuffer* Lighting::m_Buffer{ nullptr };
	
	void Lighting::init()
	{
		m_Buffer = opengl::_UniformBuffer::get_buffer("Lighting");
	}


	void Lighting::set_ambient_light(const vec3f& ambient)
	{
		m_Buffer->set<FLOAT_VEC3>("ambient", ambient);
	}


	std::vector<CubeLight*> Lighting::m_CubeLights{};

	void Lighting::set(int index, CubeLight* light)
	{
		if (index >= m_CubeLights.size())
		{
			m_CubeLights.resize(index + 1);
			m_Buffer->set<Int>("numCubeLights", m_CubeLights.size());
		}

		m_CubeLights[index] = light;

		std::string base = "cubeLights[" + std::to_string(index) + "].";
		
		m_Buffer->set<FLOAT_VEC3>(base + "mins", light->mins);
		m_Buffer->set<FLOAT_VEC3>(base + "maxs", light->maxs);

		m_Buffer->set<FLOAT_VEC3>(base + "color", light->color.color);
		m_Buffer->set<Float>(base + "intensity", light->color.intensity);

		m_Buffer->set<Float>(base + "radius", light->radius);
	}

	void Lighting::add(CubeLight* light)
	{
		// Make sure you aren't adding the same light twice
		for (int index = 0; index < m_CubeLights.size(); ++index)
		{
			if (m_CubeLights[index] == light)
			{
				set(index, light);
				return;
			}
		}

		// Make sure you aren't exceeding the maximum number of allowed lights
		if (m_CubeLights.size() < MAX_NUMBER_CUBE_LIGHTS - 1)
		{
			// Add the light at the back
			set(m_CubeLights.size(), light);
		}
	}

	void Lighting::remove(CubeLight* light)
	{
		for (int index = m_CubeLights.size() - 1; index >= 0; --index)
		{
			if (m_CubeLights[index] == light)
			{
				// If the light being removed isn't at the back, move the light that is at the back to replace the light being removed
				if (index < m_CubeLights.size() - 1)
					set(index, m_CubeLights.back());

				// Resize the array
				m_CubeLights.pop_back();
				m_Buffer->set<Int>("numCubeLights", m_CubeLights.size());

				return;
			}
		}
	}


	opengl::_Image* get_bluenoise_image()
	{
		static opengl::_Image* bluenoise = new opengl::_Image("world/bluenoise.png");
		return bluenoise;
	}

}