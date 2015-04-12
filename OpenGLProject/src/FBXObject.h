#pragma once

#include <FBXFile.h>
#include "gl_core_4_4.h"

class ShaderManager;
class FlyCamera;

using glm::vec3;
using glm::vec4;
using glm::mat4;

class FBXObject
{
private:
	FBXFile* m_fbx;
	unsigned int m_programID, m_texture, m_normal;

	void createOpenGLBuffers();
	void cleanupOpenGLBuffers();

	bool hasNormalsMap;
	bool hasDiffuseMap;

public:

	FBXObject(char* filePath);
	~FBXObject();

	void LoadDiffuse(char* filePath);
	void LoadNormals(char* filePath);

	void SetupShader(ShaderManager* sMan, char* vertPath, char* fragPath);
	void Render(FlyCamera* camera, vec3 LightDir, mat4 LocalMatrix);
};