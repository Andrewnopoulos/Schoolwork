#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <Gizmos.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::quat;

struct KeyFrame
{
	vec3 position;
	quat rotation;
};

class Tutorial6 : public Application
{
public:
	virtual void Startup();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

private:
	float previousTime;
	float currentTime;
	FlyCamera myCam;

	//vec3 m_positions[2];
	//quat m_rotations[2];

	KeyFrame m_hipFrames[2];
	KeyFrame m_kneeFrames[2];
	KeyFrame m_ankleFrames[2];

	mat4 m_hipBone;
	mat4 m_kneeBone;
	mat4 m_ankleBone;
};