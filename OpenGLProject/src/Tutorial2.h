#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <glm\glm.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Vertex_a {
	vec4 position;
	vec4 colour;
};

class Tutorial2 : public Application
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

	unsigned int m_VAO;
	unsigned int m_VBO; // vertex buffer object
	unsigned int m_IBO;

	unsigned int m_indexCount;

	unsigned int m_programID;

	void generateGrid( unsigned int rows, unsigned int cols );

	void loadFromFile();

	void setupShader1();
	void setupShader2();
	void setupShader3();
};