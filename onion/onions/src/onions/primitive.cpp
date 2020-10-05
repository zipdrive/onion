#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../include/onions/primitive.h"

namespace onion
{

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

}