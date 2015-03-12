#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <glm\glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Tutorial10 : public Application
{
public:
	virtual void Startup();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

private:
	FlyCamera myCam;
	float previousTime;
	float currentTime;

	unsigned int m_programID;

	void setupShader();
};