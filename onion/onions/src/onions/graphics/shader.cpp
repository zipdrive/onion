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


		struct _UniformBuffer::Index
		{
			GLuint index;

			Index(GLuint index) : index(index) {}
		};



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
			GLuint m_Location;

		public:
			_UniformProgramAttribute(std::string name, GLuint location) : _UniformAttribute(name)
			{
				m_Location = location;
			}
			
			void set(float data) const
			{
				if (std::is_same<T, float>::value)
				{
					glUniform1f(m_Location, data);
				}
			}

			void set(const vec2f& data) const
			{
				if (std::is_same<T, vec2f>::value)
				{
					glUniform2fv(m_Location, 1, data.matrix_values());
				}
			}

			void set(const vec3f& data) const
			{
				if (std::is_same<T, vec3f>::value)
				{
					glUniform3fv(m_Location, 1, data.matrix_values());
				}
			}

			void set(const vec4f& data) const
			{
				if (std::is_same<T, vec4f>::value)
				{
					glUniform4fv(m_Location, 1, data.matrix_values());
				}
			}

			void set(const mat4f& data) const
			{
				if (std::is_same<T, mat4f>::value)
				{
					glUniformMatrix4fv(m_Location, 1, GL_FALSE, data.matrix_values());
				}
			}

			void set(const mat2x4f& data) const
			{
				if (std::is_same<T, mat2x4f>::value)
				{
					glUniformMatrix4x2fv(m_Location, 1, GL_FALSE, data.matrix_values());
				}
			}

			void set(const MatrixStack& data) const
			{
				if (std::is_same<T, mat4f>::value)
				{
					glUniformMatrix4fv(m_Location, 1, GL_FALSE, data.get_values());
				}
			}
		};

		template <typename T>
		class _UniformBlockAttribute : public _UniformAttribute
		{
		private:
			// The location of the uniform within the block.
			GLuint m_Location;

		public:
			_UniformBlockAttribute(std::string name, GLuint location) : _UniformAttribute(name)
			{
				m_Location = location;
			}

			void set(float data) const
			{
				if (std::is_same<T, float>::value)
				{
					glBufferSubData(GL_UNIFORM_BUFFER, m_Location, 4, &data);
				}
			}

			void set(const vec2f& data) const
			{
				if (std::is_same<T, vec2f>::value)
				{
					glBufferSubData(GL_UNIFORM_BUFFER, m_Location, 8, data.matrix_values());
				}
			}
			
			void set(const vec3f& data) const
			{
				if (std::is_same<T, vec3f>::value)
				{
					glBufferSubData(GL_UNIFORM_BUFFER, m_Location, 16, data.matrix_values());
				}
			}

			void set(const vec4f& data) const
			{
				if (std::is_same<T, vec4f>::value)
				{
					glBufferSubData(GL_UNIFORM_BUFFER, m_Location, 16, data.matrix_values());
				}
			}

			void set(const mat4f& data) const
			{
				if (std::is_same<T, mat4f>::value)
				{
					glBufferSubData(GL_UNIFORM_BUFFER, m_Location, 64, data.matrix_values());
				}
			}

			void set(const mat2x4f& data) const
			{
				if (std::is_same<T, mat2x4f>::value)
				{
					glBufferSubData(GL_UNIFORM_BUFFER, m_Location, 32, data.matrix_values());
				}
			}

			void set(const MatrixStack& data) const
			{
				if (std::is_same<T, mat4f>::value)
				{
					glBufferSubData(GL_UNIFORM_BUFFER, m_Location, 64, data.get_values());
				}
			}
		};


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
#define SHADER_INFO_BUFFER_SIZE 500
			int success; // Retrieves whether compilation was a success or failure.
			char info_buffer[SHADER_INFO_BUFFER_SIZE]; // Buffer in case compilation/linking fails.

			// Vertex shader
			GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
			glCompileShader(vertex_shader);
			// If compilation failed, log the error
			glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertex_shader, SHADER_INFO_BUFFER_SIZE, NULL, info_buffer);
				errlog(std::string("Failed to compile vertex shader.\n") + info_buffer + "\n\n");
				errabort(ERROR_FAILED_TO_COMPILE_VERTEX_SHADER);
			}

			// Fragment shader
			GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
			glCompileShader(fragment_shader);
			// If compilation failed, log the error
			glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragment_shader, SHADER_INFO_BUFFER_SIZE, NULL, info_buffer);
				errlog(std::string("Failed to compile fragment shader.\n") + info_buffer + "\n\n");
				errabort(ERROR_FAILED_TO_COMPILE_FRAGMENT_SHADER);
			}

			// Create shader program
			GLuint id = glCreateProgram();
			glAttachShader(id, vertex_shader);
			glAttachShader(id, fragment_shader);
			glLinkProgram(id);
			m_Shader = new _ID(id);
			// If linking failed, log the error
			glGetShaderiv(id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(id, SHADER_INFO_BUFFER_SIZE, NULL, info_buffer);
				errlog(std::string("Failed to link shader program.\n") + info_buffer + "\n\n");
				errabort(ERROR_FAILED_TO_LINK_SHADER_PROGRAM);
			}

			// Delete the shaders
			glDeleteShader(vertex_shader);
			glDeleteShader(fragment_shader);

			// Determine the shader program's vertex attributes
			GLint vertex_attribute_count;
			glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &vertex_attribute_count);

			if (vertex_attribute_count >= 0)
			{
				m_VertexAttributes.attributes.resize(vertex_attribute_count);
				m_VertexAttributes.stride = 0;

				for (int index = 0; index < vertex_attribute_count; ++index)
				{
					// Calculate the size of the vertex attribute
					GLint size;
					GLenum type;
					glGetActiveAttrib(id, index, 0, NULL, &size, &type, NULL);

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

			for (int index = 0; index < uniform_block_count; ++index)
			{
				GLint name_length;
				glGetActiveUniformBlockiv(id, index, GL_UNIFORM_BLOCK_NAME_LENGTH, &name_length);

				GLchar* raw_name = new GLchar[name_length];
				glGetActiveUniformBlockName(id, index, name_length, NULL, raw_name);
				std::string name(raw_name);

				_UniformBuffer* buf = _UniformBuffer::get_buffer(name);
				if (!buf) // If the buffer does not exist, construct the uniform block
				{
					// Get each uniform in the block
					GLint buf_size;
					glGetActiveUniformBlockiv(id, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &buf_size);

					GLint* buf_uniforms = new GLint[buf_size];
					glGetActiveUniformBlockiv(id, index, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, buf_uniforms);
					GLuint* buf_uniform_indices = (GLuint*)buf_uniforms;

					// Get the type of each uniform in the block
					GLint* types = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_TYPE, types);

					// Get the offset of each uniform in the block
					GLint* offsets = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_OFFSET, offsets);

					// Get the size of each uniform in the block
					GLint* sizes = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_SIZE, sizes);
					unsigned int total_size = 0;

					// Get the length of each uniform's name
					GLint* name_lengths = new GLint[buf_size];
					glGetActiveUniformsiv(id, buf_size, buf_uniform_indices, GL_UNIFORM_NAME_LENGTH, name_lengths);

					// Create a uniform attribute for each uniform in the block
					std::vector<_UniformAttribute*> uniforms(buf_size);
					for (int i = 0; i < buf_size; ++i)
					{
						std::string uniform_name;
						if (name_lengths[i] > 0)
						{
							GLchar* name_buf = new GLchar[name_lengths[i]];
							glGetActiveUniformName(id, buf_uniform_indices[i], name_lengths[i], NULL, name_buf);
							uniform_name = std::string(name_buf);
							delete[] name_buf;
						}

						_UniformAttribute* u = nullptr;
						GLuint offset = offsets[i];

						switch (types[i])
						{
						case GL_FLOAT:
							u = new _UniformBlockAttribute<float>(uniform_name, offset);
							break;
						case GL_FLOAT_VEC2:
							u = new _UniformBlockAttribute<vec2f>(uniform_name, offset);
							break;
						case GL_FLOAT_VEC3:
							u = new _UniformBlockAttribute<vec3f>(uniform_name, offset);
							break;
						case GL_FLOAT_VEC4:
							u = new _UniformBlockAttribute<vec4f>(uniform_name, offset);
							break;
						case GL_FLOAT_MAT4:
							u = new _UniformBlockAttribute<mat4f>(uniform_name, offset);
							break;
						case GL_FLOAT_MAT4x2:
							u = new _UniformBlockAttribute<mat2x4f>(uniform_name, offset);
							break;
						}

						uniforms[i] = u;
						total_size += sizes[i];
					}

					// Construct the uniform block
					buf = new _UniformBuffer(name, uniforms, total_size);

					// Clean up all the arrays we allocated earlier
					delete[] buf_uniforms;
					delete[] types;
					delete[] offsets;
					delete[] sizes;
					delete[] name_lengths;
				}

				// Bind the shader's uniform block to a uniform buffer
				glUniformBlockBinding(id, index, buf->m_Index->index);

				// Clean up the array we allocated earlier
				delete[] raw_name;
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
					// Retrieve the info associated with the uniform
					GLint size;
					GLenum type;
					glGetActiveUniform(id, uniform_index, 0, NULL, &size, &type, NULL);

					GLint name_length;
					glGetActiveUniformsiv(id, 1, &uniform_index, GL_UNIFORM_NAME_LENGTH, &name_length);

					std::string uniform_name;
					if (name_length > 0)
					{
						GLchar* name_buf = new GLchar[name_length];
						glGetActiveUniformName(id, uniform_index, name_length, NULL, name_buf);
						uniform_name = std::string(name_buf);
						delete[] name_buf;
					}

					// Construct a uniform object
					_UniformAttribute* u = nullptr;

					switch (type)
					{
					case GL_FLOAT:
						u = new _UniformProgramAttribute<float>(uniform_name, uniform_index);
						break;
					case GL_FLOAT_VEC2:
						u = new _UniformProgramAttribute<vec2f>(uniform_name, uniform_index);
						break;
					case GL_FLOAT_VEC3:
						u = new _UniformProgramAttribute<vec3f>(uniform_name, uniform_index);
						break;
					case GL_FLOAT_VEC4:
						u = new _UniformProgramAttribute<vec4f>(uniform_name, uniform_index);
						break;
					case GL_FLOAT_MAT4:
						u = new _UniformProgramAttribute<mat4f>(uniform_name, uniform_index);
						break;
					case GL_FLOAT_MAT4x2:
						u = new _UniformProgramAttribute<mat2x4f>(uniform_name, uniform_index);
						break;
					}

					if (u)
					{
						m_UniformAttributes.push_back(u);
					}
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



		std::unordered_map<std::string, _UniformBuffer*> _UniformBuffer::m_Buffers{};

		_UniformBuffer::_UniformBuffer(std::string name, const std::vector<_UniformAttribute*> uniforms, unsigned int size)
		{
			m_Name = name;
			m_Buffers.emplace(name, this);

			// Generate the buffer
			GLuint id;
			glGenBuffers(1, &id);
			glBindBuffer(GL_UNIFORM_BUFFER, id);
			glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
			m_Buffer = new _ID(id);

			// Bind the buffer to a binding point
			glBindBufferBase(GL_UNIFORM_BUFFER, m_Buffers.size(), id);
			m_Index = new _UniformBuffer::Index(m_Buffers.size());

			// Set the uniforms
			m_Uniforms = uniforms;
		}

		_UniformBuffer::~_UniformBuffer()
		{
			// Delete the buffer
			glDeleteBuffers(1, &m_Buffer->id);

			// Free the ID and location objects
			delete m_Buffer;
			delete m_Index;
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
			glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer->id);
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