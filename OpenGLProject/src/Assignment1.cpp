#include "Assignment1.h"

void Assignment1::Startup()
{
	m_terrainGen = new TerrainGenerator(5.0f);

	m_camera = new FlyCamera();

	m_camera->SetInputWindow(window);

	m_camera->setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	m_camera->setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	m_camera->setSpeed(10);
	m_camera->setRotationSpeed(10);

	previousTime = (float)glfwGetTime();

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
	DrawTerrain();
}

void Assignment1::DrawTerrain()
{
	glUseProgram(m_terrainShader);
	unsigned int projectionViewUniform = glGetUniformLocation(m_terrainShader, "ProjectionView"); // find uniform values in shaders
	glUniformMatrix4fv(projectionViewUniform, 1, false, &m_camera->getProjectionView()[0][0]); // set uniform values in shaders

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_terrainTextureMap);

	unsigned int texloc = glGetUniformLocation(m_terrainShader, "perlin_texture");
	glUniform1i(texloc, 0);

	glBindVertexArray(m_ter_VAO);

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe

	glDrawElements(GL_TRIANGLES, m_terrainIndeces, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Assignment1::Destroy()
{

}

void Assignment1::SetupScene()
{
	// get number of dimensions from user
	int dimensions = 257;

	// get max height of terrain
	m_terrainGen->SetMaxHeight(5.0f);

	SetupTerrainShader();
	GenerateTerrain(dimensions, 1);
	GenerateGrid(dimensions, dimensions);
}

void Assignment1::SetupTerrainShader()
{
	m_terrainShader = m_shaderManager->LoadShader("Terrain", "../data/shaders/a1Terrain.vert", "../data/shaders/a1Terrain.frag");
}

void Assignment1::GenerateTerrain(unsigned int dimensions, int method)
{
	float* terrain_data;
	if (method)
	{
		// ask for roughness and smoothing
		terrain_data = m_terrainGen->GenerateDiamondSquare(dimensions, 0.03f, false);
	}
	else
	{
		// ask how many octaves
		terrain_data = m_terrainGen->GeneratePerlin(dimensions, 6);
	}

	glGenTextures(1, &m_terrainTextureMap);
	glBindTexture(GL_TEXTURE_2D, m_terrainTextureMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimensions, dimensions, 0, GL_RED, GL_FLOAT, terrain_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Assignment1::GenerateGrid(unsigned int rows, unsigned int cols)
{
	terrainVert* aoVertices = new terrainVert[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, 0, (float)r, 1);
			aoVertices[r * cols + c].texIndex = vec2(((float)r / (rows - 1)), ((float)c / (cols - 1)));
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

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(terrainVert), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(terrainVert), (void*)(sizeof(vec4)));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}