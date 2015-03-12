#include "Application.h"

#include "ShaderManager.h"

int Application::Run()
{
	if (glfwInit() == false)
	return -1;

	window = glfwCreateWindow(1280, 720, "Computer Grafix", nullptr, nullptr);
	
	if (window == nullptr) {
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (ogl_LoadFunctions() == ogl_LOAD_FAILED) {
		glfwDestroyWindow(window);
		glfwTerminate();
		return -3;
	}

	auto major = ogl_GetMajorVersion();
	auto minor = ogl_GetMinorVersion();
	printf("GL: %i.%i\n", major, minor);

	m_shaderManager = new ShaderManager();

	Startup();

	while (glfwWindowShouldClose(window) == false && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS)
	{
		glClearColor(0.25f, 0.25f, 0.25f, 1);
		glEnable(GL_DEPTH_TEST);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		Update();
		Draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	Destroy();

	delete m_shaderManager;

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;

}