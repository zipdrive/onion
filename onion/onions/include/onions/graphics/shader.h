#pragma once
#include <vector>
#include <unordered_map>
#include "../error.h"
#include "../fileio.h"
#include "../matrix.h"

#define BUFFER_KEY Int

namespace onion
{

	namespace opengl
	{

		/// <summary>Synchronizes the CPU with the GPU.</summary>
		void synchronize();


		// An untyped vertex attrib.
		struct _VertexAttrib
		{
			// The offset of this attrib from the beginning of the vertex, in bytes.
			const Uint offset;


			/// <summary>Constructs a vertex attrib.</summary>
			/// <param name="offset">The offset of this attrib from the beginning of the vertex, in bytes.</param>
			_VertexAttrib(Uint offset);
			
			/// <summary>Virtual deconstructor.</summary>
			virtual ~_VertexAttrib() = default;


			/// <summary>Retrieves the size of the attribute.</summary>
			/// <returns>The size of the attribute, in bytes.</returns>
			virtual Uint size() const = 0;

			/// <summary>Retrieves the number of primitives contained in the attribute.</summary>
			/// <returns>The number of primitives contained in the attribute.</returns>
			virtual Int count() const = 0;


			/// <summary>Sets the vertex attrib pointer.</summary>
			/// <param name="index">The index of the vertex attrib pointer.</param>
			/// <param name="stride">The stride between each vertex.</param>
			virtual void set(Uint index, Uint stride) const = 0;
		};

		// A typed vertex attrib.
		template <typename T>
		struct VertexAttrib : public _VertexAttrib
		{
			/// <summary>Constructs a typed vertex attrib.</summary>
			/// <param name="type">The GLenum value associated with the type.</param>
			/// <param name="offset">The offset of this attrib from the beginning of the vertex, in bytes.</param>
			VertexAttrib(Uint offset);


			/// <summary>Retrieves the size of the attribute.</summary>
			/// <returns>The size of the attribute, in bytes.</returns>
			Uint size() const;

			/// <summary>Retrieves the number of primitives contained in the attribute.</summary>
			/// <returns>The number of primitives contained in the attribute.</returns>
			Int count() const;


			/// <summary>Sets the vertex attrib pointer.</summary>
			/// <param name="index">The index of the vertex attrib pointer.</param>
			/// <param name="stride">The stride between each vertex.</param>
			void set(Uint index, Uint stride) const;
		};

		struct VertexAttribs
		{
			// All vertex attribs, in order.
			std::vector<_VertexAttrib*> attribs;

			/// <summary>Deletes all vertex attribs.</summary>
			~VertexAttribs();

			/// <summary>Pushes a vertex attrib onto the back of the list of vertex attribs.</summary>
			/// <param name="type">The GLenum value associated with the type.</param>
			void push(Uint type);

			/// <summary>Enables the vertex attribs.</summary>
			void enable() const;

			/// <summary>Disables the vertex attribs.</summary>
			void disable() const;
		};
		
		
		
		// Predeclaration of the ID object.
		struct _ID;


		// The data passed to a vertex buffer.
		class _VertexBufferData
		{
		public:
			// An untyped attribute for the vertex buffer.
			struct _Attribute
			{
				/// <summary>Retrieves the size of the attribute.</summary>
				/// <returns>The size of the attribute, in bytes.</returns>
				virtual std::size_t size() const = 0;

				/// <summary>Assigns the value of the attribute to a location in the buffer.</summary>
				/// <param name="ptr">A pointer to the beginning of where to assign the value. Outputs a pointer to the byte after the value.</param>
				virtual void assign(void*& ptr) const = 0;

				/// <summary>Clones the attribute.</summary>
				/// <returns>A pointer to a copy of this attribute.</returns>
				virtual _Attribute* clone() const = 0;
			};

			// A typed attribute for the vertex buffer.
			template <typename _Attrib>
			struct Attribute : public _Attribute
			{
				/// <summary>Retrieves the size of the attribute.</summary>
				/// <returns>The size of the attribute, in bytes.</returns>
				static constexpr std::size_t attr_size()
				{
					return type_size<_Attrib>::whole;
				}

				// The value of the attribute.
				_Attrib value;

				/// <summary>Default constructor.</summary>
				Attribute() = default;

				/// <summary>Constructs an attribute with a given value.</summary>
				/// <param name="other">The value of the attribute.</param>
				Attribute(const _Attrib& other) : value(other) {}

				/// <summary>Retrieves the size of the attribute.</summary>
				/// <returns>The size of the attribute, in bytes.</returns>
				std::size_t size() const
				{
					return attr_size();
				}

				/// <summary>Assigns the value of the attribute to a location in the buffer.</summary>
				/// <param name="ptr">A pointer to the beginning of where to assign the value. Outputs a pointer to the byte after the value.</param>
				void assign(void*& ptr) const
				{
					_Attrib* tptr = (_Attrib*)ptr;
					*tptr++ = value;
					ptr = tptr;
				}

				/// <summary>Clones the attribute.</summary>
				/// <returns>A pointer to a copy of this attribute.</returns>
				_Attribute* clone() const
				{
					return new Attribute<_Attrib>(value);
				}
			};

			// A matrix-typed attribute for the vertex buffer.
			template <typename _Number, int _Columns, int _Rows>
			struct Attribute<matrix<_Number, _Columns, _Rows>> : public _Attribute
			{
				/// <summary>Retrieves the size of the attribute.</summary>
				/// <returns>The size of the attribute, in bytes.</returns>
				static constexpr std::size_t attr_size()
				{
					return type_size<matrix<_Number, _Columns, _Rows>>::whole;
				}

				// The value of the attribute.
				matrix<_Number, _Columns, _Rows> value;

				/// <summary>Default constructor.</summary>
				Attribute() = default;

				/// <summary>Constructs an attribute with a given value.</summary>
				/// <param name="other">The value of the attribute.</param>
				Attribute(const matrix<_Number, _Columns, _Rows>& other) : value(other) {}

				/// <summary>Retrieves the size of the attribute.</summary>
				/// <returns>The size of the attribute, in bytes.</returns>
				std::size_t size() const
				{
					return attr_size();
				}

				/// <summary>Assigns the value of the attribute to a location in the buffer.</summary>
				/// <param name="ptr">A pointer to the beginning of where to assign the value. Outputs a pointer to the byte after the value.</param>
				void assign(void*& ptr) const
				{
					_Number* tptr = (_Number*)ptr;

					for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
						tptr[k] = value.get(k);

					ptr = tptr + (_Rows * _Columns);
				}

				/// <summary>Clones the attribute.</summary>
				/// <returns>A pointer to a copy of this attribute.</returns>
				_Attribute* clone() const
				{
					return new Attribute<matrix<_Number, _Columns, _Rows>>(value);
				}
			};


			/// <summary>Virtual deconstructor.</summary>
			virtual ~_VertexBufferData() = default;


			/// <summary>Retrieves the size of each vertex.</summary>
			/// <returns>The size of each vertex, in bytes.</returns>
			virtual std::size_t vertex_size() const = 0;

			/// <summary>Retrieves the number of vertices in the buffer.</summary>
			/// <returns>The number of vertices in the buffer.</returns>
			virtual std::size_t buffer_size() const = 0;


			/// <summary>Pushes new vertices to the back of the buffer.</summary>
			/// <param name="count">The number of new vertices to add.</param>
			/// <returns>The index of the first new vertex.</returns>
			virtual int push(int count = 1) = 0;

			/// <summary>Inserts new vertices at the specified index.</summary>
			/// <param name="index">The index to begin inserting new vertices at.</param>
			/// <param name="count">The number of new vertices to add.</param>
			/// <returns>The index of the first new vertex.</returns>
			virtual int insert(int index, int count = 1) = 0;

			/// <summary>Pops the vertices on the back of the buffer.</summary>
			/// <param name="count">The number of vertices to delete.</param>
			virtual void pop(int count = 1) = 0;


			/// <summary>Compiles the buffer of data.</summary>
			/// <param name="bytes">The size of the buffer, in bytes.</param>
			/// <returns>A pointer to the start of the buffer.</returns>
			virtual char* compile(std::size_t& bytes) const = 0;
		};



		// A uniform attribute.
		class _UniformAttribute
		{
		public:
			// The name of the uniform attribute.
			const std::string name;

			/// <summary>Constructs an object managing a uniform attribute.</summary>
			/// <param name="name">The name of the uniform attribute.</param>
			_UniformAttribute(std::string name);

			/// <summary>Virtual deconstructor.</summary>
			virtual ~_UniformAttribute();

			/// <summary>Retrieves the size of the uniform attribute.</summary>
			/// <returns>The size of the uniform attribute, in bytes.</returns>
			virtual unsigned int get_size() const = 0;
		};

		// A uniform attribute with a defined type.
		template <typename T>
		class _UniformTypedAttribute : public _UniformAttribute
		{
		public:
			/// <summary>Constructs an object managing a typed uniform attribute.</summary>
			/// <param name="name">The name of the uniform attribute.</param>
			_UniformTypedAttribute(std::string name);

			/// <summary>Retrieves the size of the uniform attribute.</summary>
			/// <returns>The size of the uniform attribute, in bytes.</returns>
			virtual unsigned int get_size() const;

			/// <summary>Sets the value of the uniform attribute.</summary>
			/// <param name="value">The new value of the uniform attribute.</param>
			virtual void set(const T& value) const = 0;
		};

		template <typename T>
		class _UniformBlockAttribute : public _UniformTypedAttribute<T>
		{
		protected:
			// The offset of the uniform within the block.
			const Int m_Offset;

		public:
			_UniformBlockAttribute(std::string name, Int offset);

			void set(const T& value) const;
		};



		// Handles all of the OpenGL calls for creating and using shader programs.
		class _Shader
		{
		private:
			// The ID of the currently active shader.
			static _ID* m_ActiveShader;

			// The ID of this shader.
			_ID* m_Shader;


			// A list of all vertex attributes.
			VertexAttribs m_VertexAttributes;

			/// <summary>Compiles the shader program from raw text.</summary>
			/// <param name="vertex_shader_text">The vertex shader, in raw text form.</param>
			/// <param name="fragment_shader_text">The fragment shader, in raw text form.</param>
			void compile(const char* vertex_shader_text, const char* fragment_shader_text, const std::vector<String>& uniform_names);
			
			/// <summary>Compiles the shader program from raw text.</summary>
			/// <param name="vertex_shader_text">The vertex shader, in raw text form.</param>
			/// <param name="geometry_shader_text">The geometry shader, in raw text form.</param>
			/// <param name="fragment_shader_text">The fragment shader, in raw text form.</param>
			void compile(const char* vertex_shader_text, const char* geometry_shader_text, const char* fragment_shader_text, const std::vector<String>& uniform_names);

			/// <summary>Processes information about vertex and uniform attributes from the compiled shader program.</summary>
			void process(const std::vector<String>& uniform_names);

		protected:
			// A list of all uniform attributes (that aren't in blocks).
			std::vector<_UniformAttribute*> m_UniformAttributes;


			/// <summary>Loads shaders from vertex and fragment shader files.</summary>
			/// <param name="path">The path to the vertex and fragment shader files, from the res/data/shaders/ folder, omitting file extensions.</param>
			_Shader(const char* path, const std::vector<String>& uniform_names);
			
			/// <summary>Constructs a shader from raw text.</summary>
			/// <param name="vertex_shader_text">The vertex shader, in raw text form.</param>
			/// <param name="fragment_shader_text">The fragment shader, in raw text form.</param>
			_Shader(const char* vertex_shader_text, const char* fragment_shader_text, const std::vector<String>& uniform_names);

			/// <summary>Activates the shader.</summary>
			void __activate() const;

		public:
			/// <summary>Destroys the shader.</summary>
			virtual ~_Shader();

			/// <summary>Checks whether this shader is the active one.</summary>
			/// <returns>True if this shader is active, false otherwise.</returns>
			bool is_active() const;

			/// <summary>Retrieves information about the vertex attributes of the shader program.</summary>
			/// <returns>Information about the vertex attributes.</returns>
			const VertexAttribs& get_attribs() const;
		};


		
		// Handles all of the OpenGL calls for constructing and binding uniform buffers.
		class _UniformBuffer
		{
		private:
			friend class _Shader; // Allows the shader to access the locations of the uniform buffers

			// The currently bound uniform buffer.
			static _ID* m_ActiveBuffer;

			// All uniform buffers.
			static std::unordered_map<std::string, _UniformBuffer*> m_Buffers;

			// The name of the buffer.
			std::string m_Name;

			// The ID of this buffer. Used to bind the buffer.
			_ID* m_Buffer;

			/// <summary>Binds this uniform buffer as the one being written to.</summary>
			void bind() const;


			// An object that represents a binding point that the buffer in memory.
			struct BindingPoint;

			// The binding point that the buffer is linked to. Used to connect the shaders to the uniform buffers they use.
			BindingPoint* m_BindingPoint;


			// The uniforms associated with the buffer.
			std::vector<_UniformAttribute*> m_Uniforms;

		public:
			/// <summary>Retrieves the uniform buffer with the given name.</summary>
			/// <param name="name">The name of the uniform buffer, as it is referred to in the shaders.</param>
			/// <returns>The uniform buffer with that name. NULL if the buffer does not exist.</returns>
			static _UniformBuffer* get_buffer(std::string name);


			/// <summary>Constructs an empty uniform buffer handler.</summary>
			/// <param name="name">The name of the uniform buffer, as it is referred to in the shaders.</param>
			_UniformBuffer(std::string name);

			/// <summary>Constructs a uniform buffer handler.</summary>
			/// <param name="name">The name of the uniform buffer, as it is referred to in the shaders.</param>
			/// <param name="uniforms">The list of uniform attributes in the block.</param>
			/// <param name="size">The size of the uniform buffer.</param>
			_UniformBuffer(std::string name, const std::vector<_UniformAttribute*>& uniforms, unsigned int size);

			/// <summary>Cleans up the buffer.</summary>
			virtual ~_UniformBuffer();

			
			/// <summary>Sets what uniforms are in the buffer.</summary>
			/// <param name="uniforms">The list of uniform attributes in the block.</param>
			/// <param name="size">The size of the uniform buffer.</param>
			void set_uniforms(const std::vector<_UniformAttribute*>& uniforms, unsigned int size);


			/// <summary>Sets the value of a uniform in the buffer.</summary>
			/// <param name="index">The index of the uniform.</param>
			/// <param name="uniform">The value of the uniform.</param>
			template <typename _Uniform>
			void set(int index, const _Uniform& uniform) const
			{
				bind();

				if (_UniformTypedAttribute<_Uniform>* u = dynamic_cast<_UniformTypedAttribute<_Uniform>*>(m_Uniforms[index]))
				{
					u->set(uniform);
				}
			}

			/// <summary>Sets the value of a uniform in the buffer.</summary>
			/// <param name="name">The name of the uniform.</param>
			/// <param name="uniform">The value of the uniform.</param>
			template <typename _Uniform>
			void set(std::string name, const _Uniform& uniform) const
			{
				bind();

				for (auto iter = m_Uniforms.begin(); iter != m_Uniforms.end(); ++iter)
				{
					if ((*iter)->name.compare(name) == 0)
					{
						if (_UniformTypedAttribute<_Uniform>* u = dynamic_cast<_UniformTypedAttribute<_Uniform>*>(*iter))
						{
							u->set(uniform);
						}
					}
				}
			}
		};


		// Handles all of the OpenGL calls for constructing and binding vertex attribute buffers.
		class _VertexBuffer
		{
		private:
			// The ID of the currently active buffer.
			static _ID* m_ActiveBuffer;

			// The ID of this buffer.
			_ID* m_Buffer;

			// The ID of the VAO containing buffer information.
			_ID* m_VAO;

		protected:
			/// <summary>Activates anything else that needs to be activated.</summary>
			virtual void __activate() const;

		public:
			/// <summary>Constructs a buffer from data.</summary>
			/// <param name="data">The data to use in the buffer.</param>
			_VertexBuffer(const _VertexBufferData* data, const VertexAttribs& attribs);

			/// <summary>Frees the buffer from memory.</summary>
			virtual ~_VertexBuffer();

			/// <summary>Checks whether this buffer is the active one.</summary>
			/// <returns>True if this buffer is active, false otherwise.</returns>
			bool is_active() const;

			/// <summary>Activates the buffer.</summary>
			void activate() const;
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
			int get_width() const;

			/// <summary>Retrieves the height of the image.</summary>
			/// <returns>The height of the image, in pixels.</returns>
			int get_height() const;

			/// <summary>Checks whether this buffer is the active one.</summary>
			/// <returns>True if this buffer is active, false otherwise.</returns>
			bool is_active() const;

			/// <summary>Activates the buffer and binds it to the n-th texture slot.</summary>
			void activate(int slot = 0) const;
		};



		// Handles all calls to display something using information from a buffer.
		class _VertexBufferDisplayer
		{
		protected:
			// The buffer to display from.
			_VertexBuffer* m_Buffer = nullptr;

		public:
			/// <summary>Displays using the currently bound shader and information from the buffer.</summary>
			/// <param name="start">A pointer to the start location in the buffer.
			/// This should be equal to the index of the starting vertex in the buffer array.</param>
			/// <param name="count">The number of sequential shapes to display.</param>
			virtual void display(BUFFER_KEY start, int count = 1) const = 0;

			/// <summary>Sets the buffer to use.</summary>
			/// <param name="buffer">The new buffer to use.</param>
			void set_buffer(_VertexBuffer* buffer);
		};

		// Displays two consecutive triangles in the buffer.
		class _SquareBufferDisplayer : public _VertexBufferDisplayer
		{
		public:
			/// <summary>Displays using the currently bound shader and information from the buffer.</summary>
			/// <param name="start">A pointer to the start location in the buffer.
			/// This should be equal to the index of the starting vertex in the buffer array.</param>
			/// <param name="count">The number of sequential shapes to display.</param>
			virtual void display(BUFFER_KEY start, Int count = 1) const;
		};

	}


	// Strictly typed data for a vertex buffer.
	template <typename... _Attribs>
	class VertexBufferData : public opengl::_VertexBufferData
	{
	public:
		template <int N>
		using nth_t = std::tuple_element_t<N, std::tuple<_Attribs...>>;

	private:
		using vertex_t = std::tuple<Attribute<_Attribs>...>;

		// The data for every vertex in the buffer.
		std::vector<vertex_t> m_Vertices;

		template <typename _NthAttrib, typename... _RemainingAttribs>
		struct VertexSizeGetter
		{
			/// <summary>Retrieves the total size of the n-th attribute and beyond.</summary>
			/// <returns>The size of the n-th attribute and beyond, in bytes.</returns>
			static std::size_t size()
			{
				return Attribute<_NthAttrib>::attr_size();
			}
		};

		template <typename _NthAttrib, typename _NthPlusOneAttrib, typename... _RemainingAttribs>
		struct VertexSizeGetter<_NthAttrib, _NthPlusOneAttrib, _RemainingAttribs...>
		{
			/// <summary>Retrieves the total size of the n-th attribute and beyond.</summary>
			/// <returns>The size of the n-th attribute and beyond, in bytes.</returns>
			static std::size_t size()
			{
				return Attribute<_NthAttrib>::attr_size() + VertexSizeGetter<_NthPlusOneAttrib, _RemainingAttribs...>::size();
			}
		};

		/// <summary>Assigns the n-th element in the vertex and every element after it to a location in the buffer.</summary>
		/// <param name="ptr">A pointer to the beginning of where to assign the values. Outputs a pointer to the byte after the vertex.</param>
		/// <param name="vertex">The vertex to assign.</param>
		template <int N>
		void __assign(void*& ptr, const vertex_t& vertex) const
		{
			std::get<N>(vertex).assign(ptr);
			__assign<N + 1>(ptr, vertex);
		}

		/// <summary>The end condition for the template recursion.</summary>
		/// <param name="ptr">A pointer to the beginning of where to assign the values. Outputs a pointer to the byte after the vertex.</param>
		/// <param name="vertex">The vertex to assign.</param>
		template <>
		void __assign<sizeof...(_Attribs)>(void*& ptr, const vertex_t& vertex) const {}

		/// <summary>Assigns the values of each attribute in the vertex to a location in the buffer.</summary>
		/// <param name="ptr">A pointer to the beginning of where to assign the values. Outputs a pointer to the byte after the vertex.</param>
		/// <param name="vertex">The vertex to assign.</param>
		void assign(void*& ptr, const vertex_t& vertex) const
		{
			__assign<0>(ptr, vertex);
		}

	public:
		/// <summary>Retrieves the size of each vertex.</summary>
		/// <returns>The size of each vertex, in bytes.</returns>
		std::size_t vertex_size() const
		{
			return VertexSizeGetter<_Attribs...>::size();
		}

		/// <summary>Retrieves the number of vertices in the buffer.</summary>
		/// <returns>The number of vertices in the buffer.</returns>
		std::size_t buffer_size() const
		{
			return m_Vertices.size();
		}

		/// <summary>Retrieves the n-th element of the vertex at the specified index.</summary>
		/// <param name="index">The index of the vertex.</param>
		template <int N>
		const nth_t<N>& get(int index) const
		{
			return std::get<N>(m_Vertices[index]).value;
		}

		/// <summary>Assigns a value to the n-th element of the vertex at the specified index.</summary>
		/// <param name="index">The index of the vertex.</param>
		/// <param name="value">The value to assign to the n-th element of the vertex.</param>
		template <int N>
		void set(int index, const nth_t<N>& value)
		{
			std::get<N>(m_Vertices[index]).value = value;
		}

		/// <summary>Pushes new vertices to the back of the buffer.</summary>
		/// <param name="count">The number of new vertices to add.</param>
		/// <returns>The index of the first new vertex.</returns>
		int push(int count = 1)
		{
			int index = m_Vertices.size();
			m_Vertices.resize(index + count);
			return index;
		}

		/// <summary>Inserts new vertices at the specified index.</summary>
		/// <param name="index">The index to begin inserting new vertices at.</param>
		/// <param name="count">The number of new vertices to add.</param>
		/// <returns>The index of the first new vertex.</returns>
		int insert(int index, int count = 1)
		{
			m_Vertices.resize(m_Vertices.size() + count);
			for (int k = m_Vertices.size() - 1; k >= index + count; --k)
				m_Vertices[k] = m_Vertices[k - count];
			return index;
		}

		/// <summary>Pops the vertices on the back of the buffer.</summary>
		/// <param name="count">The number of vertices to delete.</param>
		void pop(int count = 1)
		{
			m_Vertices.pop_back();
		}

		/// <summary>Compiles the buffer of data into an array.</summary>
		/// <param name="bytes">The size of the buffer, in bytes.</param>
		/// <returns>A pointer to the start of the buffer.</returns>
		char* compile(std::size_t& bytes) const
		{
			bytes = vertex_size() * m_Vertices.size();
			char* buf = new char[bytes];

			void* ptr = buf;
			for (auto iter = m_Vertices.begin(); iter != m_Vertices.end(); ++iter)
			{
				assign(ptr, *iter);
			}

			return buf;
		}
	};


	// A vertex buffer that also manages an image that should be bound to the 0-index slot.
	class ImageBuffer : public opengl::_VertexBuffer
	{
	private:
		// The image in the buffer.
		opengl::_Image* m_Image;

	protected:
		/// <summary>Binds the buffers and the image.</summary>
		virtual void __activate() const;

	public:
		/// <summary>Constructs a buffer that also manages a image.</summary>
		/// <param name="data">The data to fill the buffer with.</param>
		/// <param name="attribs">The information about the vertex attributes in the buffer.</param>
		/// <param name="image">The image to be managed by the buffer.</param>
		ImageBuffer(const opengl::_VertexBufferData* data, const opengl::VertexAttribs& attribs, opengl::_Image* image);

		/// <summary>Frees the image.</summary>
		~ImageBuffer();

		/// <summary>Retrieves the width of the image.</summary>
		/// <returns>The width of the image included in the buffer.</returns>
		int get_width() const;

		/// <summary>Retrieves the height of the image.</summary>
		/// <returns>The height of the image included in the buffer.</returns>
		int get_height() const;
	};


	// A wrapper for a shader program.
	template <typename... _Uniforms>
	class Shader : public opengl::_Shader
	{
	private:
		/// <summary>The end condition for the recursion.</summary>
		void set_uniforms(std::vector<opengl::_UniformAttribute*>::const_iterator iter) {}

		/// <summary>Sets the values of each uniform variable recursively.</summary>
		/// <param name="iter">An iterator pointing to the current uniform.</param>
		/// <param name="first">The value of the uniform currently being set.</param>
		/// <param name="others">The remaining uniform variables.</param>
		template <typename _FirstUniform, typename... _OtherUniforms>
		void set_uniforms(std::vector<opengl::_UniformAttribute*>::const_iterator iter, const _FirstUniform& first, const _OtherUniforms&... others)
		{
			if (opengl::_UniformTypedAttribute<_FirstUniform>* u = dynamic_cast<opengl::_UniformTypedAttribute<_FirstUniform>*>(*iter))
			{
				u->set(first);
			}
			else
			{
				errlog("ONION: Type of shader uniform does not correspond to template argument.\n");
			}

			set_uniforms(++iter, others...);
		}

	public:
		/// <summary>Loads shaders from a file.</summary>
		/// <param name="path">The path to the vertex and fragment shader files, from the res/shaders/ folder, omitting file extensions.</param>
		Shader(const char* path, const std::vector<String>& uniform_names) : opengl::_Shader(path, uniform_names)
		{
			if (sizeof...(_Uniforms) != m_UniformAttributes.size())
			{
				std::string message("Error loading shaders from path \"");
				message.append(path).append("\".\n");
				message.append("Mismatch between template parameter count of Shader (").append(std::to_string(sizeof...(_Uniforms)))
					.append(") and number of uniform attributes detected (").append(std::to_string(m_UniformAttributes.size())).append(").");
				errlog(message);
				// TODO abort
			}
		}

		/// <summary>Constructs a shader program.</summary>
		/// <param name="vertex_shader_text">The vertex shader, in raw text form.</param>
		/// <param name="fragment_shader_text">The fragment shader, in raw text form.</param>
		Shader(const char* vertex_shader_text, const char* fragment_shader_text, const std::vector<String>& uniform_names) : opengl::_Shader(vertex_shader_text, fragment_shader_text, uniform_names)
		{
			if (sizeof...(_Uniforms) != m_UniformAttributes.size() + 1)
			{
				std::string message("Error loading shaders from raw text.\n\n--- VERTEX ---\n");
				message.append(vertex_shader_text).append("\n\n--- FRAGMENT ---\n").append(fragment_shader_text).append("\n------------\n");
				message.append("Mismatch between template parameter count of Shader (").append(std::to_string(sizeof...(_Uniforms)))
					.append(") and number of uniform attributes detected (").append(std::to_string(m_UniformAttributes.size())).append(").");
				errlog(message);
				// TODO abort
			}
		}

		/// <summary>Activates the shader program and sets any uniform variables.</summary>
		/// <param name="uniforms">The values of the uniform variables.</param>
		void activate(const _Uniforms&... uniforms)
		{
			opengl::_Shader::__activate();
			set_uniforms(m_UniformAttributes.begin(), uniforms...);
		}
	};



}