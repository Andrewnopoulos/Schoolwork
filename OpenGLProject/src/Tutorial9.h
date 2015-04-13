#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "GPUParticleEmitter.h"

#include "SnowEmitter.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Tutorial9 : public Application
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

	GPUParticleEmitter* m_emitter;

	//SnowEmitter* m_emitter;

	unsigned int m_programID;

	void setupShader();

};