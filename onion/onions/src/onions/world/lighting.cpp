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

	void PointLight::reset() const
	{
		Lighting::Light::reset();

		set<FLOAT_VEC3>("pos", pos);
	}

	void CubeLight::reset() const
	{
		Lighting::Light::reset();

		set<FLOAT_VEC3>("mins", mins);
		set<FLOAT_VEC3>("maxs", maxs);
	}

	void ConeLight::reset() const
	{
		Lighting::Light::reset();

		set<FLOAT_VEC3>("pos", pos);
		set<FLOAT_VEC3>("dir", dir);
		set<Float>("angle", angle);
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
		static opengl::_Image* bluenoise = new opengl::_Image("world/greynoise.png");
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
			LightObject(new OrthogonalPrism(position, dimensions))
		{
			m_Light.mins = position;
			m_Light.maxs = position + dimensions;

			m_Light.color = color;
			m_Light.intensity = intensity;

			m_Light.radius = radius;
		}

		Lighting::Light* CubeLightObject::get_light()
		{
			return &m_Light;
		}


		CubeLightObjectGenerator::CubeLightObjectGenerator(std::string id, const StringData& params) : ObjectGenerator(id)
		{
			if (!params.get("size", m_Dimensions)) m_Dimensions = vec3i(0, 0, 0);
			m_Dimensions = m_Dimensions;

			vec3i color;
			if (!params.get("color", color)) color = vec3i(255, 255, 255);
			m_Color = color / 255.f;

			Int intensity;
			if (!params.get("intensity", intensity)) intensity = 0;
			m_Intensity = 0.01f * intensity;

			if (!params.get("radius", m_Radius)) m_Radius = 0;
		}
	
		Object* CubeLightObjectGenerator::generate(const StringData& params) const
		{
			vec3i pos;
			params.get("pos", pos);

			return new CubeLightObject(
				pos,
				m_Dimensions,
				m_Color,
				m_Intensity,
				m_Radius
			);
		}



		ConeLightObject::ConeLightObject(const vec3i& position, const vec3f& color, Float intensity, const vec3f& dir, Float angle, Int radius)
			: LightObject(new Point(position))
		{
			m_Light.pos = position;

			m_Light.color = color;
			m_Light.intensity = intensity;

			dir.normalize(m_Light.dir);
			m_Light.angle = angle;
			m_Light.radius = radius;
		}

		Lighting::Light* ConeLightObject::get_light()
		{
			return &m_Light;
		}


		ConeLightObjectGenerator::ConeLightObjectGenerator(std::string id, const StringData& params) : ObjectGenerator(id)
		{
			vec3i color;
			if (!params.get("color", color)) color = vec3i(255, 255, 255);
			for (int k = 2; k >= 0; --k)
				m_Color(k) = color(k) / 255.f;

			Int intensity;
			if (!params.get("intensity", intensity)) intensity = 0;
			m_Intensity = 0.01f * intensity;

			if (!params.get("angle", m_Angle)) m_Angle = 0;

			if (!params.get("radius", m_Radius)) m_Radius = 0;
		}

		Object* ConeLightObjectGenerator::generate(const StringData& params) const
		{
			vec3i pos;
			params.get("pos", pos);

			vec3f dir;
			if (!params.get("dir", dir)) dir = vec3f(0.f, 0.f, -1.f);

			return new ConeLightObject(
				pos,
				m_Color,
				m_Intensity,
				dir,
				m_Angle * 0.0174532925f,
				m_Radius
			);
		}

	}

}