#pragma once

#include <vector>
#include <glm\glm.hpp>

class OBJLoader
{

public:
	bool loadOBJ(
		char* fileName,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals,
		std::vector<unsigned int> & out_indices
		);

};