#include "Tutorial1.h"

void Tutorial1::Startup()
{
	Gizmos::create();

	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16/9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(5);

	currentTime = 0;
	previousTime = 0;
}

void Tutorial1::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	Gizmos::clear();

	Gizmos::addTransform(glm::mat4(1));

	myCam.update(deltaTime);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(
			vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);

		Gizmos::addLine(
			vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

	Gizmos::addSphere(glm::vec3(0, 0, 0), 5.0f, 20, 20, black);
}

void Tutorial1::Draw()
{
	Gizmos::draw(myCam.getProjectionView());
}

void Tutorial1::Destroy()
{
	Gizmos::destroy();
}
