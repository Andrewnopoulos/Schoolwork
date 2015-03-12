#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <Gizmos.h>
#include <vector>
#include "tiny_obj_loader.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

typedef struct
{
	std::vector<float>			positions;
	std::vector<float>			normals;
	std::vector<float>			texcoords;
	std::vector<unsigned int>	indices;
	std::vector<int>			material_ids;
} mesh_t;

typedef struct
{
	std::string		name;
	mesh_t			mesh;
} shape_t;

struct GLInfo
{
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	unsigned int m_index_count;
};

class Tutorial3 : public Application
{
public:
	virtual void Startup();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

	void createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes);

private:
	mat4 view;
	mat4 projection;
	mat4 inverse;
	float previousTime;
	float currentTime;
	FlyCamera myCam;

	std::vector<GLInfo> m_gl_info;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	void setupShader1();
	unsigned int m_programID;


};