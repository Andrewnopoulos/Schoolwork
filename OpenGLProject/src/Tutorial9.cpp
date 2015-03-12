#include "Tutorial9.h"

void Tutorial9::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(20);
	myCam.setRotationSpeed(0.1f);

	m_emitter = new GPUParticleEmitter();
	m_emitter->initialise(100000,
		0.1f, 5.0f,
		5, 20,
		1, 0.0f,
		vec4(1, 0, 0, 1), vec4(1, 1, 0, 1));
}

void Tutorial9::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);
}

void Tutorial9::Draw()
{
	m_emitter->draw((float)glfwGetTime(),
		myCam.getWorldTransform(),
		myCam.getProjectionView());
}

void Tutorial9::Destroy()
{
	delete m_emitter;
}