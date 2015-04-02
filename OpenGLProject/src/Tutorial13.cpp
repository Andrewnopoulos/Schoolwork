#include "Tutorial13.h"
#include <stb_image.h>
#include "Gizmos.h"

#include "ShaderManager.h"

void Tutorial13::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 10000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(10);
	myCam.setRotationSpeed(10.0f);

	m_fbx = new FBXFile();
	m_fbx->load("../data/soulspear/soulspear.fbx");

	setupShader();

	createOpenGLBuffers(m_fbx);

	setupVerts();

	setupShadowMap();

	loadTextures();


	Gizmos::create();
}

void Tutorial13::Update()
{
	Gizmos::clear();
	Gizmos::addTransform(mat4(1), 20.0f);

	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);

	glm::mat4 lightProjection = glm::ortho<float>(-20, 20,
		-20, 20, -20, 20);

	glm::mat4 lightView = glm::lookAt(m_lightDirection, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 lightTrans = glm::inverse(lightView);
	lightTrans[3] = myCam.getWorldTransform()[3];
	lightView = glm::inverse(lightTrans);

	m_lightMatrix = lightProjection * lightView;
}

void Tutorial13::Draw()
{
	m_lightDirection = glm::normalize(vec3(sin(glfwGetTime()), 3, cos(glfwGetTime())));

	mat4 lightMatrix = textureSpaceOffset * m_lightMatrix;

	// shadow pass: bind our shadow map target and clear the depth
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glViewport(0, 0, 1028, 1028);
	glClear(GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_shadowDepth);
	// bind the light matrix
	int loc = glGetUniformLocation(m_shadowDepth, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_lightMatrix[0][0]));

	// draw all shadow-casting geometry
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(),
			GL_UNSIGNED_INT, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_floorShader);

	loc = glGetUniformLocation(m_floorShader, "lightDir");
	glUniform3f(loc, m_lightDirection.x, m_lightDirection.y, m_lightDirection.z);

	loc = glGetUniformLocation(m_floorShader, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(myCam.getProjectionView()));

	loc = glGetUniformLocation(m_floorShader, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);

	loc = glGetUniformLocation(m_floorShader, "shadowMap");
	glUniform1i(loc, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_fboDepth);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glUseProgram(m_programID);

	loc = glGetUniformLocation(m_programID, "shadowMap");
	glUniform1i(loc, 2);

	loc = glGetUniformLocation(m_programID, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);

	loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(myCam.getProjectionView()));

	loc = glGetUniformLocation(m_programID, "LightDir");
	glUniform3f(loc, m_lightDirection.x, m_lightDirection.y, m_lightDirection.z);

	int light_colour_uniform = glGetUniformLocation(m_programID, "LightColour");
	glUniform3f(light_colour_uniform, 1, 1, 1);

	mat4 camera_matrix = myCam.getWorldTransform();
	int camera_pos_uniform = glGetUniformLocation(m_programID, "CameraPos");
	glUniform3f(camera_pos_uniform, camera_matrix[3][0], camera_matrix[3][1], camera_matrix[3][2]);

	int specular_uniform = glGetUniformLocation(m_programID, "SpecPow");
	glUniform1f(specular_uniform, 12);

	int shadowbias = glGetUniformLocation(m_programID, "shadowBias");
	glUniform1f(shadowbias, 0.1f);

	int roughness = glGetUniformLocation(m_programID, "roughness");
	glUniform1f(roughness, 0.2f);

	int fresnel = glGetUniformLocation(m_programID, "fresnel");
	glUniform1f(fresnel, 1.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normal);

	loc = glGetUniformLocation(m_programID, "diffuse");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(m_programID, "normal");
	glUniform1i(loc, 1);

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}


	Gizmos::draw(myCam.getProjectionView());
}

void Tutorial13::Destroy()
{
	cleanupOpenGLBuffers(m_fbx);

	glDeleteProgram(m_programID);
}

void Tutorial13::createOpenGLBuffers(FBXFile* fbx)
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

void Tutorial13::cleanupOpenGLBuffers(FBXFile* fbx)
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

void Tutorial13::setupShader()
{
	m_programID = m_shaderManager->LoadShader("Object", "../data/shaders/tut13.vert", "../data/shaders/tut13.frag");
	m_floorShader = m_shaderManager->LoadShader("Floor", "../data/shaders/floorShader.vert", "../data/shaders/floorShader.frag");
	m_shadowDepth = m_shaderManager->LoadShader("Shadows", "../data/shaders/shadow13.vert", "../data/shaders/shadow13.frag");
}

void Tutorial13::loadTextures()
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data;

	// load diffuse map
	data = stbi_load("../data/soulspear/soulspear_diffuse.tga",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	// load normal map
	data = stbi_load("../data/soulspear/soulspear_normal.tga",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_normal);
	glBindTexture(GL_TEXTURE_2D, m_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

}

void Tutorial13::setupVerts()
{
	float vertexData[] = {
		-5000, -1, 5000, 1, 0, 1, 0, 0,
		5000, -1, 5000, 1, 0, 1, 0, 0,
		5000, -1, -5000, 1, 0, 1, 0, 0,
		-5000, -1, -5000, 1, 0, 1, 0, 0,
	};
	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)* 8 * 4,
		vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)* 6,
		indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 8, 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE,
		sizeof(float)* 8, ((char*)0) + 16);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Tutorial13::setupRenderTarget()
{
	// setup shadow map buffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboDepth);
	glBindTexture(GL_TEXTURE_2D, m_fboDepth);

	// texture uses a 16-bit depth component format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);

	// attached as a depth attachment to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		m_fboDepth, 0);

	// no colour targets are used
	glDrawBuffer(GL_NONE);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Tutorial13::setupShadowMap()
{
	// setup shadow map buffer
	glGenFramebuffers(1, &m_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
	glGenTextures(1, &m_fboDepth);
	glBindTexture(GL_TEXTURE_2D, m_fboDepth);

	// texture uses a 16-bit depth component format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1028, 1028,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
		GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
		GL_CLAMP_TO_EDGE);

	// attached as a depth attachment to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		m_fboDepth, 0);

	// no colour targets are used
	glDrawBuffer(GL_NONE);
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_lightDirection = glm::normalize(glm::vec3(1, 2.5f, 1));

	glm::mat4 lightProjection = glm::ortho<float>(-10, 10,
		-10, 10, -10, 10);

	glm::mat4 lightView = glm::lookAt(m_lightDirection,
		glm::vec3(0), glm::vec3(0, 1, 0));
	
	m_lightMatrix = lightProjection * lightView;

	textureSpaceOffset = mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);
}

