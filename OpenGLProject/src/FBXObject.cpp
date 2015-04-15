#include "FBXObject.h"
#include <stb_image.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include "ShaderManager.h"
#include "FlyCamera.h"

FBXObject::FBXObject(char* filePath)
{
	m_fbx = new FBXFile();
	m_fbx->load(filePath, FBXFile::UNITS_CENTIMETER, false, false);
	createOpenGLBuffers();
	hasDiffuseMap = false;
	hasNormalsMap = false;
}

void FBXObject::createOpenGLBuffers()
{
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER,
			mesh->m_vertices.size() * sizeof(FBXVertex),
			mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->m_indices.size() * sizeof(unsigned int),
			mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
			sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE,
			sizeof(FBXVertex),
			((char*)0) + FBXVertex::NormalOffset);

		glEnableVertexAttribArray(2); // texture
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);

		glEnableVertexAttribArray(3); // tangent
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);

		glEnableVertexAttribArray(4); // weights
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);

		glEnableVertexAttribArray(5); //indices
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;

	}
}

void FBXObject::cleanupOpenGLBuffers()
{
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

FBXObject::~FBXObject()
{
	cleanupOpenGLBuffers();
	delete m_fbx;
	glDeleteProgram(m_programID);
}

void FBXObject::SetupShader(ShaderManager* sMan,  char* vertPath, char* fragPath)
{
	m_programID = sMan->LoadShader("program", vertPath, fragPath);
}

void FBXObject::LoadDiffuse(char* filePath)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data;

	// load diffuse map
	data = stbi_load(filePath,
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	hasDiffuseMap = true;
}

void FBXObject::LoadNormals(char* filePath)
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data;

	// load normal map
	data = stbi_load(filePath,
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_normal);
	glBindTexture(GL_TEXTURE_2D, m_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	hasNormalsMap = true;
}

void FBXObject::Render(FlyCamera* camera, vec3 LightDir, mat4 LocalMatrix)
{
	glUseProgram(m_programID);

	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		glm::value_ptr(camera->getProjectionView()));

	loc = glGetUniformLocation(m_programID, "Local");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(LocalMatrix));

	vec4 Light = vec4(LightDir, 1) * LocalMatrix;
	vec3 Lightout = glm::normalize(vec3(Light.x, Light.y, Light.z));

	loc = glGetUniformLocation(m_programID, "LightDir");
	glUniform3f(loc, Lightout.x, Lightout.y, Lightout.z);

	mat4 camera_matrix = camera->getWorldTransform();
	loc = glGetUniformLocation(m_programID, "CameraPos");
	glUniform3f(loc, camera_matrix[3][0], 
		camera_matrix[3][1], camera_matrix[3][2]);

	if (hasDiffuseMap)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		loc = glGetUniformLocation(m_programID, "diffuseMap");
		glUniform1i(loc, 0);
	}

	if (hasNormalsMap)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_normal);

		loc = glGetUniformLocation(m_programID, "normalMap");
		glUniform1i(loc, 1);
	}

	int roughness = glGetUniformLocation(m_programID, "roughness");
	glUniform1f(roughness, 0.8f);

	int fresnel = glGetUniformLocation(m_programID, "fresnel");
	glUniform1f(fresnel, 1.0f);

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

}