#include "Tutorial10.h"
#include "ShaderManager.h"

void Tutorial10::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(20);
	myCam.setRotationSpeed(0.1f);

	setupShader();
}

void Tutorial10::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);
}

void Tutorial10::Draw()
{
	glUseProgram(m_programID);

}

void Tutorial10::Destroy()
{

}