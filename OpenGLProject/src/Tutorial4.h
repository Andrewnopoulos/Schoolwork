#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <Gizmos.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Tutorial4 : public Application
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

	unsigned int m_program;
	unsigned int m_texture;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	void setupShader();
	void setupVerts();

};