#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <Gizmos.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Tutorial1 : public Application
{
public:
	virtual void Startup();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

private:
	mat4 view;
	mat4 projection;
	mat4 inverse;
	float previousTime;
	float currentTime;
	FlyCamera myCam;

};