#include "Tutorial7.h"
#include <stb_image.h>

void Tutorial7::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 10000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(700);

	m_fbx = new FBXFile();
	m_fbx->load("../data/characters/Pyro/pyro.fbx");

	createOpenGLBuffers(m_fbx);

	setupShader();

	loadTextures();
}

void Tutorial7::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);

	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	FBXAnimation* animation = m_fbx->getAnimationByIndex(0);

	skeleton->evaluate(animation, currentTime);

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
	{
		skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}

}

void Tutorial7::Draw()
{
	glUseProgram(m_programID);

	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(myCam.getProjectionView()));

	/*int light_dir_uniform = glGetUniformLocation(m_programID, "LightDir");
	glUniform3f(light_dir_uniform, 0, 1, 0);*/

	vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));
	loc = glGetUniformLocation(m_programID, "LightDir");
	glUniform3f(loc, light.x, light.y, light.z);

	int light_colour_uniform = glGetUniformLocation(m_programID, "LightColour");
	glUniform3f(light_colour_uniform, 1, 1, 1);

	mat4 camera_matrix = myCam.getWorldTransform();
	int camera_pos_uniform = glGetUniformLocation(m_programID, "CameraPos");
	glUniform3f(camera_pos_uniform, camera_matrix[3][0], camera_matrix[3][1], camera_matrix[3][2]);

	int specular_uniform = glGetUniformLocation(m_programID, "SpecPow");
	glUniform1f(specular_uniform, 12);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal);

	loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(m_programID, "normal");
	glUniform1i(loc, 1);

	FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
	skeleton->updateBones();

	int bones_location = glGetUniformLocation(m_programID, "bones");
	glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}


}

void Tutorial7::Destroy()
{
	cleanupOpenGLBuffers(m_fbx);

	glDeleteProgram(m_programID);
}

void Tutorial7::createOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
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

void Tutorial7::cleanupOpenGLBuffers(FBXFile* fbx)
{
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void Tutorial7::setupShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 Position; \
							layout(location=1) in vec4 Normal; \
							layout(location=2) in vec2 TexCoord; \
							layout(location=3) in vec4 Tangent; \
							layout(location=4) in vec4 weights; \
							layout(location=5) in vec4 indices; \
							out vec3 vNormal; \
							out vec3 vPosition; \
							out vec3 vBiTangent; \
							out vec3 vTangent; \
							out vec2 vTexCoord; \
							uniform mat4 ProjectionView; \
							const int MAX_BONES = 128; \
							uniform mat4 bones[MAX_BONES]; \
							void main() { vNormal = Normal.xyz; \
							vTexCoord = TexCoord; \
							vTangent = Tangent.xyz; \
							vBiTangent = cross(vNormal, vTangent); \
							vPosition = Position.xyz; \
							ivec4 index = ivec4(indices); \
							vec4 P = bones[ index.x ] * Position * weights.x; \
							P += bones[index.y] * Position * weights.y; \
							P += bones[index.z] * Position * weights.z; \
							P += bones[index.w] * Position * weights.w; \
							gl_Position = ProjectionView * P; }";
	const char* fsSource = "#version 410\n \
						   	in vec3 vNormal; \
							in vec3 vPosition; \
							in vec2 vTexCoord; \
							in vec3 vTangent; \
							in vec3 vBiTangent; \
							out vec4 FragColor; \
							uniform vec3 LightDir; \
							uniform vec3 LightColour; \
							uniform vec3 CameraPos; \
							uniform sampler2D diffuse; \
							uniform sampler2D normal; \
							uniform float SpecPow; \
							void main() { \
							mat3 TBN = mat3( \
							normalize( vTangent ), \
							normalize( vBiTangent ), \
							normalize( vNormal )); \
							vec3 N = texture( normal, \
							vTexCoord).xyz * 2 - 1; \
							float d = max(0, dot( normalize(TBN * N), normalize(LightDir) ) ); \
							vec3 E = normalize( CameraPos - vPosition ); \
							vec3 R = reflect( -LightDir, vNormal ); \
							float s = max( 0, dot( E, R ) ); \
							s = pow( s, SpecPow ); \
							FragColor = texture(diffuse, vTexCoord); \
							FragColor.rgb = FragColor.rgb * LightColour * d + LightColour * s * 0.1; }";

	/*
	FragColor = texture(diffuse, vTexCoord);\
							FragColor.rgb = (FragColor.rgb * LightColour * d + LightColour * s);\*/

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE) {
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Tutorial7::loadTextures()
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data;

	// load diffuse map
	data = stbi_load("../data/characters/Pyro/Pyro_D.tga",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	// load normal map
	data = stbi_load("../data/characters/Pyro/Pyro_N.tga",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_normal);
	glBindTexture(GL_TEXTURE_2D, m_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

}