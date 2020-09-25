#include <regex>
#include <filesystem>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <SOIL.h>
#include "../../../include/onions/error.h"
#include "../../../include/onions/matrix.h"
#include "../../../include/onions/graphics/sprite.h"

namespace onion
{
	namespace opengl
	{


		/// <summary>Checks for any OpenGL errors. If any were received, logs them.</summary>
		/// <param name="message">The header written before writing the OpenGL error codes.</param>
		void errcheck(std::string message)
		{
			if (GLenum errcode = glGetError())
			{
				// Log the header message
				errlog("ONION: " + message + "\n");

				// Log OpenGL error codes until there are none left
				do
				{
					errlog(std::string("  OpenGL error message received: ") + std::to_string(errcode) + "\n");
				} 
				while (errcode = glGetError());

				// Add a newline afterwards.
				errlog("\n");
			}
		}



		struct _ID
		{
			GLuint id;

			/// <summary>Constructs the shader ID.</summary>
			/// <param name="id">The OpenGL ID for the shader program.</param>
			_ID(GLuint id) : id(id) {}

			/// <summary>Checks if the ID is equal to another ID.</summary>
			bool operator==(const _ID& other)
			{
				return id == other.id;
			}
		};

		_ID* _Shader::m_ActiveShader{ nullptr };
		_ID* _VertexBuffer::m_ActiveBuffer{ nullptr };
		_ID* _Image::m_ActiveImage{ nullptr };


		struct _UniformBuffer::BindingPoint
		{
		private:
			static GLuint m_NextAvailableBindingPoint;

		public:
			// The binding point
			GLuint binding;

			BindingPoint() : binding(m_NextAvailableBindingPoint++) {}
		};

		GLuint _UniformBuffer::BindingPoint::m_NextAvailableBindingPoint{ 0 };



		template <typename T>
		unsigned int get_byte_count()
		{
			return 0;
		}

		template <> unsigned int get_byte_count<float>() { return 4; }
		template <> unsigned int get_byte_count<vec2f>() { return 8; }
		template <> unsigned int get_byte_count<vec3f>() { return 12; }
		template <> unsigned int get_byte_count<vec4f>() { return 16; }
		template <> unsigned int get_byte_count<mat2f>() { return 16; }
		template <> unsigned int get_byte_count<mat3f>() { return 36; }
		template <> unsigned int get_byte_count<mat4f>() { return 64; }
		template <> unsigned int get_byte_count<mat2x4f>() { return 32; }



		class _VertexAttributeLocation : public _VertexAttribute
		{
		private:
			// The location of the vertex attribute.
			GLuint m_Location;

		public:
			_VertexAttributeLocation(GLuint location, GLuint size)
			{
				m_Location = location;
				m_Size = size;
			}

			void set(unsigned int offset, unsigned int stride) const
			{
				glEnableVertexAttribArray(m_Location);
				glVertexAttribPointer(m_Location, m_Size, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * offset));
			}
		};

		_VertexAttributeInformation::~_VertexAttributeInformation()
		{
			for (auto iter = attributes.begin(); iter != attributes.end(); ++iter)
				delete iter->attrib;
		}



		_UniformAttribute::_UniformAttribute(std::string name) : name(name) {}
		_UniformAttribute::~_UniformAttribute() {}
		
		// The location of a uniform within a shader program.
		template <typename T>
		class _UniformProgramAttribute : public _UniformAttribute
		{
		private:
			// The location within the shader program.
			GLint m_Location;

		public:
			_UniformProgramAttribute(std::string name, GLint location) : _UniformAttribute(name)
			{
				m_Location = location;
			}

			unsigned int get_size() const
			{
				return get_byte_count<T>();
			}

			void set(float value) const {}

			void set(const vec2f& value) const {}
			void set(const vec3f& value) const {}
			void set(const vec4f& value) const {}

			void set(const mat2f& value) const {}
			void set(const mat3f& value) const {}
			void set(const mat4f& value) const {}
			void set(const mat2x4f& value) const {}

			void set(const MatrixStack& value) const {}
		};

		template <>
		void _UniformProgramAttribute<float>::set(float value) const
		{
			glUniform1f(m_Location, value);
		}

		template <>
		void _UniformProgramAttribute<vec2f>::set(const vec2f& value) const
		{
			glUniform2fv(m_Location, 1, value.matrix_values());
		}

		template <>
		void _UniformProgramAttribute<vec3f>::set(const vec3f& value) const
		{
			glUniform3fv(m_Location, 1, value.matrix_values());
		}

		template <>
		void _UniformProgramAttribute<vec4f>::set(const vec4f& value) const
		{
			glUniform4fv(m_Location, 1, value.matrix_values());
		}

		template <>
		void _UniformProgramAttribute<mat2f>::set(const mat2f& value) const
		{
			glUniformMatrix2fv(m_Location, 1, GL_FALSE, value.matrix_values());
		}

		template <>
		void _UniformProgramAttribute<mat3f>::set(const mat3f& value) const
		{
			glUniformMatrix3fv(m_Location, 1, GL_FALSE, value.matrix_values());
		}

		template <>
		void _UniformProgramAttribute<mat4f>::set(const mat4f& value) const
		{
			glUniformMatrix4fv(m_Location, 1, GL_FALSE, value.matrix_values());
		}

		template <>
		void _UniformProgramAttribute<mat2x4f>::set(const mat2x4f& value) const
		{
			glUniformMatrix4x2fv(m_Location, 1, GL_FALSE, value.matrix_values());
		}

		template <>
		void _UniformProgramAttribute<TRANSFORM_MATRIX>::set(const MatrixStack& value) const
		{
			glUniformMatrix4fv(m_Location, 1, GL_FALSE, value.get_values());
		}


		template <typename T>
		class _UniformBlockAttribute : public _UniformAttribute
		{
		protected:
			// The offset of the uniform within the block.
			const GLint m_Offset;

		public:
			_UniformBlockAttribute(std::string name, GLint offset) : _UniformAttribute(name), m_Offset(offset) {}

			unsigned int get_size() const
			{
				return get_byte_count<T>();
			}

			void set(float value) const {}

			void set(const vec2f& value) const {}
			void set(const vec3f& value) const {}
			void set(const vec4f& value) const {}

			void set(const mat2f& value) const {}
			void set(const mat3f& value) const {}
			void set(const mat4f& value) const {}
			void set(const mat2x4f& value) const {}

			void set(const MatrixStack& value) const {}
		};

		template <>
		void _UniformBlockAttribute<float>::set(float value) const
		{
			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, 4, &value);
		}

		template <>
		void _UniformBlockAttribute<vec2f>::set(const vec2f& value) const
		{
			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, 8, value.matrix_values());
		}

		template <>
		void _UniformBlockAttribute<vec2f>::set(const vec3f& value) const
		{
			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, 16, value.matrix_values());
		}

		template <>
		void _UniformBlockAttribute<vec2f>::set(const vec4f& value) const
		{
			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, 16, value.matrix_values());
		}

		template <>
		void _UniformBlockAttribute<mat4f>::set(const mat4f& value) const
		{
			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, 64, value.matrix_values());
		}

		template <>
		void _UniformBlockAttribute<mat2x4f>::set(const mat2x4f& value) const
		{
			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, 32, value.matrix_values());
		}

		template <>
		void _UniformBlockAttribute<mat4f>::set(const MatrixStack& value) const
		{
			/*
			const float* mat = value.get_values();
			GLsizeiptr size = sizeof(float) * 16;

			GLint buffer_size = 0;
			glGetBufferParameteriv(GL_UNIFORM_BUFFER, GL_BUFFER_SIZE, &buffer_size);
			errcheck("Error retrieving the size of the buffer for the uniform block.");

			GLvoid* ptr = glMapBufferRange(GL_UNIFORM_BUFFER, m_Offset, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
			errcheck("Error received when mapping a range from the buffer for the uniform block.");
			memcpy(ptr, mat, sizeof(float) * 16);
			glUnmapBuffer(GL_UNIFORM_BUFFER);
			*/

			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, 64, value.get_values());
		}


		template <typename T, bool _IsRowMajor>
		class _UniformBlockMatrixAttribute : public _UniformBlockAttribute<T>
		{
		protected:
			// The stride between columns of the matrix.
			const GLint m_MatrixStride;

		public:
			_UniformBlockMatrixAttribute(std::string name, GLint offset, GLint matrix_stride) : _UniformBlockAttribute<T>(name, offset), m_MatrixStride(matrix_stride) {}

			void set(const mat2f& value) const {}
			void set(const mat3f& value) const {}
			void set(const mat4f& value) const {}
			void set(const mat2x4f& value) const {}

			void set(const MatrixStack& value) const {}
		};

		template <>
		void _UniformBlockMatrixAttribute<mat2f, false>::set(const mat2f& value) const
		{
			int size = (m_MatrixStride / 4) + 2;
			float* data = new float[size];
			data[0] = value.get(0);
			data[1] = value.get(1);
			data[m_MatrixStride + 0] = value.get(2);
			data[m_MatrixStride + 1] = value.get(3);

			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, size * 4, value.matrix_values());
		}

		template <>
		void _UniformBlockMatrixAttribute<mat3f, false>::set(const mat3f& value) const
		{
			int size = (m_MatrixStride / 2) + 3;
			float* data = new float[size];
			data[0] = value.get(0);
			data[1] = value.get(1);
			data[2] = value.get(2);
			data[m_MatrixStride + 0] = value.get(3);
			data[m_MatrixStride + 1] = value.get(4);
			data[m_MatrixStride + 2] = value.get(5);
			data[(2 * m_MatrixStride) + 0] = value.get(6);
			data[(2 * m_MatrixStride) + 1] = value.get(7);
			data[(2 * m_MatrixStride) + 2] = value.get(8);

			glBufferSubData(GL_UNIFORM_BUFFER, m_Offset, size * 4, value.matrix_values());
		}



		_Shader::_Shader(const char* path)
		{
			std::string vertex_shader, fragment_shader;

			// Load the files of the shaders
			std::string fpath("res/data/shaders/");
			fpath += path;

			LoadFile vertex(fpath + ".vertex");
			LoadFile fragment(fpath + ".fragment");

			// Collect the text of each shader in a string
			while (vertex.good())
				vertex_shader += vertex.load_line() + "\n";

			while (fragment.good())
				fragment_shader += fragment.load_line() + "\n";

			// Compile the shaders
			compile(vertex_shader.c_str(), fragment_shader.c_str());
		}

		_Shader::_Shader(const char* vertex_shader_text, const char* fragment_shader_text)
		{
			compile(vertex_shader_text, fragment_shader_text);
		}
		
		void _Shader::compile(const char* vertex_shader_text, const char* fragment_shader_text)
		{
			errcheck("Error received at some point before beginning shader compilation.");

#define SHADER_INFO_BUFFER_SIZE 500
			GLint success; // Retrieves whether compilation was a success or failure.
			GLchar info_buffer[SHADER_INFO_BUFFER_SIZE]; // Buffer in case compilation/linking fails.

			// Vertex shader
			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
			errcheck("Error received when setting the source text for the vertex shader.");
			glCompileShader(vertex_shader);
			errcheck("Error received when issuing the instruction to compile the vertex shader.");
			// If compilation failed, log the error
			glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
			errcheck("Error received when checking the compilation status of the vertex shader.");
			if (!success)
			{
				glGetShaderInfoLog(vertex_shader, SHADER_INFO_BUFFER_SIZE, NULL, info_buffer);
				errlog(std::string("ONION: Error received when compiling the vertex shader.\n") + info_buffer + "\n\n");
				errabort(ERROR_FAILED_TO_COMPILE_VERTEX_SHADER);
			}

			// Fragment shader
			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
			errcheck("Error received when setting the source text for the fragment shader.");
			glCompileShader(fragment_shader);
			errcheck("Error received when issuing the instruction to compile the fragment shader.");
			// If compilation failed, log the error
			glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
			errcheck("Error received when checking the compilation status of the fragment shader.");
			if (!success)
			{
				glGetShaderInfoLog(fragment_shader, SHADER_INFO_BUFFER_SIZE, NULL, info_buffer);
				errlog(std::string("ONION: Error received when compiling the fragment shader.\n") + info_buffer + "\n\n");
				errabort(ERROR_FAILED_TO_COMPILE_FRAGMENT_SHADER);
			}

			// Create shader program
			GLuint id = glCreateProgram();
			glAttachShader(id, vertex_shader);
			errcheck("Error received when attaching vertex shader to shader program.");
			glAttachShader(id, fragment_shader);
			errcheck("Error received when attaching fragment shader to shader program.");
			glLinkProgram(id);
			errcheck("Error received when issuing the instruction to link the shader program.");
			m_Shader = new _ID(id);
			// If linking failed, log the error
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			errcheck("Error received when checking the link status of the shader program.");
			if (!success)
			{
				glGetProgramInfoLog(id, SHADER_INFO_BUFFER_SIZE, NULL, info_buffer);
				errlog(std::string("ONION: Error received when linking the shader program.\n") + info_buffer + "\n\n");
				errabort(ERROR_FAILED_TO_LINK_SHADER_PROGRAM);
			}

			// Delete the shaders
			glDeleteShader(vertex_shader);
			errcheck("Error received when deleting the vertex shader after linking it to the shader program.");
			glDeleteShader(fragment_shader);
			errcheck("Error received when deleting the fragment shader after linking it to the shader program.");

			// Determine the shader program's vertex attributes
			GLint vertex_attribute_count;
			glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &vertex_attribute_count);
			errcheck("Error received when checking number of vertex attributes.");

			if (vertex_attribute_count >= 0)
			{
				m_VertexAttributes.attributes.resize(vertex_attribute_count);
				m_VertexAttributes.stride = 0;

				for (GLuint index = 0; index < vertex_attribute_count; ++index)
				{
					// Calculate the size of the vertex attribute
					GLint size;
					GLenum type;
					glGetActiveAttrib(id, index, 0, NULL, &size, &type, NULL);
					errcheck("Error received when retrieving information about the vertex attribute with index " + std::to_string(index) + ".");

					switch (type)
					{
					case GL_FLOAT:
						size *= 1;
						break;
					case GL_FLOAT_VEC2:
						size *= 2;
						break;
					case GL_FLOAT_VEC3:
						size *= 3;
						break;
					case GL_FLOAT_VEC4:
					case GL_FLOAT_MAT2:
						size *= 4;
						break;
					case GL_FLOAT_MAT2x3:
					case GL_FLOAT_MAT3x2:
						size *= 6;
						break;
					case GL_FLOAT_MAT2x4:
					case GL_FLOAT_MAT4x2:
						size *= 8;
						break;
					case GL_FLOAT_MAT3:
						size *= 9;
						break;
					case GL_FLOAT_MAT3x4:
					case GL_FLOAT_MAT4x3:
						size *= 12;
						break;
					case GL_FLOAT_MAT4:
						size *= 16;
						break;
					default:
						size = 0;
						break;
					}

					m_VertexAttributes.attributes[index].attrib = new _VertexAttributeLocation(index, size);
					m_VertexAttributes.attributes[index].offset = m_VertexAttributes.stride;
					m_VertexAttributes.stride += size;
				}
			}

			// Determine and construct the uniform blocks that the shader program uses
			GLint uniform_block_count;
			glGetProgramiv(id, GL_ACTIVE_UNIFORM_BLOCKS, &uniform_block_count);

			for (GLuint uniform_block_index = 0; uniform_block_index < uniform_block_count; ++uniform_block_index)
			{
				// Get the name of the uniform block
				GLint uniform_block_name_length;
				glGetActiveUniformBlockiv(id, uniform_block_index, GL_UNIFORM_BLOCK_NAME_LENGTH, &uniform_block_name_length);

				GLchar* raw_uniform_block_name = new GLchar[uniform_block_name_length];
				glGetActiveUniformBlockName(id, uniform_block_index, uniform_block_name_length, NULL, raw_uniform_block_name);
				std::string uniform_block_name(raw_uniform_block_name);
				delete[] raw_uniform_block_name;

				_UniformBuffer* buf = _UniformBuffer::get_buffer(uniform_block_name);
				if (!buf) // If the buffer does not exist, construct the uniform block
				{
					// Get each uniform in the block
					GLint buf_size;
					glGetActiveUniformBlockiv(id, uniform_block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &buf_size);

					GLint* buf_uniforms = new GLint[buf_size];
					glGetActiveUniformBlockiv(id, uniform_block_index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, buf_uniforms);
					GLuint* buf_uniform_indices = (GLuint*)buf_uniforms;

					// Get the type of each uniform in the block
					GLint* types = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_TYPE, types);

					// Get the offset of each uniform in the block
					GLint* offsets = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_OFFSET, offsets);

					// Get the size and stride of the array for each uniform in the block
					GLint* array_sizes = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_SIZE, array_sizes);

					GLint* array_strides = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_ARRAY_STRIDE, array_strides);

					// Get the length of each uniform's name
					GLint* uniform_name_lengths = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_NAME_LENGTH, uniform_name_lengths);

					// Create a uniform attribute for each uniform in the block
					std::vector<_UniformAttribute*> uniforms;
					unsigned int total_uniform_block_size = 0;
					for (int i = 0; i < buf_size; ++i)
					{
						std::string uniform_name;
						if (uniform_name_lengths[i] > 0)
						{
							GLchar* raw_uniform_name = new GLchar[uniform_name_lengths[i]];
							glGetActiveUniformName(id, buf_uniform_indices[i], uniform_name_lengths[i], NULL, raw_uniform_name);
							uniform_name = std::string(raw_uniform_name);
							delete[] raw_uniform_name;
						}

						GLint offset = offsets[i];
						GLenum type = types[i];

						GLint array_size = array_sizes[i];
						for (int n = 0; n < array_size; ++n)
						{
							_UniformAttribute* u = nullptr;

							std::string uname(uniform_name);
							if (array_size > 1)
								uname += "[" + std::to_string(n) + "]";

							if (type == GL_FLOAT)
							{
								u = new _UniformBlockAttribute<float>(uname, offset);
							}
							else if (type == GL_FLOAT_VEC2)
							{
								u = new _UniformBlockAttribute<vec2f>(uname, offset);
							}
							else if (type == GL_FLOAT_VEC3)
							{
								u = new _UniformBlockAttribute<vec3f>(uname, offset);
							}
							else if (type == GL_FLOAT_VEC4)
							{
								u = new _UniformBlockAttribute<vec4f>(uname, offset);
							}
							else
							{
								if (type == GL_FLOAT_MAT4)
								{
									u = new _UniformBlockAttribute<mat4f>(uname, offset);
								}
								else if (type == GL_FLOAT_MAT4x2)
								{
									u = new _UniformBlockAttribute<mat2x4f>(uname, offset);
								}
							}

							uniforms.push_back(u);

							total_uniform_block_size = std::max(total_uniform_block_size, offset + u->get_size());
							offset += array_strides[i];
						}

					}

					// Construct the uniform block
					buf = new _UniformBuffer(uniform_block_name, uniforms, total_uniform_block_size);

					// Clean up all the arrays we allocated earlier
					delete[] buf_uniforms;
					delete[] types;
					delete[] offsets;
					delete[] array_sizes;
					delete[] array_strides;
					delete[] uniform_name_lengths;
				}

				// Bind the shader's uniform block to a uniform buffer
				glUniformBlockBinding(id, uniform_block_index, buf->m_BindingPoint->binding);
			}

			// Determine any other uniforms
			GLint uniform_count;
			glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniform_count);

			for (unsigned int uniform_index = 0; uniform_index < uniform_count; ++uniform_index)
			{
				GLint uniform_block_index;
				glGetActiveUniformsiv(id, 1, &uniform_index, GL_UNIFORM_BLOCK_INDEX, &uniform_block_index);

				if (uniform_block_index < 0) // Uniform doesn't belong to a uniform block
				{
					// Retrieve the name of the uniform
					GLint uniform_name_length;
					glGetActiveUniformsiv(id, 1, &uniform_index, GL_UNIFORM_NAME_LENGTH, &uniform_name_length);

					std::string uniform_name;
					if (uniform_name_length > 0)
					{
						GLchar* name_buf = new GLchar[uniform_name_length];
						glGetActiveUniformName(id, uniform_index, uniform_name_length, NULL, name_buf);
						uniform_name = std::string(name_buf);
						delete[] name_buf;
					}

					// Retrieve the location of the uniform
					GLint uniform_location = glGetUniformLocation(id, uniform_name.c_str());

					// Retrieve the info associated with the uniform
					GLint size;
					GLenum type;
					glGetActiveUniform(id, uniform_index, 0, NULL, &size, &type, NULL);

					// Construct a uniform object
					_UniformAttribute* u = nullptr;

					switch (type)
					{
					case GL_FLOAT:
						u = new _UniformProgramAttribute<float>(uniform_name, uniform_location);
						break;
					case GL_FLOAT_VEC2:
						u = new _UniformProgramAttribute<vec2f>(uniform_name, uniform_location);
						break;
					case GL_FLOAT_VEC3:
						u = new _UniformProgramAttribute<vec3f>(uniform_name, uniform_location);
						break;
					case GL_FLOAT_VEC4:
						u = new _UniformProgramAttribute<vec4f>(uniform_name, uniform_location);
						break;
					case GL_FLOAT_MAT4:
						u = new _UniformProgramAttribute<mat4f>(uniform_name, uniform_location);
						break;
					case GL_FLOAT_MAT4x2:
						u = new _UniformProgramAttribute<mat2x4f>(uniform_name, uniform_location);
						break;
					}

					if (u)
						m_UniformAttributes.push_back(u);
				}
			}
		}

		_Shader::~_Shader()
		{
			// If active, reset the active shader program
			if (m_ActiveShader == m_Shader)
				m_ActiveShader = nullptr;

			// Free the shader program
			glDeleteProgram(m_Shader->id);

			// Delete the ID
			delete m_Shader;
		}

		const _VertexAttributeInformation& _Shader::get_attribs() const
		{
			return m_VertexAttributes;
		}

		bool _Shader::is_active() const
		{
			return m_ActiveShader == m_Shader;
		}

		void _Shader::__activate() const
		{
			if (!is_active()) // Check to make sure the shader isn't already active
			{
				glUseProgram(m_Shader->id);
				m_ActiveShader = m_Shader;
			}
		}



		_ID* _UniformBuffer::m_ActiveBuffer{ nullptr };
		
		std::unordered_map<std::string, _UniformBuffer*> _UniformBuffer::m_Buffers{};

		_UniformBuffer::_UniformBuffer(std::string name, const std::vector<_UniformAttribute*> uniforms, unsigned int size)
		{
			m_Name = name;
			m_Buffers.emplace(name, this);

			// Generate the buffer
			GLuint id;
			glGenBuffers(1, &id);
			errcheck("Error when generating the buffer for uniform block " + name + ".");
			glBindBuffer(GL_UNIFORM_BUFFER, id);
			errcheck("Error when binding the buffer for uniform block " + name + ".");
			glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
			errcheck("Error when setting up the buffer for uniform block " + name + ".");
			m_Buffer = new _ID(id);

			// Bind the buffer to a binding point
			m_BindingPoint = new _UniformBuffer::BindingPoint();
			glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint->binding, id);
			errcheck("Error when binding the buffer for uniform block " + name + " to binding point " + std::to_string(m_BindingPoint->binding) + ".");

			// Set the uniforms
			m_Uniforms = uniforms;
		}

		_UniformBuffer::~_UniformBuffer()
		{
			// If active, reset the active buffer.
			if (m_ActiveBuffer == m_Buffer)
				m_ActiveBuffer = nullptr;

			// Delete the buffer
			glDeleteBuffers(1, &m_Buffer->id);

			// Free the ID and location objects
			delete m_Buffer;
			delete m_BindingPoint;
		}

		_UniformBuffer* _UniformBuffer::get_buffer(std::string name)
		{
			auto iter = m_Buffers.find(name);
			if (iter != m_Buffers.end())
				return iter->second;
			return nullptr;
		}

		void _UniformBuffer::bind() const
		{
			if (m_ActiveBuffer != m_Buffer)
			{
				m_ActiveBuffer = m_Buffer;
				glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer->id);
			}
		}



		_VertexBuffer::_VertexBuffer(const std::vector<float>& data)
		{
			// Bind the data to a buffer
			GLuint buf;
			glGenBuffers(1, &buf);
			glBindBuffer(GL_ARRAY_BUFFER, buf);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

			// Set the ID
			m_Buffer = new _ID(buf);
		}

		_VertexBuffer::~_VertexBuffer()
		{
			// If active, reset the active buffer
			if (m_ActiveBuffer == m_Buffer)
				m_ActiveBuffer = nullptr;

			// Free the buffer
			glDeleteBuffers(1, &m_Buffer->id);

			// Delete the ID
			delete m_Buffer;
		}

		bool _VertexBuffer::is_active() const
		{
			return m_ActiveBuffer == m_Buffer;
		}

		void _VertexBuffer::__activate() const {}

		void _VertexBuffer::activate() const
		{
			if (!is_active())
			{
				// Set as the active buffer
				m_ActiveBuffer = m_Buffer;

				// Bind the buffer
				glBindBuffer(GL_ARRAY_BUFFER, m_Buffer->id);

				// Bind the vertex attributes
				__activate();
			}
		}



		_Image::_Image()
		{
			m_IsLoaded = false;
		}
		
		_Image::_Image(const char* path, bool pixel_perfect)
		{
			m_IsLoaded = false;
			load(path, pixel_perfect);
		}

		_Image::~_Image()
		{
			free();
		}

		void _Image::free()
		{
			// If active, resets the active image
			if (m_ActiveImage == m_Image)
				m_ActiveImage = nullptr;

			// Frees the image from memory
			glDeleteTextures(1, &m_Image->id);

			// Deletes the ID object
			delete m_Image;

			// Unset the flag that the image is ready to use
			m_IsLoaded = false;
		}

		bool _Image::is_loaded() const
		{
			return m_IsLoaded;
		}

		bool _Image::load(const char* path, bool pixel_perfect)
		{
			// Free the previous image, if there was one.
			if (m_IsLoaded)
			{
				free();
			}

			// Generate the actual path.
			std::string fpath("res/img/");
			fpath.append(path);

			// Check if the file exists. If not, quits.
			if (FILE* file = fopen(fpath.c_str(), "r"))
			{
				fclose(file);
			}
			else
			{
				return false;
			}

			// Load data from file using SOIL.
			unsigned char* data;
			int channels;

			data = SOIL_load_image(fpath.c_str(), &m_Width, &m_Height, &channels, SOIL_LOAD_RGBA);

			// Bind data to texture.
			GLuint tex;
			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

			// Set the magnification and minimization filters
			if (pixel_perfect)
			{
				// If pixel perfect, just use the nearest pixel
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			}
			else
			{
				// If not pixel perfect, generate and use mipmaps for filtering
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			}

			// Generate an ID object for the image
			m_Image = new _ID(tex);

			// Set the flag that the image is ready to use
			m_IsLoaded = true;
			return true;
		}

		int _Image::get_width() const
		{
			return m_Width;
		}

		int _Image::get_height() const
		{
			return m_Height;
		}

		bool _Image::is_active() const
		{
			return m_ActiveImage == m_Image;
		}

		void _Image::activate() const
		{
			// Change the image being drawn from.
			glBindTexture(GL_TEXTURE_2D, m_Image->id);
		}




		void _VertexBufferDisplayer::set_buffer(_VertexBuffer* buffer)
		{
			// Free the previous buffer being used, if there was one
			if (m_Buffer)
				delete m_Buffer;

			// Set the new buffer to use
			m_Buffer = buffer;
		}


		void _SquareBufferDisplayer::display(BUFFER_KEY start, int count) const
		{
			// Bind the buffer
			m_Buffer->activate();

			// Display the sprite using information from buffer
			glDrawArrays(GL_TRIANGLES, start, 6 * count);
		}

	}




	
	
	VertexBuffer::VertexBuffer(const std::vector<float>& data, const opengl::_VertexAttributeInformation& attribs) : opengl::_VertexBuffer(data), m_Attribs(attribs) {}

	VertexBuffer::~VertexBuffer() {}
	
	void VertexBuffer::__activate() const
	{
		for (auto iter = m_Attribs.attributes.begin(); iter != m_Attribs.attributes.end(); ++iter)
			iter->attrib->set(iter->offset, m_Attribs.stride);
	}


	ImageBuffer::ImageBuffer(const std::vector<float>& data, const opengl::_VertexAttributeInformation& attribs, opengl::_Image* image) : VertexBuffer(data, attribs)
	{
		m_Image = image;
	}

	ImageBuffer::~ImageBuffer()
	{
		delete m_Image;
	}
	
	void ImageBuffer::__activate() const
	{
		// Bind the buffers
		VertexBuffer::__activate();

		// Activate the image
		m_Image->activate();
	}

	int ImageBuffer::get_width() const 
	{
		return m_Image->get_width();
	}

	int ImageBuffer::get_height() const
	{
		return m_Image->get_height();
	}



}