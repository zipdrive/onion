#pragma once
#include <vector>
#include <unordered_map>

#define BUFFER_KEY int

namespace onion
{

	namespace opengl
	{

		// Predeclaration of the ID object.
		struct _ID;

		// Predeclaration of the location object.
		struct _Location;



		// Handles all of the OpenGL calls for creating and using shader programs.
		class _Shader
		{
		private:
			// The ID of the currently active shader.
			static _ID* m_ActiveShader;

			// The ID of this shader.
			_ID* m_Shader;

		protected:
			/// <summary>Constructs a shader.</summary>
			/// <param name="vertex_shader_text">The vertex shader, in raw text form.</param>
			/// <param name="fragment_shader_text">The fragment shader, in raw text form.</param>
			_Shader(const char* vertex_shader_text, const char* fragment_shader_text);

			/// <summary>Activates the shader.</summary>
			void __activate();

			/// <summary>Sets a uniform value.</summary>
			/// <param name="loc">The location of the uniform.</param>
			/// <param name="uniform">The value to set the uniform as.</param>
			template <typename _Uniform>
			void set_uniform(_Location* loc, _Uniform uniform);

		public:
			/// <summary>Destroys the shader.</summary>
			virtual ~_Shader();

			/// <summary>Checks whether this shader is the active one.</summary>
			/// <returns>True if this shader is active, false otherwise.</returns>
			bool is_active();

			/// <summary>Retrieves the location of a uniform variable in the shader program.</summary>
			/// <param name="name">The name of the uniform in the shader program.</param>
			/// <returns>The location of the uniform variable.</returns>
			_Location* get_uniform(const char* name);

			/// <summary>Retrieves the location of a vertex attribute in the shader program.</summary>
			/// <param name="name">The name of the vertex attribute in the shader program.</param>
			/// <returns>The location of the vertex attribute variable.</returns>
			_Location* get_attrib(const char* name);
		};


		// Handles all of the OpenGL calls for constructing and binding data buffers.
		class _Buffer
		{
		private:
			// The ID of the currently active buffer.
			static _ID* m_ActiveBuffer;

			// The ID of this buffer.
			_ID* m_Buffer;

		protected:
			/// <summary>Constructs a buffer from data.</summary>
			/// <param name="data">The data to use in the buffer.</param>
			_Buffer(const std::vector<float>& data);

			/// <summary>Activates the buffer.</summary>
			virtual void __activate();

			/// <summary>Sets an vertex attribute array.</summary>
			/// <param name="loc">The location of the vertex attribute in the shader program.</param>
			/// <param name="ptr">The number of floats between the start of the vertex data and the start of this vertex attribute.</param>
			/// <param name="length">The number of floats in the vertex attribute.</param>
			/// <param name="stride">The number of floats corresponding to a single vertex.</param>
			void set_attrib(_Location* loc, unsigned int ptr, unsigned int length, unsigned int stride);

		public:
			/// <summary>Frees the buffer from memory.</summary>
			virtual ~_Buffer();

			/// <summary>Checks whether this buffer is the active one.</summary>
			/// <returns>True if this buffer is active, false otherwise.</returns>
			bool is_active();

			/// <summary>Activates the buffer.</summary>
			void activate();
		};


		// Handles all calls to load and manage images.
		class _Image
		{
		private:
			// The ID of the currently active image.
			static _ID* m_ActiveImage;

			// True if the image has been loaded, false otherwise.
			bool m_IsLoaded;

			// The ID of this image.
			_ID* m_Image;

			// The width of the image.
			int m_Width;

			// The height of the image.
			int m_Height;

			/// <summary>Frees the buffer from memory.</summary>
			void free();

		public:
			/// <summary>Constructs an empty image object to be loaded later.</summary>
			_Image();

			/// <summary>Loads an image from memory.</summary>
			/// <param name="path">The file path to the image, starting from the res/img/ folder.</param>
			/// <param name="pixel_perfect">True if the image should be pixel perfect, false if it can blend.</param>
			_Image(const char* path, bool pixel_perfect = true);

			/// <summary>Frees the buffer from memory.</summary>
			~_Image();

			/// <summary>Checks if the image is loaded and ready to use.</summary>
			/// <returns>True if the image has been loaded successfully and is ready to use, false otherwise.</returns>
			bool is_loaded() const;

			/// <summary>Loads an image from memory.</summary>
			/// <param name="path">The file path to the image, starting from the res/img/ folder.</param>
			/// <param name="pixel_perfect">True if the image should be pixel perfect, false if it can blend.</param>
			/// <returns>True if the image was loaded successfully, false otherwise.</returns>
			bool load(const char* path, bool pixel_perfect = true);

			/// <summary>Retrieves the width of the image.</summary>
			/// <returns>The width of the image, in pixels.</returns>
			int get_width();

			/// <summary>Retrieves the height of the image.</summary>
			/// <returns>The height of the image, in pixels.</returns>
			int get_height();

			/// <summary>Checks whether this buffer is the active one.</summary>
			/// <returns>True if this buffer is active, false otherwise.</returns>
			bool is_active();

			/// <summary>Activates the buffer.</summary>
			void activate();
		};



		// Handles all calls to display something using information from a buffer.
		class _BufferDisplayer
		{
		protected:
			// The buffer to display from.
			_Buffer* m_Buffer = nullptr;

		public:
			/// <summary>Displays using the currently bound shader and information from the buffer.</summary>
			/// <param name="start">A pointer to the start location in the buffer.</param>
			/// <param name="count">The number of sequential shapes to display.</param>
			virtual void display(BUFFER_KEY start, int count = 1) const = 0;

			/// <summary>Sets the buffer to use.</summary>
			/// <param name="buffer">The new buffer to use.</param>
			void set_buffer(_Buffer* buffer);
		};

		// Displays two consecutive triangles in the buffer.
		class _SquareBufferDisplayer : public _BufferDisplayer
		{
		public:
			/// <summary>Displays using the currently bound shader and information from the buffer.</summary>
			/// <param name="start">A pointer to the start location in the buffer.</param>
			/// <param name="count">The number of sequential shapes to display.</param>
			virtual void display(BUFFER_KEY start, int count = 1) const;
		};

	}



	// A wrapper for a shader program.
	template <typename... _Uniforms>
	class Shader : public opengl::_Shader
	{
	private:
		// The locations of all uniform variables.
		opengl::_Location* m_Uniforms[sizeof...(_Uniforms)];

		/// <summary>The end condition for the recursion.</summary>
		void set_uniforms(opengl::_Location** index) {}

		/// <summary>Sets the values of each uniform variable recursively.</summary>
		/// <param name="index">The index of the uniform currently being set.</param>
		/// <param name="first">The value of the uniform currently being set.</param>
		/// <param name="others">The remaining uniform variables.</param>
		template <typename _FirstUniform, typename... _OtherUniforms>
		void set_uniforms(opengl::_Location** index, _FirstUniform first, _OtherUniforms... others)
		{
			set_uniform<const _FirstUniform&>(*index, first);
			set_uniforms(index + 1, others...);
		}

	public:
		/// <summary>Constructs a shader program.</summary>
		/// <param name="vertex_shader_text">The vertex shader, in raw text form.</param>
		/// <param name="fragment_shader_text">The fragment shader, in raw text form.</param>
		/// <param name="uniforms">An array of the names of each uniform variable included in the shader program. Should have size exactly equal to the number of template parameters to this class.</param>
		Shader(const char* vertex_shader_text, const char* fragment_shader_text, const std::vector<std::string>& uniforms) : opengl::_Shader(vertex_shader_text, fragment_shader_text)
		{
			if (sizeof...(_Uniforms) > uniforms.size())
			{
				// TODO throw error
			}

			for (int k = sizeof...(_Uniforms)-1; k >= 0; --k)
			{
				m_Uniforms[k] = get_uniform(uniforms[k].c_str());
			}
		}

		/// <summary>Destroys the shader program.</summary>
		virtual ~Shader()
		{
			for (int k = sizeof...(_Uniforms)-1; k >= 0; --k)
				delete m_Uniforms[k];
		}

		/// <summary>Activates the shader program and sets any uniform variables.</summary>
		/// <param name="uniforms">The values of the uniform variables.</param>
		void activate(_Uniforms... uniforms)
		{
			opengl::_Shader::__activate();
			set_uniforms(m_Uniforms, uniforms...);
		}
	};


	// A wrapper for a buffer of attributes for a shader program.
	template <unsigned int... _Lengths>
	class Buffer : public opengl::_Buffer
	{
	private:
		// The location of each attribute in the buffer.
		opengl::_Location* m_Attribs[sizeof...(_Lengths)];

		/// <summary>End condition for the recursion. Sets the last attrib in the buffer.</summary>
		/// <param name="index">A pointer to the current attrib's location in the attrib location array.</param>
		/// <param name="ptr">The total length of all attribs before this one.</param>
		template <unsigned int _DummyParameter>
		unsigned int set_attrib(opengl::_Location** index, unsigned int ptr)
		{
			return ptr;
		}

		/// <summary>Sets the current attrib and all attribs after it.</summary>
		/// <param name="index">A pointer to the current attrib's location in the attrib location array.</param>
		/// <param name="ptr">The total length of all attribs before this one.</param>
		template <unsigned int _DummyParameter, unsigned int _FirstLength, unsigned int... _OtherLengths>
		unsigned int set_attrib(opengl::_Location** index, unsigned int ptr)
		{
			unsigned int stride = set_attrib<_DummyParameter, _OtherLengths...>(index + 1, ptr + _FirstLength);
			opengl::_Buffer::set_attrib(*index, ptr, _FirstLength, stride);

			return stride;
		}

	protected:
		/// <summary>Activates the buffer.</summary>
		virtual void __activate()
		{
			// Activate the buffer
			opengl::_Buffer::__activate();

			// Set the vertex attrib arrays
			set_attrib<0, _Lengths...>(m_Attribs, 0);
		}

	public:
		/// <summary>Constructs a buffer.</summary>
		/// <param name="shader">The shader program that the buffer is associated with.</param>
		/// <param name="attribs">An array of the names of each attrib variable included in the shader program. Should have size exactly equal to the number of template parameters to this class.</param>
		/// <param name="data">The raw data of the buffer.</param>
		Buffer(opengl::_Shader* shader, const std::vector<std::string>& attribs, const std::vector<float>& data) : opengl::_Buffer(data)
		{
			if (sizeof...(_Lengths) > attribs.size())
			{
				// TODO throw an error
			}

			for (int k = sizeof...(_Lengths)-1; k >= 0; --k)
			{
				m_Attribs[k] = shader->get_attrib(attribs[k].c_str());
			}
		}

		/// <summary>Frees the buffer.</summary>
		virtual ~Buffer()
		{
			for (int k = sizeof...(_Lengths)-1; k >= 0; --k)
			{
				delete m_Attribs[k];
			}
		}
	};

	// A buffer that also manages an image.
	template <unsigned int... _Lengths>
	class ImageBuffer : public Buffer<_Lengths...>
	{
	private:
		// The image in the buffer.
		opengl::_Image* m_Image;

	protected:
		/// <summary>Binds the buffers and the image.</summary>
		virtual void __activate()
		{
			// Bind the buffers
			Buffer<_Lengths...>::__activate();

			// Activate the image
			m_Image->activate();
		}

	public:
		/// <summary>Constructs a buffer that also manages a image.</summary>
		/// <param name="shader">The shader program that the buffer is associated with.</param>
		/// <param name="attribs">An array of the names of each attrib variable included in the shader program. Should have size exactly equal to the number of template parameters to this class.</param>
		/// <param name="data">The raw data of the buffer.</param>
		/// <param name="image">The image to be managed by the buffer.</param>
		ImageBuffer(opengl::_Shader* shader, const std::vector<std::string>& attribs, const std::vector<float>& data, opengl::_Image* image) : Buffer<_Lengths...>(shader, attribs, data)
		{
			m_Image = image;
		}

		/// <summary>Frees the image.</summary>
		~ImageBuffer()
		{
			delete m_Image;
		}

		/// <summary>Retrieves the width of the image.</summary>
		/// <returns>The width of the image included in the buffer.</returns>
		int get_width()
		{
			return m_Image->get_width();
		}

		/// <summary>Retrieves the height of the image.</summary>
		/// <returns>The height of the image included in the buffer.</returns>
		int get_height()
		{
			return m_Image->get_height();
		}
	};

}