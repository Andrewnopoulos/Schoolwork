#include "Assignment1.h"
#include <stb_image.h>
#include <vector>

void Assignment1::Startup()
{
	m_terrainGen = new TerrainGenerator(5.0f);

	m_camera = new FlyCamera();

	m_camera->SetInputWindow(window);

	m_camera->setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	m_camera->setLookAt(vec3(10, 30, 10), vec3(100, 0, 100), vec3(0, 1, 0));

	m_camera->setSpeed(100);
	m_camera->setRotationSpeed(10);

	previousTime = (float)glfwGetTime();

	LoadTextures();

	SetupScene();
}

void Assignment1::Update()
{
	float currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	m_camera->update(deltaTime);
}

void Assignment1::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawSkybox();

	DrawTerrain();
}

void Assignment1::DrawTerrain()
{
	glUseProgram(m_terrainShader);
	unsigned int projectionViewUniform = glGetUniformLocation(m_terrainShader, "ProjectionView"); // find uniform values in shaders
	glUniformMatrix4fv(projectionViewUniform, 1, false, &m_camera->getProjectionView()[0][0]); // set uniform values in shaders

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_terrainTextureMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_sandTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_grassTexture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_snowTexture);

	unsigned int texloc = glGetUniformLocation(m_terrainShader, "heightMap");
	glUniform1i(texloc, 0);

	texloc = glGetUniformLocation(m_terrainShader, "sandTexture");
	glUniform1i(texloc, 1);

	texloc = glGetUniformLocation(m_terrainShader, "grassTexture");
	glUniform1i(texloc, 2);

	texloc = glGetUniformLocation(m_terrainShader, "snowTexture");
	glUniform1i(texloc, 3);

	texloc = glGetUniformLocation(m_terrainShader, "maxHeight");
	glUniform1f(texloc, m_terrainGen->GetMaxHeight());

	glBindVertexArray(m_ter_VAO);

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe

	glDrawElements(GL_TRIANGLES, m_terrainIndeces, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Assignment1::LoadTextures()
{
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data;


	data = stbi_load("../data/textures/grass.jpg",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_grassTexture);
	glBindTexture(GL_TEXTURE_2D, m_grassTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);


	data = stbi_load("../data/textures/dirt.jpg",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_sandTexture);
	glBindTexture(GL_TEXTURE_2D, m_sandTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);


	data = stbi_load("../data/textures/snow.jpg",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_snowTexture);
	glBindTexture(GL_TEXTURE_2D, m_snowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

}

void Assignment1::Destroy()
{

}

void Assignment1::SetupScene()
{
	// get number of dimensions from user
	int dimensions = 257;

	// get max height of terrain
	m_terrainGen->SetMaxHeight(80.0f);

	SetupTerrainShader();
	GenerateTerrain(dimensions, 1);

	SetupSkyboxShader();
	LoadSkybox();

}

void Assignment1::DrawSkybox()
{
	glDepthMask(GL_FALSE);
	glUseProgram(m_skyboxShader);

	unsigned int projectionUniform = glGetUniformLocation(m_skyboxShader,
		"Projection"); // find uniform values in shaders
	glUniformMatrix4fv(projectionUniform, 1, 
		false, &m_camera->getProjection()[0][0]); // set uniform values in shaders

	unsigned int viewUniform = glGetUniformLocation(m_skyboxShader,
		"View"); // find uniform values in shaders
	glUniformMatrix4fv(viewUniform, 1,
		false, &m_camera->getView()[0][0]); // set uniform values in shaders

	glBindVertexArray(m_skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(m_skyboxShader, "skybox"), 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
}

void Assignment1::SetupSkyboxShader()
{
	m_skyboxShader = m_shaderManager->LoadShader(
		"Skybox", 
		"../data/shaders/skybox.vert", 
		"../data/shaders/skybox.frag");
}

void Assignment1::SetupTerrainShader()
{
	m_terrainShader = m_shaderManager->LoadShader(
		"Terrain", 
		"../data/shaders/a1Terrain.vert", 
		"../data/shaders/a1Terrain.frag");
}

void Assignment1::LoadSkybox()
{
	float skyboxVertices[] = {
		// Positions          
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,

		-1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f
	};

	std::vector<char*> faces;
	faces.push_back("../data/textures/skybox/right.bmp");
	faces.push_back("../data/textures/skybox/left.bmp");
	faces.push_back("../data/textures/skybox/top.bmp");
	faces.push_back("../data/textures/skybox/bottom.bmp");
	faces.push_back("../data/textures/skybox/back.bmp");
	faces.push_back("../data/textures/skybox/front.bmp");

	glGenVertexArrays(1, &m_skyboxVAO);
	glGenBuffers(1, &m_skyboxVBO);
	glBindVertexArray(m_skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glBindVertexArray(0);

	glGenTextures(1, &m_skyboxTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skyboxTexture);

	int width, height, imageformat;
	unsigned char* image;

	for (int i = 0; i < faces.size(); i++)
	{
		image = stbi_load(faces[i], &width, &height, &imageformat, STBI_default);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Assignment1::GenerateTerrain(unsigned int dimensions, int method)
{
	if (method)
	{
		// ask for roughness and smoothing
		m_terrainData = m_terrainGen->GenerateDiamondSquare(dimensions, 0.3f, true);
	}
	else
	{
		// ask how many octaves
		m_terrainData = m_terrainGen->GeneratePerlin(dimensions, 6);
	}

	glGenTextures(1, &m_terrainTextureMap);
	glBindTexture(GL_TEXTURE_2D, m_terrainTextureMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimensions, dimensions, 0, GL_RED, GL_FLOAT, m_terrainData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GenerateGrid(dimensions, dimensions);
}

void Assignment1::GenerateGrid(unsigned int rows, unsigned int cols)
{
	terrainVert* aoVertices = new terrainVert[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, m_terrainData[r * cols + c], (float)r, 1);
			aoVertices[r * cols + c].normal = vec4(0, 1, 0, 1);
			aoVertices[r * cols + c].texIndex = vec2(((float)r / (rows - 1)), ((float)c / (cols - 1)));
		}
	}

	// calculate normals
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			vec3 a = aoVertices[r * cols + c].position.xyz;
			vec3 b;
			vec3 d;
			if (r != rows - 1 && c != cols - 1)
			{
				b = aoVertices[(r + 1) * cols + c].position.xyz;
				d = aoVertices[(r + 1) * cols + (c + 1)].position.xyz;
				aoVertices[r * cols + c].normal = vec4(glm::normalize(glm::cross(b - a, d - a)), 1);
			}
		}
	}



	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];

	// creates index array
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);

			// triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	m_terrainIndeces = index;

	// create and bind buffers to a vertex array object
	glGenVertexArrays(1, &m_ter_VAO);
	glGenBuffers(1, &m_ter_VBO);
	glGenBuffers(1, &m_ter_IBO);

	glBindVertexArray(m_ter_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_ter_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ter_IBO);

	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(terrainVert), aoVertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVert), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVert), (void*)(sizeof(vec4)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(terrainVert), (void*)(2 * sizeof(vec4)));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}