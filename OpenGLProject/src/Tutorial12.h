#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <glm\glm.hpp>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Vertex_b {
	vec4 position;
	vec2 texIndex;
};

class Tutorial12 : public Application
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

	unsigned int dimensions;

	unsigned int m_VAO;
	unsigned int m_VBO; // vertex buffer object
	unsigned int m_IBO;

	unsigned int m_indexCount;

	unsigned int m_programID;

	unsigned int m_perlin_texture;

	void generateGrid( unsigned int rows, unsigned int cols );

	void setupShader1();
	void setupShader2();
	void setupShader3();
	void setupShader4();

	void generatePerlin(unsigned int dimension);
};