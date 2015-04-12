#pragma once

#include <FBXFile.h>

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

public:

	void LoadVerts(char* filePath);
	void LoadDiffuse(char* filePath);
	void LoadNormals(char* filePath);

	void SetupShader(char* fragpath, char* vertpath);
	void Render(mat4 LocalMatrix);
};