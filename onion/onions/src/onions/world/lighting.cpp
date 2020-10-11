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


	void Lighting::Light::reset() const
	{
		set<FLOAT_VEC3>("color", color);
		set<Float>("intensity", intensity);
		set<Float>("radius", radius);
	}

	void CubeLight::reset() const
	{
		Lighting::Light::reset();

		set<FLOAT_VEC3>("mins", mins);
		set<FLOAT_VEC3>("maxs", maxs);
	}



	void Lighting::set_ambient_light(const vec3f& ambient)
	{
		m_Buffer->set<FLOAT_VEC3>("ambient", ambient);
	}


	std::vector<Lighting::_LightArray*> Lighting::m_LightArrays{};

	void Lighting::add(Lighting::Light* light)
	{
		for (auto iter = m_LightArrays.begin(); iter != m_LightArrays.end(); ++iter)
			if ((*iter)->add(light))
				break;
	}

	void Lighting::remove(Lighting::Light* light)
	{
		for (auto iter = m_LightArrays.begin(); iter != m_LightArrays.end(); ++iter)
			if ((*iter)->remove(light))
				break;
	}


	opengl::_Image* get_bluenoise_image()
	{
		static opengl::_Image* bluenoise = new opengl::_Image("world/bluenoise.png");
		return bluenoise;
	}



	namespace world
	{

		LightObject::LightObject(Shape* bounds) : Object(bounds) {}

		void LightObject::toggle(bool on)
		{
			if (on)
				Lighting::add(get_light());
			else
				Lighting::remove(get_light());
		}


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

			m_Light.color = color;
			m_Light.intensity = intensity;

			m_Light.radius = radius / UNITS_PER_PIXEL;
		}

		Lighting::Light* CubeLightObject::get_light()
		{
			return &m_Light;
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