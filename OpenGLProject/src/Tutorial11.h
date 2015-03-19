#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <vector>
#include <FBXFile.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Tutorial11 : public Application
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

	void createOpenGLBuffers(FBXFile* fbx);
	void cleanupOpenGLBuffers(FBXFile* fbx);

	void loadTextures();

	FBXFile* m_fbx;
	unsigned int m_programID;
	unsigned int m_floorShader;

	unsigned int m_texture, m_normal;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;

	void setupShader();
	void setupVerts();
};