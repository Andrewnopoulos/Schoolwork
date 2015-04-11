#include "Assignment1.h"

void Assignment1::Startup()
{
	m_camera->SetInputWindow(window);

	m_camera->setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	m_camera->setSpeed(10);
	m_camera->setRotationSpeed(10);

	previousTime = (float)glfwGetTime();

	SetupScene();
}

void Assignment1::Update()
{
	float currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	 
}

void Assignment1::Draw()
{

}

void Assignment1::Destroy()
{

}

void Assignment1::SetupScene()
{

}