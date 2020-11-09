#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../include/onions/primitive.h"

namespace onion
{

	template <>
	struct _primitive<bool>
	{
		using type = GLboolean;
	};
	
	template <>
	struct _primitive<int>
	{
		using type = GLint;
	};

	template <>
	struct _primitive<unsigned int>
	{
		using type = GLuint;
	};

	template <>
	struct _primitive<float>
	{
		using type = GLfloat;
	};

	template <>
	struct _primitive<double>
	{
		using type = GLdouble;
	};

	template <>
	struct _primitive<char>
	{
		using type = GLchar;
	};

	template <>
	struct _primitive<std::string>
	{
		using type = std::basic_string<GLchar>;
	};



	template <>
	struct type_limits<GLint>
	{
		static constexpr GLint min()
		{
			return std::numeric_limits<GLint>::min() / (1 << ((sizeof(GLint) * 4) - 1));
		}

		static constexpr GLint max()
		{
			return std::numeric_limits<GLint>::max() / (1 << ((sizeof(GLint) * 4) - 1));
		}
	};

	template <>
	struct type_limits<GLuint>
	{
		static constexpr GLuint min()
		{
			return 0;
		}

		static constexpr GLuint max()
		{
			return std::numeric_limits<GLuint>::max() / (1 << ((sizeof(GLuint) * 4) - 1));
		}
	};

}