#include "../../../include/onions/world/lighting.h"

namespace onion
{

	opengl::_UniformBuffer* Lighting::m_Buffer{ nullptr };
	
	void Lighting::init()
	{
		m_Buffer = opengl::_UniformBuffer::get_buffer("Lighting");
	}


	std::vector<PointLight*> Lighting::m_PointLights{};

	void Lighting::set(int index, PointLight* light)
	{
		if (index >= m_PointLights.size())
		{
			m_PointLights.resize(index + 1);
			m_Buffer->set<int>("num_point_lights", m_PointLights.size());
		}

		m_PointLights[index] = light;

		std::string base = "point_lights[" + std::to_string(index) + "].";
		m_Buffer->set(base + "position", light->position);
		m_Buffer->set(base + "linear", light->linear);
		m_Buffer->set(base + "quadratic", light->quadratic);
	}

	void Lighting::add(PointLight* light)
	{
		// Make sure you aren't adding the same light twice
		for (int index = 0; index < m_PointLights.size(); ++index)
		{
			if (m_PointLights[index] == light)
			{
				set(index, light);
				return;
			}
		}

		// Make sure you aren't exceeding the maximum number of allowed lights
		if (m_PointLights.size() < MAX_NUMBER_POINT_LIGHTS - 1)
		{
			// Add the light at the back
			set(m_PointLights.size(), light);
		}
	}

	void Lighting::remove(PointLight* light)
	{
		for (int index = m_PointLights.size() - 1; index >= 0; --index)
		{
			if (m_PointLights[index] == light)
			{
				// If the light being removed isn't at the back, move the light that is at the back to replace the light being removed
				if (index < m_PointLights.size() - 1)
					set(index, m_PointLights.back());

				// Resize the array
				m_PointLights.pop_back();
				m_Buffer->set<int>("num_point_lights", m_PointLights.size());

				return;
			}
		}
	}


}