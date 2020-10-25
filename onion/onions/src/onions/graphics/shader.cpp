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
				} while (errcode = glGetError());

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



		template <GLenum... _EnumValues>
		struct map_glenum_base
		{
			template <typename... _TypeValues>
			struct to_type_base
			{
				template <std::size_t N, GLenum _CompareValue, GLenum... _RemainingEnumValues>
				struct EnumIndexer
				{
					static constexpr std::size_t index = N;
				};

				template <std::size_t N, GLenum _CompareValue, GLenum _NthEnumValue, GLenum... _RemainingEnumValues>
				struct EnumIndexer<N, _CompareValue, _NthEnumValue, _RemainingEnumValues...>
				{
					static constexpr std::size_t index = _CompareValue == _NthEnumValue ? N : EnumIndexer<N + 1, _CompareValue, _RemainingEnumValues...>::index;
				};


				template <std::size_t N, GLenum _NthEnumValue, GLenum... _RemainingEnumValues>
				struct NthEnumGetter
				{
					static constexpr GLenum value = NthEnumGetter<N - 1, _RemainingEnumValues...>::value;
				};

				template <GLenum _NthEnumValue, GLenum... _RemainingEnumValues>
				struct NthEnumGetter<0, _NthEnumValue, _RemainingEnumValues...>
				{
					static constexpr GLenum value = _NthEnumValue;
				};

				template <std::size_t N>
				struct NthEnum
				{
					static constexpr GLenum value = NthEnumGetter<N, _EnumValues...>::value;
				};


				template <std::size_t N>
				using NthType = typename std::tuple_element<N, std::tuple<_TypeValues...>>::type;

				template <GLenum _EnumValue>
				using mapper = NthType<EnumIndexer<0, _EnumValue, _EnumValues...>::index>;


				static constexpr std::size_t count = sizeof...(_EnumValues);
			};
		};

		typedef map_glenum_base <
			GL_FLOAT,
			GL_FLOAT_VEC2,
			GL_FLOAT_VEC3,
			GL_FLOAT_VEC4,
			GL_FLOAT_MAT2,
			GL_FLOAT_MAT3,
			GL_FLOAT_MAT4,
			GL_FLOAT_MAT2x3,
			GL_FLOAT_MAT2x4,
			GL_FLOAT_MAT3x4,
			GL_FLOAT_MAT3x2,
			GL_FLOAT_MAT4x2,
			GL_FLOAT_MAT4x3,
			GL_INT,
			GL_INT_VEC2,
			GL_INT_VEC3,
			GL_INT_VEC4,
			GL_SAMPLER_2D,
			GL_UNSIGNED_INT,
			GL_UNSIGNED_INT_VEC2,
			GL_UNSIGNED_INT_VEC3,
			GL_UNSIGNED_INT_VEC4,
			GL_DOUBLE,
			GL_DOUBLE_VEC2,
			GL_DOUBLE_VEC3,
			GL_DOUBLE_VEC4,
			GL_DOUBLE_MAT2,
			GL_DOUBLE_MAT3,
			GL_DOUBLE_MAT4,
			GL_DOUBLE_MAT2x3,
			GL_DOUBLE_MAT2x4,
			GL_DOUBLE_MAT3x4,
			GL_DOUBLE_MAT3x2,
			GL_DOUBLE_MAT4x2,
			GL_DOUBLE_MAT4x3
		> map_glenum;

		typedef map_glenum::to_type_base<
			Float,
			FLOAT_VEC2,
			FLOAT_VEC3,
			FLOAT_VEC4,
			FLOAT_MAT2,
			FLOAT_MAT3,
			FLOAT_MAT4,
			FLOAT_MAT2X3,
			FLOAT_MAT2X4,
			FLOAT_MAT3X4,
			FLOAT_MAT3X2,
			FLOAT_MAT4X2,
			FLOAT_MAT4X3,
			Int,
			INT_VEC2,
			INT_VEC3,
			INT_VEC4,
			Int,
			Uint,
			UINT_VEC2,
			UINT_VEC3,
			UINT_VEC4,
			Double,
			DOUBLE_VEC2,
			DOUBLE_VEC3,
			DOUBLE_VEC4,
			DOUBLE_MAT2,
			DOUBLE_MAT3,
			DOUBLE_MAT4,
			DOUBLE_MAT2X3,
			DOUBLE_MAT2X4,
			DOUBLE_MAT3X4,
			DOUBLE_MAT3X2,
			DOUBLE_MAT4X2,
			DOUBLE_MAT4X3
		> map_glenum_to_type;

		template <GLenum _EnumValue>
		using glenum_type = map_glenum_to_type::mapper<_EnumValue>;

		template <std::size_t N>
		using glenum_at = map_glenum_to_type::NthEnum<N>;

		template <std::size_t N>
		using glenum_type_at = map_glenum_to_type::NthType<N>;



		_VertexAttrib::_VertexAttrib(Uint offset) : offset(offset) {}
		
		template <typename T>
		VertexAttrib<T>::VertexAttrib(Uint offset) : _VertexAttrib(offset) {}
		
		template <typename T>
		Uint VertexAttrib<T>::size() const
		{
			return type_size<T>::whole;
		}

		template <typename T>
		Int VertexAttrib<T>::count() const
		{
			return type_size<T>::whole / type_size<T>::primitive;
		}

		template <typename T>
		struct type_to_glenum_primitive
		{
			static constexpr GLenum value = 0;
		};

		template <>
		struct type_to_glenum_primitive<Float>
		{
			static constexpr GLenum value = GL_FLOAT;
		};

		template <>
		struct type_to_glenum_primitive<Double>
		{
			static constexpr GLenum value = GL_DOUBLE;
		};

		template <>
		struct type_to_glenum_primitive<Int>
		{
			static constexpr GLenum value = GL_INT;
		};

		template <>
		struct type_to_glenum_primitive<Uint>
		{
			static constexpr GLenum value = GL_UNSIGNED_INT;
		};

		template <typename T, int _Columns, int _Rows>
		struct type_to_glenum_primitive<matrix<T, _Columns, _Rows>>
		{
			static constexpr GLenum value = type_to_glenum_primitive<T>::value;
		};

		template <typename T>
		void VertexAttrib<T>::set(Uint index, Uint stride) const
		{
			static constexpr GLenum type = type_to_glenum_primitive<T>::value;
			
			switch (type)
			{
			case GL_FLOAT:
				glVertexAttribPointer(index, count(), type, GL_FALSE, stride, (void*)offset);
				break;
			case GL_DOUBLE:
				glVertexAttribLPointer(index, count(), type, stride, (void*)offset);
				break;
			default:
				glVertexAttribIPointer(index, count(), type, stride, (void*)offset);
				break;
			}
		}


		VertexAttribs::~VertexAttribs()
		{
			for (auto iter = attribs.begin(); iter != attribs.end(); ++iter)
				delete *iter;
		}
		
		template <std::size_t N>
		_VertexAttrib* generate_vertex_attribute(GLenum type, Uint offset)
		{
			if (glenum_at<N>::value == type)
			{
				return new VertexAttrib<glenum_type_at<N>>(offset);
			}
			else
			{
				return generate_vertex_attribute<N + 1>(type, offset);
			}
		}

		template <>
		_VertexAttrib* generate_vertex_attribute<map_glenum_to_type::count>(GLenum type, GLuint offset)
		{
			return nullptr;
		}

		void VertexAttribs::push(Uint type)
		{
			Uint offset = 0;
			for (auto iter = attribs.begin(); iter != attribs.end(); ++iter)
				offset += (*iter)->size();

			if (_VertexAttrib* attrib = generate_vertex_attribute<0>(type, offset))
				attribs.push_back(attrib);
		}

		void VertexAttribs::enable() const
		{
			if (!attribs.empty())
			{
				GLsizei stride = attribs.back()->offset + attribs.back()->size();
				for (Uint index = 0; index < attribs.size(); ++index)
				{
					attribs[index]->set(index, stride);
					errcheck("ONION: Error received when setting the vertex attrib with index " + std::to_string(index) + ".");
					glEnableVertexAttribArray(index);
					errcheck("ONION: Error received when enabling vertex attrib array with index " + std::to_string(index) + ".");
				}
			}
		}

		void VertexAttribs::disable() const
		{
			for (Uint index = 0; index < attribs.size(); ++index)
				glDisableVertexAttribArray(index);
		}



		/// <summary>Retrieves the number of base primitives that make up the type.</summary>
		/// <returns>The sizeof the type, divided by the sizeof the base primitives underlying the type.
		/// For primitives, it will return 1.
		/// For matrices, it will return the number of elements in the matrix.</returns>
		template <std::size_t N = 0>
		std::size_t retrieve_countof_type(GLenum type)
		{
			if (glenum_at<N>::value == type)
			{
				return type_size<glenum_type_at<N>>::whole / type_size<glenum_type_at<N>>::primitive;
			}
			else
			{
				return retrieve_countof_type<N + 1>(type);
			}
		}

		template <>
		std::size_t retrieve_countof_type<map_glenum_to_type::count>(GLenum type)
		{
			return 0;
		}



		_UniformAttribute::_UniformAttribute(std::string name) : name(name) {}
		_UniformAttribute::~_UniformAttribute() {}

		template <typename T>
		_UniformTypedAttribute<T>::_UniformTypedAttribute(std::string name) : _UniformAttribute(name) {}

		template <typename T>
		unsigned int _UniformTypedAttribute<T>::get_size() const
		{
			return type_size<T>::whole;
		}

		
		// The location of a uniform within a shader program.
		template <typename T>
		class _UniformProgramAttribute : public _UniformTypedAttribute<T>
		{
		private:
			// The location within the shader program.
			const GLint m_Location;

		public:
			_UniformProgramAttribute(std::string name, GLint location) : _UniformTypedAttribute<T>(name), m_Location(location) {}

			void set(const T& value) const {}
		};

		template <>
		void _UniformProgramAttribute<Float>::set(const Float& value) const
		{
			glUniform1f(m_Location, value);
		}

		template <>
		void _UniformProgramAttribute<Int>::set(const Int& value) const
		{
			glUniform1i(m_Location, value);
		}

		template <>
		void _UniformProgramAttribute<Uint>::set(const Uint& value) const
		{
			glUniform1ui(m_Location, value);
		}

		template <>
		void _UniformProgramAttribute<Double>::set(const Double& value) const
		{
			glUniform1d(m_Location, value);
		}


		
		
		
		template <typename... _Functions>
		struct FunctionList
		{
		private:
			template <int N, typename _NthFunction, typename... _RemainingFunctions>
			struct NthFunctionGetter
			{
				using type = typename NthFunctionGetter<N - 1, _RemainingFunctions...>::type;
			};

			template <typename _NthFunction, typename... _RemainingFunctions>
			struct NthFunctionGetter<0, _NthFunction, _RemainingFunctions...>
			{
				using type = _NthFunction;
			};

		public:
			const std::tuple<_Functions...> func;

			constexpr FunctionList(_Functions... f) : func(f...) {}

			template <int N>
			using ptr = typename NthFunctionGetter<N, _Functions...>::type;

			template <int N>
			constexpr ptr<N> get()
			{
				return std::get<N>(func);
			}
		};

		template <typename... _Functions>
		constexpr auto generate_function_list(_Functions... f)
		{
			return std::tuple<_Functions...>(f...);
		}
		
		
		
		// This is a hacky solution, I'm aware.
		// But I'm working with VS2015, which doesn't fully support C++14's constexpr extensions, so I'm making do.

		// Also, for some reason it seems like casting PFNGLUNIFORM2FVPROC* to void(*)(GLint, GLint, const GLfloat*) results in an
		// access violation error, so I have to keep a reference to the actual type of each function.

		template <typename T>
		struct UniformMatrixProgramFunctions {};

		template <>
		struct UniformMatrixProgramFunctions<float>
		{
			using tuple_type = std::tuple<
				PFNGLUNIFORM2FVPROC*,
				PFNGLUNIFORM3FVPROC*,
				PFNGLUNIFORM4FVPROC*,
				PFNGLUNIFORMMATRIX2FVPROC*,
				PFNGLUNIFORMMATRIX3X2FVPROC*,
				PFNGLUNIFORMMATRIX4X2FVPROC*,
				PFNGLUNIFORMMATRIX2X3FVPROC*,
				PFNGLUNIFORMMATRIX3FVPROC*,
				PFNGLUNIFORMMATRIX4X3FVPROC*,
				PFNGLUNIFORMMATRIX2X4FVPROC*,
				PFNGLUNIFORMMATRIX3X4FVPROC*,
				PFNGLUNIFORMMATRIX4FVPROC*
			>;

			static const tuple_type functions;
		};

		const UniformMatrixProgramFunctions<float>::tuple_type UniformMatrixProgramFunctions<float>::functions =
			std::make_tuple(
				&glUniform2fv,
				&glUniform3fv,
				&glUniform4fv,
				&glUniformMatrix2fv,
				&glUniformMatrix2x3fv,
				&glUniformMatrix2x4fv,
				&glUniformMatrix3x2fv,
				&glUniformMatrix3fv,
				&glUniformMatrix3x4fv,
				&glUniformMatrix4x2fv,
				&glUniformMatrix4x3fv,
				&glUniformMatrix4fv
			);
		
		template <>
		struct UniformMatrixProgramFunctions<double>
		{
			using tuple_type = std::tuple<
				PFNGLUNIFORM2DVPROC*,
				PFNGLUNIFORM3DVPROC*,
				PFNGLUNIFORM4DVPROC*,
				PFNGLUNIFORMMATRIX2DVPROC*,
				PFNGLUNIFORMMATRIX3X2DVPROC*,
				PFNGLUNIFORMMATRIX4X2DVPROC*,
				PFNGLUNIFORMMATRIX2X3DVPROC*,
				PFNGLUNIFORMMATRIX3DVPROC*,
				PFNGLUNIFORMMATRIX4X3DVPROC*,
				PFNGLUNIFORMMATRIX2X4DVPROC*,
				PFNGLUNIFORMMATRIX3X4DVPROC*,
				PFNGLUNIFORMMATRIX4DVPROC*
			>;

			static const tuple_type functions;
		};

		const UniformMatrixProgramFunctions<double>::tuple_type UniformMatrixProgramFunctions<double>::functions =
			std::make_tuple(
				&glUniform2dv,
				&glUniform3dv,
				&glUniform4dv,
				&glUniformMatrix2dv,
				&glUniformMatrix2x3dv,
				&glUniformMatrix2x4dv,
				&glUniformMatrix3x2dv,
				&glUniformMatrix3dv,
				&glUniformMatrix3x4dv,
				&glUniformMatrix4x2dv,
				&glUniformMatrix4x3dv,
				&glUniformMatrix4dv
			);
		
		template <>
		struct UniformMatrixProgramFunctions<int>
		{
			using tuple_type = std::tuple<
				PFNGLUNIFORM2IVPROC*,
				PFNGLUNIFORM3IVPROC*,
				PFNGLUNIFORM4IVPROC*
			>;

			static const tuple_type functions;
		};

		const UniformMatrixProgramFunctions<int>::tuple_type UniformMatrixProgramFunctions<int>::functions =
			std::make_tuple(
				&glUniform2iv,
				&glUniform3iv,
				&glUniform4iv
			);

		template <>
		struct UniformMatrixProgramFunctions<unsigned int>
		{
			using tuple_type = std::tuple<
				PFNGLUNIFORM2UIVPROC*,
				PFNGLUNIFORM3UIVPROC*,
				PFNGLUNIFORM4UIVPROC*
			>;

			static const tuple_type functions;
		};

		const UniformMatrixProgramFunctions<unsigned int>::tuple_type UniformMatrixProgramFunctions<unsigned int>::functions = 
			std::make_tuple(
				&glUniform2uiv,
				&glUniform3uiv,
				&glUniform4uiv
			);


		template <typename _Number, int _Columns, int _Rows>
		class _UniformProgramAttribute<matrix<_Number, _Columns, _Rows>> : public _UniformTypedAttribute<matrix<_Number, _Columns, _Rows>>
		{
		private:
			// The index of the function.
			static constexpr int m_FunctionIndex = (_Rows - 2) + (3 * (_Columns - 1));

			using FunctionPtr = std::tuple_element_t<m_FunctionIndex, typename UniformMatrixProgramFunctions<_Number>::tuple_type>;

			// The function that sets the uniform values.
			static FunctionPtr m_Function;

			// The location within the shader program.
			const GLint m_Location;

		public:
			_UniformProgramAttribute(std::string name, GLint location) : _UniformTypedAttribute<matrix<_Number, _Columns, _Rows>>(name), m_Location(location) {}

			void set(const matrix<_Number, _Columns, _Rows>& value) const 
			{
				(*m_Function)(m_Location, 1, GL_FALSE, value.matrix_values());
			}
		};

		template <typename _Number, int _Columns, int _Rows>
		typename _UniformProgramAttribute<matrix<_Number, _Columns, _Rows>>::FunctionPtr _UniformProgramAttribute<matrix<_Number, _Columns, _Rows>>::m_Function =
			std::get<_UniformProgramAttribute<matrix<_Number, _Columns, _Rows>>::m_FunctionIndex>(UniformMatrixProgramFunctions<_Number>::functions);
		
		template <typename _Number, int _Rows>
		class _UniformProgramAttribute<matrix<_Number, _Rows, 1>> : public _UniformTypedAttribute<matrix<_Number, _Rows, 1>>
		{
		private:
			// The index of the function.
			static constexpr int m_FunctionIndex = _Rows - 2;

			using FunctionPtr = std::tuple_element_t<m_FunctionIndex, typename UniformMatrixProgramFunctions<_Number>::tuple_type>;

			// The function that sets the uniform values
			static FunctionPtr m_Function;

			// The location within the shader program.
			const GLint m_Location;

		public:
			_UniformProgramAttribute(std::string name, GLint location) : _UniformTypedAttribute<matrix<_Number, _Rows, 1>>(name), m_Location(location) {}

			void set(const matrix<_Number, _Rows, 1>& value) const
			{
				(*m_Function)(m_Location, 1, value.matrix_values());
			}
		};

		template <typename _Number, int _Rows>
		typename _UniformProgramAttribute<matrix<_Number, _Rows, 1>>::FunctionPtr _UniformProgramAttribute<matrix<_Number, _Rows, 1>>::m_Function =
			std::get<_UniformProgramAttribute<matrix<_Number, _Rows, 1>>::m_FunctionIndex>(UniformMatrixProgramFunctions<_Number>::functions);


		template <std::size_t N = 0>
		_UniformAttribute* generate_uniform_program_attribute(GLenum type, std::string name, GLint location)
		{
			if (glenum_at<N>::value == type)
			{
				return new _UniformProgramAttribute<glenum_type_at<N>>(name, location);
			}
			else
			{
				return generate_uniform_program_attribute<N + 1>(type, name, location);
			}
		}

		template <>
		_UniformAttribute* generate_uniform_program_attribute<map_glenum_to_type::count>(GLenum type, std::string, GLint location)
		{
			return nullptr;
		}



		template <typename T>
		_UniformBlockAttribute<T>::_UniformBlockAttribute(std::string name, Int offset) : _UniformTypedAttribute<T>(name), m_Offset(offset) {}

		template <typename T>
		void _UniformBlockAttribute<T>::set(const T& value) const 
		{
			T* ptr = (T*)glMapBufferRange(GL_UNIFORM_BUFFER, m_Offset, type_size<T>::whole, GL_MAP_WRITE_BIT);//GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
			*ptr = value;
			glUnmapBuffer(GL_UNIFORM_BUFFER);
		}


		template <typename _Number, int _Columns, int _Rows>
		class _UniformBlockAttribute<matrix<_Number, _Columns, _Rows>> : public _UniformTypedAttribute<matrix<_Number, _Columns, _Rows>>
		{
		protected:
			// The offset of the uniform within the block.
			const GLint m_Offset;

		public:
			_UniformBlockAttribute(std::string name, GLint offset) : _UniformTypedAttribute<matrix<_Number, _Columns, _Rows>>(name), m_Offset(offset) {}

			void set(const matrix<_Number, _Columns, _Rows>& value) const 
			{
				_Number* ptr = (_Number*)glMapBufferRange(GL_UNIFORM_BUFFER, m_Offset, type_size<matrix<_Number, _Columns, _Rows>>::whole, GL_MAP_WRITE_BIT);// GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT);
				for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
					ptr[k] = value.get(k);
				glUnmapBuffer(GL_UNIFORM_BUFFER);
			}
		};


		template <std::size_t N = 0>
		_UniformAttribute* generate_uniform_block_attribute(GLenum type, std::string name, GLint offset)
		{
			if (glenum_at<N>::value == type)
			{
				return new _UniformBlockAttribute<glenum_type_at<N>>(name, offset);
			}
			else
			{
				return generate_uniform_block_attribute<N + 1>(type, name, offset);
			}
		}

		template <>
		_UniformAttribute* generate_uniform_block_attribute<map_glenum_to_type::count>(GLenum type, std::string, GLint location)
		{
			return nullptr;
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
			void set(const mat4x2f& value) const {}

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



		_Shader::_Shader(const char* path, const std::vector<String>& uniform_names)
		{
			std::string vertex_shader, fragment_shader;

			// Load the files of the shaders
			std::string fpath("res/shaders/");
			fpath += path;

			LoadFile vertex(fpath + ".vertex");
			LoadFile geometry(fpath + ".geometry");
			LoadFile fragment(fpath + ".fragment");

			// Collect the text of each shader in a string
			while (vertex.good())
				vertex_shader += vertex.load_line() + "\n";

			while (fragment.good())
				fragment_shader += fragment.load_line() + "\n";

			if (geometry.good()) // A geometry shader
			{
				std::string geometry_shader;
				while (geometry.good())
					geometry_shader += geometry.load_line() + "\n";

				// Compile the shaders
				compile(vertex_shader.c_str(), geometry_shader.c_str(), fragment_shader.c_str(), uniform_names);
			}
			else // No geometry shader
			{
				// Compile the shaders
				compile(vertex_shader.c_str(), fragment_shader.c_str(), uniform_names);
			}
		}

		_Shader::_Shader(const char* vertex_shader_text, const char* fragment_shader_text, const std::vector<String>& uniform_names)
		{
			compile(vertex_shader_text, fragment_shader_text, uniform_names);
		}
		

#define SHADER_INFO_BUFFER_SIZE 500

		void _Shader::compile(const char* vertex_shader_text, const char* fragment_shader_text, const std::vector<String>& uniform_names)
		{
			errcheck("Error received at some point before beginning shader compilation.");

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

			// Process the variables
			process(uniform_names);
		}

		void _Shader::compile(const char* vertex_shader_text, const char* geometry_shader_text, const char* fragment_shader_text, const std::vector<String>& uniform_names)
		{
			errcheck("Error received at some point before beginning shader compilation.");

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

			// Geometry shader
			GLuint geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry_shader, 1, &geometry_shader_text, NULL);
			errcheck("Error received when setting the source text for the geometry shader.");
			glCompileShader(geometry_shader);
			errcheck("Error received when issuing the instruction to compile the geometry shader.");
			// If compilation failed, log the error
			glGetShaderiv(geometry_shader, GL_COMPILE_STATUS, &success);
			errcheck("Error received when checking the compilation status of the geometry shader.");
			if (!success)
			{
				glGetShaderInfoLog(geometry_shader, SHADER_INFO_BUFFER_SIZE, NULL, info_buffer);
				errlog(std::string("ONION: Error received when compiling the geometry shader.\n") + info_buffer + "\n\n");
				errabort(ERROR_FAILED_TO_COMPILE_GEOMETRY_SHADER);
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
			glAttachShader(id, geometry_shader);
			errcheck("Error received when attaching geometry shader to shader program.");
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
			glDeleteShader(geometry_shader);
			errcheck("Error received when deleting the geometry shader after linking it to the shader program.");
			glDeleteShader(fragment_shader);
			errcheck("Error received when deleting the fragment shader after linking it to the shader program.");

			// Process the variables
			process(uniform_names);
		}

		void _Shader::process(const std::vector<String>& uniform_names)
		{
			GLuint id = m_Shader->id;

			// Determine the shader program's vertex attributes
			GLint vertex_attribute_count;
			glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &vertex_attribute_count);
			errcheck("Error received when checking number of vertex attributes.");

			if (vertex_attribute_count >= 0)
			{
				m_VertexAttributes.attribs.reserve(vertex_attribute_count);

				for (GLuint index = 0; index < vertex_attribute_count; ++index)
				{
					// Calculate the size of the vertex attribute
					GLint size;
					GLenum type;
					glGetActiveAttrib(id, index, 0, NULL, &size, &type, NULL);
					errcheck("Error received when retrieving information about the vertex attribute with index " + std::to_string(index) + ".");

					m_VertexAttributes.push(type);
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
					// Construct an empty uniform block
					buf = new _UniformBuffer(uniform_block_name);
				}

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
						std::string uname(uniform_name);
						if (array_size > 1)
							uname += "[" + std::to_string(n) + "]";

						if (_UniformAttribute* u = generate_uniform_block_attribute(type, uname, offset))
						{
							uniforms.push_back(u);
							total_uniform_block_size = std::max(total_uniform_block_size, offset + u->get_size());
						}

						offset += array_strides[i];
					}
				}

				buf->set_uniforms(uniforms, total_uniform_block_size);

				// Bind the shader's uniform block to a uniform buffer
				glUniformBlockBinding(id, uniform_block_index, buf->m_BindingPoint->binding);
				errcheck("Error binding the uniform block \"" + uniform_block_name + "\" of the shader.");

				// Clean up all the arrays we allocated earlier
				delete[] buf_uniforms;
				delete[] types;
				delete[] offsets;
				delete[] array_sizes;
				delete[] array_strides;
				delete[] uniform_name_lengths;
			}

			// Determine any other uniforms
			GLint uniform_count;
			glGetProgramiv(id, GL_ACTIVE_UNIFORMS, &uniform_count);
			errcheck("Error retrieving the number of uniforms of the shader program.");

			m_UniformAttributes.resize(uniform_names.size());

			for (unsigned int uniform_index = 0; uniform_index < uniform_count; ++uniform_index)
			{
				GLint uniform_block_index;
				glGetActiveUniformsiv(id, 1, &uniform_index, GL_UNIFORM_BLOCK_INDEX, &uniform_block_index);
				errcheck("Error retrieving the uniform block of the uniform with index " + std::to_string(uniform_index) + ".");

				if (uniform_block_index < 0) // Uniform doesn't belong to a uniform block
				{
					// Retrieve the name of the uniform
					GLint uniform_name_length;
					glGetActiveUniformsiv(id, 1, &uniform_index, GL_UNIFORM_NAME_LENGTH, &uniform_name_length);
					errcheck("Error retrieving the length of the name of the uniform with index " + std::to_string(uniform_index) + ".");

					std::string uniform_name;
					if (uniform_name_length > 0)
					{
						GLchar* name_buf = new GLchar[uniform_name_length];
						glGetActiveUniformName(id, uniform_index, uniform_name_length, NULL, name_buf);
						errcheck("Error retrieving the name of the uniform with index " + std::to_string(uniform_index) + ".");
						uniform_name = std::string(name_buf);
						delete[] name_buf;
					}

					// Retrieve the location of the uniform
					GLint uniform_location;
					uniform_location = glGetUniformLocation(id, uniform_name.c_str());
					if (uniform_location < 0)
					{
						errcheck("Error retrieving the location of the uniform with name " + uniform_name + ".");
					}
					else
					{
						// Retrieve the info associated with the uniform
						GLint array_size = 1;
						glGetActiveUniformsiv(id, 1, &uniform_index, GL_UNIFORM_SIZE, &array_size);
						errcheck("Error retrieving the array size of uniform \"" + uniform_name + "\".");

						GLint type;
						glGetActiveUniformsiv(id, 1, &uniform_index, GL_UNIFORM_TYPE, &type);
						errcheck("Error retrieving the type of uniform \"" + uniform_name + "\".");

						// Construct a uniform object
						for (int n = 0; n < array_size; ++n)
						{
							std::string uname(uniform_name);
							if (array_size > 1)
								uname += "[" + std::to_string(n) + "]";

							// Generate the object managing the uniform attribute
							if (_UniformAttribute* u = generate_uniform_program_attribute(type, uname, uniform_location))
							{
								for (int k = uniform_names.size() - 1; k >= 0; --k)
								{
									if (uname.compare(uniform_names[k]) == 0)
									{
										m_UniformAttributes[k] = u;
										break;
									}
								}
							}
						}
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

		const VertexAttribs& _Shader::get_attribs() const
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

		_UniformBuffer::_UniformBuffer(std::string name)
		{
			m_Name = name;
			m_Buffers.emplace(name, this);
			m_Buffer = nullptr;
		}
		
		_UniformBuffer::_UniformBuffer(std::string name, const std::vector<_UniformAttribute*>& uniforms, unsigned int size)
		{
			m_Name = name;
			m_Buffers.emplace(name, this);
			m_Buffer = nullptr;

			// Set the uniforms
			set_uniforms(uniforms, size);
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
			return new _UniformBuffer(name);
		}

		void _UniformBuffer::set_uniforms(const std::vector<_UniformAttribute*>& uniforms, unsigned int size)
		{
			if (!m_Buffer)
			{
				// Generate the buffer
				GLuint id;
				glGenBuffers(1, &id);
				errcheck("Error when generating the buffer for uniform block " + m_Name + ".");
				glBindBuffer(GL_UNIFORM_BUFFER, id);
				errcheck("Error when binding the buffer for uniform block " + m_Name + ".");
				glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
				errcheck("Error when setting up the buffer for uniform block " + m_Name + ".");
				m_Buffer = new _ID(id);

				// Bind the buffer to a binding point
				m_BindingPoint = new _UniformBuffer::BindingPoint();
				glBindBufferBase(GL_UNIFORM_BUFFER, m_BindingPoint->binding, id);
				errcheck("Error when binding the buffer for uniform block " + m_Name + " to binding point " + std::to_string(m_BindingPoint->binding) + ".");

				// Set the uniforms
				m_Uniforms = uniforms;
			}
		}

		void _UniformBuffer::bind() const
		{
			if (m_ActiveBuffer != m_Buffer)
			{
				m_ActiveBuffer = m_Buffer;
				glBindBuffer(GL_UNIFORM_BUFFER, m_Buffer->id);
			}
		}



		_VertexBuffer::_VertexBuffer(const _VertexBufferData* data, const VertexAttribs& attribs)
		{
			// Generate a vertex array object
			errcheck("ONION: Error generated at some point before creating the vertex buffer.");
			GLuint arr;
			glGenVertexArrays(1, &arr);
			glBindVertexArray(arr);
			errcheck("ONION: Error generated when generating and binding the VAO.");

			// Generate the array of data for the buffer
			std::size_t bytes;
			char* ptr = data->compile(bytes);

			// Bind the data to a buffer
			GLuint buf;
			glGenBuffers(1, &buf);
			glBindBuffer(GL_ARRAY_BUFFER, buf);
			glBufferData(GL_ARRAY_BUFFER, bytes, ptr, GL_STATIC_DRAW);
			errcheck("ONION: Error generated when generating and binding the VBO.");

			// Clean up the array of data for the buffer
			delete[] ptr;

			// Set vertex attributes
			attribs.enable();
			errcheck("ONION: Error generated when enabling vertex attribs.");

			// Set the ID
			m_Buffer = new _ID(buf);
			m_VAO = new _ID(arr);
		}

		_VertexBuffer::~_VertexBuffer()
		{
			// If active, reset the active buffer
			if (m_ActiveBuffer == m_Buffer)
				m_ActiveBuffer = nullptr;

			// Free the buffer
			glDeleteBuffers(1, &m_Buffer->id);

			// Free the VAO
			glDeleteVertexArrays(1, &m_VAO->id);

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
				glBindVertexArray(m_VAO->id);

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

		void _Image::activate(int slot) const
		{
			if (slot >= 0 && slot < 16) // Make sure the slot is valid
			{
				// Change the active texture
				glActiveTexture(GL_TEXTURE0 + slot);

				// Change the image being drawn from.
				glBindTexture(GL_TEXTURE_2D, m_Image->id);
			}
		}




		void _VertexBufferDisplayer::set_buffer(_VertexBuffer* buffer)
		{
			// Free the previous buffer being used, if there was one
			if (m_Buffer)
				delete m_Buffer;

			// Set the new buffer to use
			m_Buffer = buffer;
		}


		void _SquareBufferDisplayer::display(BUFFER_KEY start, Int count) const
		{
			// Bind the buffer
			m_Buffer->activate();

			// Display the sprite using information from buffer
			glDrawArrays(GL_TRIANGLES, start, 6 * count);
		}

	}




	
	
	ImageBuffer::ImageBuffer(const opengl::_VertexBufferData* data, const opengl::VertexAttribs& attribs, opengl::_Image* image) : opengl::_VertexBuffer(data, attribs)
	{
		m_Image = image;
	}

	ImageBuffer::~ImageBuffer()
	{
		delete m_Image;
	}
	
	void ImageBuffer::__activate() const
	{
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