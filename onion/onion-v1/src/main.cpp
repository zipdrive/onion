#include "..\include\onion.h"
#include <GLFW\glfw3.h>

GLFWwindow* g_MainWindow;


void onionKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// TODO
}

void onionCursorCallback(GLFWwindow* window, double xpos, double ypos)
{
	// TODO
}


int onionInit(const char* title)
{
	if (!glfwInit())
	{
		// GLFW failed to initialize.
		return -1;
	}

	g_MainWindow = glfwCreateWindow(640, 480, title, NULL, NULL);
	if (!g_MainWindow)
	{
		// GLFW window failed to initialize.
		glfwTerminate();
		return -1;
	}

	glfwSetKeyCallback(g_MainWindow, onionKeyCallback);
	glfwSetCursorPosCallback(g_MainWindow, onionCursorCallback);

	glfwMakeContextCurrent(g_MainWindow);
}

void onionMain()
{
	while (!glfwWindowShouldClose(g_MainWindow))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(g_MainWindow);
		glfwPollEvents();
	}

	glfwTerminate();
}