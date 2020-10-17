#pragma once
#include <unordered_set>
#include "../graphics/shader.h"
#include "camera.h"
#include "object.h"

namespace onion
{

	class Lighting;

	



#define MAX_NUMBER_CUBE_LIGHTS 8
	
	


	class Lighting
	{
	private:
		// The uniform buffer that stores data used to calculate lights.
		static opengl::_UniformBuffer* m_Buffer;

	public:
		struct Light
		{
			// The prefix for the name of the light in the buffer.
			std::string prefix;


			// The diffuse color of the light.
			vec3f color;

			// The intensity of the specular highlight.
			Float intensity;

			// The maximum distance from the light source that light still illuminates, in pixel coordinates.
			Int radius;


			/// <summary>Sets a single uniform in the buffer.</summary>
			/// <param name="name">The name of the uniform.</param>
			/// <param name="value">The value of the uniform.</param>
			template <typename T>
			void set(const char* name, const T& value) const
			{
				if (!prefix.empty())
				{
					m_Buffer->set<T>(prefix + name, value);
				}
			}

			/// <summary>Resets all uniforms in the buffer.</summary>
			virtual void reset() const;
		};

	private:
		// An array of lights in the lighting buffer.
		struct _LightArray
		{
			// The maximum number of lights that can be stored in the array.
			const Int max_size;


			/// <summary>Constructs a light array with the given maximum size.</summary>
			/// <param name="max_size">The maximum number of lights that can be in the array.</param>
			_LightArray(Int max_size) : max_size(max_size) {}


			/// <summary>Adds the light to the array.</summary>
			/// <param name="light">The light to add.</param>
			virtual bool add(Light* light) = 0;

			/// <summary>Removes the light from the array.</summary>
			/// <param name="light">The light to remove.</param>
			virtual bool remove(Light* light) = 0;
		};
		
		// An array of lights in the lighting buffer.
		template <typename T>
		struct LightArray : public _LightArray
		{
			// The name of the array in the buffer.
			const std::string array_name;

			// The name of the array size in the buffer.
			const std::string count_name;


			// The lights currently set to the buffer.
			std::vector<T*> elements;


			/// <summary>Constructs a light array with the given maximum size.</summary>
			/// <param name="array_name">The name for the uniform storing the data for the lights.</param>
			/// <param name="count_name">The name for the uniform storing the size of the array.</param>
			/// <param name="max_size">The maximum number of lights that can be in the array.</param>
			LightArray(std::string array_name, std::string count_name, Int max_size) : array_name(array_name), count_name(count_name), _LightArray(max_size)
			{
				elements.reserve(max_size);
			}

			/// <summary>Sets the light at the specified index.</summary>
			/// <param name="index">The index of the light.</param>
			/// <param name="light">The data for the light.</param>
			void set(Int index, T* light)
			{
				if (index >= elements.size())
				{
					elements.resize(index + 1);
					m_Buffer->set<Int>(count_name, elements.size());
				}
				else
				{
					elements[index]->prefix = "";
				}

				elements[index] = light;
				light->prefix = array_name + "[" + std::to_string(index) + "].";
				light->reset();
			}

			/// <summary>Adds the light to the array.</summary>
			/// <param name="light">The light to add.</param>
			bool add(Light* light)
			{
				if (T* ptr = dynamic_cast<T*>(light)) // Check that the light is the correct type
				{
					for (int k = 0; k <= elements.size(); ++k)
					{
						if (k >= max_size)
						{
							// Don't add the light if it would cause the array to exceed the maximum size
							return false;
						}
						else if (k == elements.size())
						{
							// Push the light to the back of the array
							set(k, ptr);
							return true;
						}
						else if (elements[k] == ptr)
						{
							// Reset the data for the light
							ptr->reset();
							return true;
						}
					}
				}

				return false;
			}

			/// <summary>Removes the light from the array.</summary>
			/// <param name="light">The light to remove.</param>
			bool remove(Light* light)
			{
				if (T* ptr = dynamic_cast<T*>(light)) // Check that the light is the correct type
				{
					for (int k = elements.size() - 1; k >= 0; --k)
					{
						if (elements[k] == ptr)
						{
							// If the light being removed isn't at the back, move the light that is at the back to replace the light being removed
							if (k < elements.size() - 1)
								set(k, elements.back());

							// Resize the array
							elements.pop_back();
							m_Buffer->set<Int>(count_name, elements.size());

							return true;
						}
					}

					return false;
				}

				return false;
			}
		};

		// All light arrays.
		static std::vector<_LightArray*> m_LightArrays;

	public:
		/// <summary>Initializes the lighting.</summary>
		static void init();

		/// <summary>Constructs a light array with the given maximum size.</summary>
		/// <param name="array_name">The name for the uniform storing the data for the lights.</param>
		/// <param name="count_name">The name for the uniform storing the size of the array.</param>
		/// <param name="max_size">The maximum number of lights that can be in the array.</param>
		template <typename T>
		static void add_light_array(std::string array_name, std::string count_name, Int max_size)
		{
			m_LightArrays.push_back(new LightArray<T>(array_name, count_name, max_size));
		}


		/// <summary>Sets the ambient lighting.</summary>
		/// <param name="ambient">The ambient color.</param>
		static void set_ambient_light(const vec3f& ambient);

		
		/// <summary>Lights up the scene with the given light.</summary>
		/// <param name="light">The data about the light.</param>
		static void add(Lighting::Light* light);

		/// <summary>Removes the given light from the scene.</summary>
		/// <param name="light">A pointer to the light.</param>
		static void remove(Lighting::Light* light);
	};


	struct PointLight : public Lighting::Light
	{
		// The position of the light, in pixel coordinates.
		vec3f pos;


		/// <summary>Resets all uniforms in the buffer.</summary>
		virtual void reset() const;
	};
	
	struct CubeLight : public Lighting::Light
	{
		// The corner of the light with minimum values, in pixel coordinates.
		vec3f mins;

		// The corner of the light with maximum values, in pixel coordinates.
		vec3f maxs;


		/// <summary>Resets all uniforms in the buffer.</summary>
		virtual void reset() const;
	};

	struct ConeLight : public Lighting::Light
	{
		// The position of the light, in pixel coordinates.
		vec3f pos;

		// The direction of the light, normalized.
		vec3f dir;

		// The maximum angle that a point within the cone can make with the apex of the cone, in radians.
		Float angle;


		/// <summary>Resets all uniforms in the buffer.</summary>
		virtual void reset() const;
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
			virtual Lighting::Light* get_light() = 0;

			/// <summary>Toggles whether the light is being used or not.<summary>
			/// <param name="on">True if the light should be activated, false if it should be turned off.</param>
			virtual void toggle(bool on);
		};


		// An object shaped like a rectangular prism that projects light.
		class CubeLightObject : public LightObject
		{
		protected:
			// The light's data.
			CubeLight m_Light;

		public:
			/// <summary>Constructs a light object.</summary>
			/// <param name="position">The position of the light object, in unit coordinates.</param>
			/// <param name="dimensions">The dimensions of the light object, in unit coordinates.</param>
			/// <param name="color">The color of the light.</param>
			/// <param name="intensity">The intensity of the light's highlight. A number from 0 (for no light) to 1 (for no highlight) to 2+ (for a very intense highlight).<param>
			/// <param name="radius">The maximum radius of the light, in unit coordinates.</param>
			CubeLightObject(const vec3i& position, const vec3i& dimensions, const vec3f& color, Float intensity, Int radius);

			/// <summary>Retrieves a pointer to the data about the light.</summary>
			/// <returns>A pointer to the data about the light.</returns>
			Lighting::Light* get_light();
		};

		// A generator for a cube light.
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
			/// <summary>Constructs a generator for a cube light.</summary>
			/// <param name="id">The ID of the generator.</param>
			/// <param name="params">The parameters to pass to the generator.</param>
			CubeLightObjectGenerator(std::string id, const StringData& params);

			/// <summary>Generates a cube light object.</summary>
			/// <returns>A CubeLightObject created with new.</returns>
			Object* generate(const StringData& params) const;
		};


		// A point that projects light outward in the shape of a cone.
		class ConeLightObject : public LightObject
		{
		protected:
			// The light's data.
			ConeLight m_Light;

		public:
			/// <summary>Constructs a light object.</summary>
			/// <param name="position">The position of the light object.</param>
			/// <param name="color">The color of the light.</param>
			/// <param name="intensity">The intensity of the light's highlight. A number from -1 (for no light) to 0 (for no highlight) to 1 (for a very intense highlight).<param>
			/// <param name="dir">The direction of the light emanating from the position.</param>
			/// <param name="angle">The maximum angle between a point in the cone and the direction of the light, in radians.</param>
			/// <param name="radius">The maximum radius of the light, in unit coordinates.</param>
			ConeLightObject(const vec3i& position, const vec3f& color, Float intensity, const vec3f& dir, Float angle, Int radius);

			/// <summary>Retrieves a pointer to the data about the light.</summary>
			/// <returns>A pointer to the data about the light.</returns>
			Lighting::Light* get_light();
		};

		// A generator for a cone light.
		class ConeLightObjectGenerator : public ObjectGenerator
		{
		protected:
			// The color of the light.
			vec3f m_Color;

			// The intensity of the light.
			Float m_Intensity;

			// The angle of the cone.
			Int m_Angle;

			// The radius of the light.
			Int m_Radius;

		public:
			/// <summary>Constructs a generator for a cube light.</summary>
			/// <param name="id">The ID of the generator.</param>
			/// <param name="params">The parameters to pass to the generator.</param>
			ConeLightObjectGenerator(std::string id, const StringData& params);

			/// <summary>Generates a cone light object.</summary>
			/// <returns>A ConeLightObject created with new.</returns>
			Object* generate(const StringData& params) const;
		};

	}

}