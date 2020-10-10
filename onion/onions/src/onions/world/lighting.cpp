#include <regex>
#include "../../../include/onions/world/camera.h"
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



	namespace world
	{

		LightObject::LightObject(Shape* bounds) : Object(bounds) {}


		CubeLightObject::CubeLightObject(const vec3i& position, const vec3i& dimensions, const vec3f& color, Float intensity, Int radius) :
			LightObject(new RectangularPrism(position, dimensions))
		{
			m_Light.mins = vec3f(
				position.get(0) / UNITS_PER_PIXEL, 
				position.get(1) / UNITS_PER_PIXEL, 
				position.get(2) / UNITS_PER_PIXEL
			);
			m_Light.maxs = vec3f(
				(position.get(0) + dimensions.get(0)) / UNITS_PER_PIXEL, 
				(position.get(1) + dimensions.get(1)) / UNITS_PER_PIXEL, 
				(position.get(2) + dimensions.get(2)) / UNITS_PER_PIXEL
			);

			m_Light.color.color = color;
			m_Light.color.intensity = intensity;

			m_Light.radius = radius / UNITS_PER_PIXEL;
		}

		const Light* CubeLightObject::get_light() const
		{
			return &m_Light;
		}

		void CubeLightObject::toggle(bool on)
		{
			if (on)
				Lighting::add(&m_Light);
			else
				Lighting::remove(&m_Light);
		}


		CubeLightObjectGenerator::CubeLightObjectGenerator(std::string id, const _StringData& params) : ObjectGenerator(id)
		{
			if (!params.get("dx", m_Dimensions(0))) m_Dimensions(0) = 0;
			if (!params.get("dy", m_Dimensions(1))) m_Dimensions(1) = 0;
			if (!params.get("dz", m_Dimensions(2))) m_Dimensions(2) = 0;
			m_Dimensions = UNITS_PER_PIXEL * m_Dimensions;

			std::string color;
			vec3i rgb(255, 255, 255);
			if (params.get("color", color))
			{
				std::regex color_regex("(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)");
				std::smatch match;
				if (std::regex_match(color, match, color_regex))
				{
					rgb(0) = std::stoi(match[1].str());
					rgb(1) = std::stoi(match[2].str());
					rgb(2) = std::stoi(match[3].str());
				}
			}
			for (int k = 2; k >= 0; --k)
				m_Color(k) = rgb(k) / 255.f;

			Int intensity;
			if (!params.get("intensity", intensity)) intensity = 0;
			m_Intensity = 0.01f * intensity;

			if (!params.get("radius", m_Radius)) m_Radius = 0;
			m_Radius *= UNITS_PER_PIXEL;
		}
		
		Object* CubeLightObjectGenerator::generate(const _StringData& params) const
		{
			vec3i pos;
			if (!params.get("x", pos(0))) pos(0) = 0;
			if (!params.get("y", pos(1))) pos(1) = 0;
			if (!params.get("z", pos(2))) pos(2) = 0;

			return new CubeLightObject(
				UNITS_PER_PIXEL * pos,
				m_Dimensions,
				m_Color,
				m_Intensity,
				m_Radius
			);
		}


	}

}