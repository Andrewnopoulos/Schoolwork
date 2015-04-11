#pragma once

#include "Application.h"
#include "FlyCamera.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class Assignment1 : public Application
{
public:
	virtual void Startup();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

private:
	FlyCamera* m_camera;
	
	float previousTime;
	float deltaTime;

	void SetupScene();
};