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
		_ID* _Buffer::m_ActiveBuffer{ nullptr };
		_ID* _Image::m_ActiveImage{ nullptr };


		struct _Location
		{
			// The location of the uniform.
			GLuint loc;

			/// <summary>Constructs a location of a uniform in the shader program.</summary>
			/// <param name="loc">The location of the uniform.</param>
			_Location(GLuint loc) : loc(loc) {}
		};



		_Shader::_Shader(const char* vertex_shader_text, const char* fragment_shader_text)
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


		_Location* _Shader::get_uniform(const char* name)
		{
			return new _Location(glGetUniformLocation(m_Shader->id, name));
		}

		_Location* _Shader::get_attrib(const char* name)
		{
			return new _Location(glGetAttribLocation(m_Shader->id, name));
		}

		template <>
		void _Shader::set_uniform(_Location* loc, float value)
		{
			glUniform1f(loc->loc, value);
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const vec2f* value)
		{
			glUniform2fv(loc->loc, 1, value->matrix_values());
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const vec3f* value)
		{
			glUniform3fv(loc->loc, 1, value->matrix_values());
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const vec4f* value)
		{
			glUniform4fv(loc->loc, 1, value->matrix_values());
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const mat2f* value)
		{
			glUniformMatrix2fv(loc->loc, 1, GL_FALSE, value->matrix_values());
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const mat3f* value)
		{
			glUniformMatrix3fv(loc->loc, 1, GL_FALSE, value->matrix_values());
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const mat4f* value)
		{
			glUniformMatrix4fv(loc->loc, 1, GL_FALSE, value->matrix_values());
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const mat2x4f* value)
		{
			glUniformMatrix4x2fv(loc->loc, 1, GL_FALSE, value->matrix_values());
		}

		template <>
		void _Shader::set_uniform(_Location* loc, const onion::Texture* texture)
		{
			if (texture)
			{
				set_uniform(loc, &texture->tex);
			}
		}

		template <typename _Uniform>
		void _Shader::set_uniform(_Location* loc, _Uniform value) {}


		bool _Shader::is_active()
		{
			return m_ActiveShader == m_Shader;
		}

		void _Shader::__activate()
		{
			if (!is_active()) // Check to make sure the shader isn't already active
			{
				glUseProgram(m_Shader->id);
				m_ActiveShader = m_Shader;
			}
		}



		_Buffer::_Buffer(const std::vector<float>& data)
		{
			// Bind the data to a buffer
			GLuint buf;
			glGenBuffers(1, &buf);
			glBindBuffer(GL_ARRAY_BUFFER, buf);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(), data.data(), GL_STATIC_DRAW);

			// Set the ID
			m_Buffer = new _ID(buf);
		}

		_Buffer::~_Buffer()
		{
			// If active, reset the active buffer
			if (m_ActiveBuffer == m_Buffer)
				m_ActiveBuffer = nullptr;

			// Free the buffer
			glDeleteBuffers(1, &m_Buffer->id);

			// Delete the ID
			delete m_Buffer;
		}

		bool _Buffer::is_active()
		{
			return m_ActiveBuffer == m_Buffer;
		}

		void _Buffer::__activate()
		{
			// Bind the buffer
			glBindBuffer(GL_ARRAY_BUFFER, m_Buffer->id);
		}

		void _Buffer::activate()
		{
			if (!is_active())
			{
				m_ActiveBuffer = m_Buffer;
				__activate();
			}
		}

		void _Buffer::set_attrib(_Location* loc, unsigned int ptr, unsigned int length, unsigned int stride)
		{
			glEnableVertexAttribArray(loc->loc);
			glVertexAttribPointer(loc->loc, length, GL_FLOAT, GL_FALSE, sizeof(float) * stride, (void*)(sizeof(float) * ptr));
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

		int _Image::get_width()
		{
			return m_Width;
		}

		int _Image::get_height()
		{
			return m_Height;
		}

		bool _Image::is_active()
		{
			return m_ActiveImage == m_Image;
		}

		void _Image::activate()
		{
			// Change the image being drawn from.
			glBindTexture(GL_TEXTURE_2D, m_Image->id);
		}




		void _BufferDisplayer::set_buffer(_Buffer* buffer)
		{
			// Free the previous buffer being used, if there was one
			if (m_Buffer)
				delete m_Buffer;

			// Set the new buffer to use
			m_Buffer = buffer;
		}


		void _SquareBufferDisplayer::display(BUFFER_KEY key) const
		{
			// Bind the buffer
			m_Buffer->activate();

			// Display the sprite using information from buffer
			glDrawArrays(GL_TRIANGLES, key, 6);
		}


		void _SequentialSquareBufferDisplayer::set_display_count(int count)
		{
			if (count > 0)
				m_Count = count;
		}

		void _SequentialSquareBufferDisplayer::display(BUFFER_KEY key) const
		{
			// Bind the buffer
			m_Buffer->activate();

			// Display a number of sequential squares in the buffer equal to m_Count
			glDrawArrays(GL_TRIANGLES, key, 6 * m_Count);
		}

	}
}