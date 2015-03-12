#pragma once

#include "gl_core_4_4.h"
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <glm\glm.hpp>
#include <glm\ext.hpp>

class ShaderManager;

class Application
{

public:

	virtual void Startup() = 0;
	virtual void Destroy() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	int Run();

protected:

	GLFWwindow* window;
	ShaderManager* m_shaderManager;

};