#include <forward_list>
#include <iostream>

#include "../include/math.h"
#include "../include/game.h"
#include "../include/graphics.h"
#include "../include/world.h"


// The matrix stack.
std::forward_list<mat4x4f> g_MatrixStack;

void mat_push()
{
	if (g_MatrixStack.empty())
	{
		g_MatrixStack.emplace_front();
	}
	else
	{
		mat4x4f top = g_MatrixStack.front();
		g_MatrixStack.push_front(top);
	}
}

void mat_pop()
{
	g_MatrixStack.pop_front();
}

mat4x4f& mat_get()
{
	if (g_MatrixStack.empty())
	{
		g_MatrixStack.emplace_front();
	}

	return g_MatrixStack.front();
}

void mat_print()
{
	mat4x4f& mat = mat_get();
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			std::printf("% .2f\t", mat.get(i, j));
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

const float* mat_get_values()
{
	return mat_get().matrix_values();
}

void mat_ortho(float left, float right, float bottom, float top, float near, float far)
{
	g_MatrixStack.clear();

	mat4x4f mat;
	mat.set(0, 0, 2.f / (right - left));
	mat.set(1, 1, 2.f / (top - bottom));
	mat.set(2, 2, 2.f / (far - near));
	mat.set(0, 3, (left + right) / (left - right));
	mat.set(1, 3, (bottom + top) / (bottom - top));
	mat.set(2, 3, (near + far) / (near - far));

	g_MatrixStack.push_front(mat);
}

void mat_translate(float dx, float dy, float dz)
{
	mat4x4f& top = mat_get();
	
	mat4x4f trans;
	trans.get(0, 3) = dx;
	trans.get(1, 3) = dy;
	trans.get(2, 3) = dz;

	g_MatrixStack.front() = mat_mul(top, trans);
}

void mat_scale(float sx, float sy, float sz)
{
	mat4x4f& top = mat_get();
	for (int k = 3; k >= 0; --k)
	{
		top.get(0, k) *= sx;
		top.get(1, k) *= sy;
		top.get(2, k) *= sz;
	}
}


// The application window.
GLFWwindow* g_Window = NULL;


int Application::display()
{
	if (!g_Window)
	{
		// If the application window has not been initialized, initialize it.
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		g_Window = glfwCreateWindow(width, height, title.c_str(), fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
		if (!g_Window)
		{
			glfwTerminate();
			return 1;
		}
	}
	else
	{
		// If the application window has been initialized, alter its size.
		if (fullscreen)
		{
			// TODO fix fullscreen
			glfwSetWindowMonitor(g_Window, glfwGetPrimaryMonitor(), 0, 0, width, height, GLFW_DONT_CARE);
		}
		else
		{
			glfwSetWindowMonitor(g_Window, NULL, 100, 100, width, height, GLFW_DONT_CARE);
		}
	}

	// Set the viewport
	glViewport(0, 0, width, height);

	// Set the orthogonal projection
	float halfWidth = 0.5f * width;
	float halfHeight = 0.5f * height;
	mat_ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -10.f, 100.f);

	return 0;
}


/// <summary>Initializes GLFW and creates the window for the program.</summary>
/// <returns>True if an error was encountered, false otherwise.</returns>
bool loadGLFW()
{
	// Initialize the GLFW library.
	if (!glfwInit())
	{
		return true;
	}

	// Load the application settings.
	Application* app = get_application_settings();
	if (!app)
	{
		glfwTerminate();
		return true;
	}

	// Initialize the application window.
	app->display();
	if (!g_Window)
	{
		glfwTerminate();
		return true;
	}

	glfwMakeContextCurrent(g_Window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		glfwDestroyWindow(g_Window);
		glfwTerminate();
		return true;
	}

	return false;
}


static const struct
{
	float x, y;
	float r, g, b;
} vertices[3] =
{
	{ -0.6f, -0.4f, 1.f, 0.f, 0.f },
	{ 0.6f, -0.4f, 0.f, 1.f, 0.f },
	{ 0.f,  0.6f, 0.f, 0.f, 1.f }
};

static const char* vertex_shader_text =
"#version 110\n"
"attribute vec3 vCol;\n"
"attribute vec2 vPos;\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 110\n"
"varying vec3 color;\n"
"void main()\n"
"{\n"
"    gl_FragColor = vec4(color, 1.0);\n"
"}\n";

void test()
{
	/*GLuint vertex_buffer, vertex_shader, fragment_shader, program;
	GLint mvp_location, vpos_location, vcol_location;

	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
	glCompileShader(vertex_shader);

	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
	glCompileShader(fragment_shader);

	program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);

	vpos_location = glGetAttribLocation(program, "vPos");
	vcol_location = glGetAttribLocation(program, "vCol");

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)0);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*)(sizeof(float) * 2));

	while (!glfwWindowShouldClose(g_Window))
	{
		float ratio;
		int width, height;

		glfwGetFramebufferSize(g_Window, &width, &height);
		ratio = width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(program);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(g_Window);
		glfwPollEvents();
	}*/

	
}

// Tests the display of sprites.
void test_sprite()
{
	SpriteSheet sheet("res/img/tiles.png");

	SpriteInfo sprite(0, 0, TILE_WIDTH, TILE_HEIGHT);
	std::vector<SpriteInfo*> sprites;
	sprites.push_back(&sprite);
	sheet.load_sprites(sprites);

	// Enter the main loop for the program.
	while (!glfwWindowShouldClose(g_Window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		sheet.display(0);

		glfwSwapBuffers(g_Window);
		glfwPollEvents();
	}
}

// Tests the display of chunks.
void test_chunk()
{
	Chunk chunk;

	glClear(GL_COLOR_BUFFER_BIT);
	chunk.display(0, 100, 0, 100);
	glfwSwapBuffers(g_Window);

	while (!glfwWindowShouldClose(g_Window))
	{
		glfwPollEvents();
	}
}


// The entry point for the program.
int main()
{
	// Initialize GLFW and GLEW.
	if (loadGLFW())
	{
		return -1;
	}

	test_chunk();

	// Close everything down.
	glfwDestroyWindow(g_Window);
	glfwTerminate();
	return 0;
}