#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <glm\glm.hpp>
#include <Gizmos.h>

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

	unsigned int m_fbo;
	unsigned int m_fboDepth;
	unsigned int m_fboTexture;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	unsigned int m_programID;

	void setupShader();
	void setupFBO();
	void setupMesh();

	void renderFBO();

	void setupProcessing();
	void setupScreenQuad();
	void setupProcessingShader();

	void renderProcessing();


};