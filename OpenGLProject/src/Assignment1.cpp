#include "Assignment1.h"
#include <stb_image.h>
#include <vector>

#include <AntTweakBar.h>

void Assignment1::Startup()
{
	m_dimension = 257;
	m_LightDir = vec3(0, 1, 0);
	m_cameraSpeed = 10;
	m_cameraRotation = 0.1f;
	m_gaussianSmoothing = true;
	m_diamondSquareOn = true;
	m_maxHeight = 100.0f;
	m_terrainRoughness = 0.2f;
	m_generatorRoughness = 0.3f;
	m_perlinOctaves = 6;
	m_waterHeight = 10.0f;
	m_seed = 0;

	m_terrainGen = new TerrainGenerator(m_maxHeight);

	m_camera = new FlyCamera();

	m_camera->SetInputWindow(window);

	m_camera->setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	m_camera->setLookAt(vec3(10, 30, 10), vec3(100, 0, 100), vec3(0, 1, 0));

	m_camera->setSpeed(m_cameraSpeed);
	m_camera->setRotationSpeed(m_cameraRotation);

	previousTime = (float)glfwGetTime();

	LoadAssets();

	SetupScene();

	SetupAntTweakBar();
}

void Assignment1::Update()
{
	m_camera->setSpeed(m_cameraSpeed);
	m_camera->setRotationSpeed(m_cameraRotation);

	float currentTime = (float)glfwGetTime();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	m_camera->update(deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		SetupScene();
	}
}

void Assignment1::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawSkybox();

	DrawTerrain();

	DrawWater();

	DrawRocks();

	m_snowEmitter->draw(previousTime,
		m_camera->getWorldTransform(),
		m_camera->getProjectionView());

	TwDraw();
}

void OnMouseButton(GLFWwindow*, int b, int a, int m) {
	TwEventMouseButtonGLFW(b, a);
}
void OnMousePosition(GLFWwindow*, double x, double y) {
	TwEventMousePosGLFW((int)x, (int)y);
}
void OnMouseScroll(GLFWwindow*, double x, double y) {
	TwEventMouseWheelGLFW((int)y);
}
void OnKey(GLFWwindow*, int k, int s, int a, int m) {
	TwEventKeyGLFW(k, a);
}
void OnChar(GLFWwindow*, unsigned int c) {
	TwEventCharGLFW(c, GLFW_PRESS);
}
void OnWindowResize(GLFWwindow*, int w, int h) {
	TwWindowSize(w, h);
	glViewport(0, 0, w, h);
}

void Assignment1::SetupAntTweakBar()
{
	TwInit(TW_OPENGL_CORE, nullptr);
	TwWindowSize(1280, 720);

	m_gui = TwNewBar("Interface");

	TwAddVarRW(m_gui, "Light Direction", TW_TYPE_DIR3F, &m_LightDir, "group=camera");
	TwAddVarRW(m_gui, "Max Height", TW_TYPE_FLOAT, &m_maxHeight, "group=terrain");
	TwAddVarRW(m_gui, "Diamond Square", TW_TYPE_BOOLCPP, &m_diamondSquareOn, "group=terrain");
	TwAddVarRW(m_gui, "Texture roughness", TW_TYPE_FLOAT, &m_terrainRoughness, "group=terrain");
	TwAddVarRW(m_gui, "Generator roughness", TW_TYPE_FLOAT, &m_generatorRoughness, "group=terrain");
	TwAddVarRW(m_gui, "Camera Speed", TW_TYPE_FLOAT, &m_cameraSpeed, "group=camera");
	TwAddVarRW(m_gui, "Camera Rotation", TW_TYPE_FLOAT, &m_cameraRotation, "group=camera");
	TwAddVarRW(m_gui, "Perlin Octaves", TW_TYPE_UINT16, &m_perlinOctaves, "group=terrain");
	TwAddVarRW(m_gui, "Gaussian smoothing", TW_TYPE_BOOLCPP, &m_gaussianSmoothing, "group=terrain");
	TwAddVarRW(m_gui, "Water Height", TW_TYPE_FLOAT, &m_waterHeight, "group=terrain");
	TwAddVarRW(m_gui, "Random Seed", TW_TYPE_UINT16, &m_seed, "group=terrain");

	glfwSetMouseButtonCallback(window, OnMouseButton);
	glfwSetCursorPosCallback(window, OnMousePosition);
	glfwSetScrollCallback(window, OnMouseScroll);
	glfwSetKeyCallback(window, OnKey);
	glfwSetCharCallback(window, OnChar);
	glfwSetWindowSizeCallback(window, OnWindowResize);
}

void Assignment1::DrawTerrain()
{
	glUseProgram(m_terrainShader);
	unsigned int projectionViewUniform = glGetUniformLocation(m_terrainShader, "ProjectionView"); // find uniform values in shaders
	glUniformMatrix4fv(projectionViewUniform, 1, false, &m_camera->getProjectionView()[0][0]); // set uniform values in shaders

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_terrainTextureMap);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_dirtTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_grassTexture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_snowTexture);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_sandTexture);

	unsigned int texloc = glGetUniformLocation(m_terrainShader, "heightMap");
	glUniform1i(texloc, 0);

	texloc = glGetUniformLocation(m_terrainShader, "dirtTexture");
	glUniform1i(texloc, 1);

	texloc = glGetUniformLocation(m_terrainShader, "grassTexture");
	glUniform1i(texloc, 2);

	texloc = glGetUniformLocation(m_terrainShader, "snowTexture");
	glUniform1i(texloc, 3);

	texloc = glGetUniformLocation(m_terrainShader, "sandTexture");
	glUniform1i(texloc, 4);

	texloc = glGetUniformLocation(m_terrainShader, "maxHeight");
	glUniform1f(texloc, m_terrainGen->GetMaxHeight());

	texloc = glGetUniformLocation(m_terrainShader, "roughness");
	glUniform1f(texloc, m_terrainRoughness);

	texloc = glGetUniformLocation(m_terrainShader, "fresnel");
	glUniform1f(texloc, 1.0f);

	mat4 camera_matrix = m_camera->getWorldTransform();
	texloc = glGetUniformLocation(m_terrainShader, "CameraPos");
	glUniform3f(texloc, camera_matrix[3][0],
		camera_matrix[3][1], camera_matrix[3][2]);

	texloc = glGetUniformLocation(m_terrainShader, "LightDir");
	glUniform3f(texloc, m_LightDir.x, m_LightDir.y, m_LightDir.z);

	texloc = glGetUniformLocation(m_terrainShader, "waterLevel");
	glUniform1f(texloc, m_waterHeight);

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

	glGenTextures(1, &m_dirtTexture);
	glBindTexture(GL_TEXTURE_2D, m_dirtTexture);
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

	data = stbi_load("../data/textures/sand.jpg",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_sandTexture);
	glBindTexture(GL_TEXTURE_2D, m_sandTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);


	data = stbi_load("../data/textures/water512.jpg",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_waterTexture);
	glBindTexture(GL_TEXTURE_2D, m_waterTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	stbi_image_free(data);

}

void Assignment1::Destroy()
{
	TwDeleteAllBars();
	TwTerminate();
}

void Assignment1::LoadAssets()
{
	m_rock1 = new FBXObject("../data/trees/rock.fbx");
	m_rock1->SetupShader(m_shaderManager,
		"../data/shaders/tree.vert",
		"../data/shaders/tree.frag");
	m_rock1->LoadDiffuse("../data/trees/rockDiffuse.tga");
	m_rock1->LoadNormals("../data/trees/rockNormal.tga");

	m_rock2 = new FBXObject("../data/trees/Rock2.fbx");
	m_rock2->SetupShader(m_shaderManager,
		"../data/shaders/tree.vert",
		"../data/shaders/tree.frag");
	m_rock2->LoadDiffuse("../data/trees/Rock2.jpg");

	m_spire = new FBXObject("../data/trees/stone.fbx");
	m_spire->SetupShader(m_shaderManager,
		"../data/shaders/tree.vert",
		"../data/shaders/tree.frag");
	m_spire->LoadDiffuse("../data/trees/stone.jpg");

	LoadSkybox();

	LoadTextures();

}

void Assignment1::SetupScene()
{

	srand(m_seed);
	// get number of dimensions from user

	// get max height of terrain
	m_terrainGen->SetMaxHeight(m_maxHeight);

	SetupTerrainShader();
	GenerateTerrain(m_dimension, m_diamondSquareOn);

	SetupSkyboxShader();
	LoadSkybox();

	SetupRocks();

	SetupSnow();

	SetupWaterShader();
	GenerateWaterVerts(m_dimension, m_dimension);
	
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

void Assignment1::SetupRocks()
{
	// set up matrices

	m_rock1Locations.clear();
	m_rock2Locations.clear();
	m_spireLocations.clear();

	for (unsigned int i = 0; i < 20; i++)
	{
		vec3 position = vec3(rand() % m_dimension, 0, rand() % m_dimension);
		unsigned int index = position.x + m_dimension * position.z;
		position.y = m_terrainData[index];
		mat4 tempMat = glm::translate(position);
		tempMat = glm::scale(tempMat, vec3(0.027f));
		tempMat = glm::rotate(tempMat, (float)(rand() % 360), glm::normalize(vec3(rand() % 10, rand() % 10, rand() % 10)));
		m_rock1Locations.push_back(tempMat);
	}

	for (unsigned int i = 0; i < 50; i++)
	{
		vec3 position = vec3(rand() % m_dimension, 0, rand() % m_dimension);
		unsigned int index = position.x + m_dimension * position.z;
		position.y = m_terrainData[index];
		mat4 tempMat = glm::translate(position);
		tempMat = glm::scale(tempMat, vec3(1.0f));
		tempMat = glm::rotate(tempMat, (float)(rand() % 360), glm::normalize(vec3(rand() % 10, rand() % 10, rand() % 10)));
		m_rock2Locations.push_back(tempMat);
	}

	for (unsigned int i = 0; i < 4; i++)
	{
		vec3 position = vec3(rand() % m_dimension, 0, rand() % m_dimension);
		unsigned int index = position.x + m_dimension * position.z;
		position.y = m_terrainData[index] + 4;
		mat4 tempMat = glm::translate(position);
		tempMat = glm::scale(tempMat, vec3(0.05f, 0.1f, 0.05f));
		tempMat = glm::rotate(tempMat, (float)(rand() % 360), vec3(0, 1, 0));
	//	tempMat = glm::rotate(tempMat, (float)(rand() % 360), glm::normalize(vec3(rand() % 10, rand() % 10, rand() % 10)));
		m_spireLocations.push_back(tempMat);
	}

	//testMat = glm::translate(vec3(0, 50, 0));
	//testMat = glm::scale(testMat, vec3(0.05f, 0.05f, 0.05f));
	//testMat = glm::rotate(testMat, glm::pi<float>() / 2.0f, vec3(-1, 0, 0));

	//testMat2 = glm::translate(vec3(0, 25, 0));
	//testMat2 = glm::scale(testMat2, vec3(0.05f, 0.05f, 0.05f));
	//testMat2 = glm::rotate(testMat2, glm::pi<float>() / 2.0f, vec3(-1, 0, 0));
}

void Assignment1::DrawRocks()
{
	for (auto i = m_rock1Locations.begin(); i != m_rock1Locations.end(); i++)
	{
		m_rock1->Render(m_camera, m_LightDir, (*i));
	}

	for (auto i = m_rock2Locations.begin(); i != m_rock2Locations.end(); i++)
	{
		m_rock2->Render(m_camera, m_LightDir, (*i));
	}

	for (auto i = m_spireLocations.begin(); i != m_spireLocations.end(); i++)
	{
		m_spire->Render(m_camera, m_LightDir, (*i));
	}
}

void Assignment1::SetupSnow()
{
	vec3 highestPoint = m_terrainGen->GetHighestPoint();

	m_snowEmitter = new SnowEmitter(highestPoint);

	m_snowEmitter->initialise(100000,	// number
		1.0f, 50.0f,					// min/max lifespan
		5, 20,							// min/max velocity
		0.1f, 0.06f,						// starting/ending size
		vec4(1, 1, 1, 1), vec4(0.9f, 0.9f, 1, 1));	// starting/ending colour
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

void Assignment1::GenerateTerrain(unsigned int dimensions, bool method)
{
	if (method)
	{
		// ask for roughness and smoothing
		m_terrainData = m_terrainGen->GenerateDiamondSquare(dimensions, m_generatorRoughness, m_gaussianSmoothing);
	}
	else
	{
		// ask how many octaves
		m_terrainData = m_terrainGen->GeneratePerlin(dimensions, m_perlinOctaves);
	}

	glGenTextures(1, &m_terrainTextureMap);
	glBindTexture(GL_TEXTURE_2D, m_terrainTextureMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimensions, dimensions, 0, GL_RED, GL_FLOAT, m_terrainData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GenerateTerrainVerts(dimensions, dimensions);
}

void Assignment1::GenerateTerrainVerts(unsigned int rows, unsigned int cols)
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

void Assignment1::GenerateWaterVerts(unsigned int rows, unsigned int cols)
{
	terrainVert* aoVertices = new terrainVert[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = vec4((float)c, m_waterHeight, (float)r, 1);
			aoVertices[r * cols + c].normal = vec4(0, 1, 0, 1);
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

	m_waterIndices = index;

	// create and bind buffers to a vertex array object
	glGenVertexArrays(1, &m_water_VAO);
	glGenBuffers(1, &m_water_VBO);
	glGenBuffers(1, &m_water_IBO);

	glBindVertexArray(m_water_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_water_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_water_IBO);

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

void Assignment1::SetupWaterShader()
{
	m_waterShader = m_shaderManager->LoadShader(
		"Water",
		"../data/shaders/a1Water.vert",
		"../data/shaders/a1Water.frag");
}

void Assignment1::DrawWater()
{
	glUseProgram(m_waterShader);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int projectionViewUniform = glGetUniformLocation(m_waterShader, "ProjectionView"); // find uniform values in shaders
	glUniformMatrix4fv(projectionViewUniform, 1, false, &m_camera->getProjectionView()[0][0]); // set uniform values in shaders

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_waterTexture);

	unsigned int texloc = glGetUniformLocation(m_waterShader, "waterTexture");
	glUniform1i(texloc, 0);

	texloc = glGetUniformLocation(m_waterShader, "time");
	glUniform1f(texloc, previousTime);

	texloc = glGetUniformLocation(m_waterShader, "heightScale");
	glUniform1f(texloc, 0.3f);

	texloc = glGetUniformLocation(m_waterShader, "periodScale");
	glUniform1f(texloc, 0.6f);

	mat4 camera_matrix = m_camera->getWorldTransform();
	texloc = glGetUniformLocation(m_waterShader, "CameraPos");
	glUniform3f(texloc, camera_matrix[3][0],
		camera_matrix[3][1], camera_matrix[3][2]);

	texloc = glGetUniformLocation(m_waterShader, "LightDir");
	glUniform3f(texloc, m_LightDir.x, m_LightDir.y, m_LightDir.z);

	texloc = glGetUniformLocation(m_waterShader, "SpecPow");
	glUniform1f(texloc, 32.0f);

	glBindVertexArray(m_water_VAO);

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe

	glDrawElements(GL_TRIANGLES, m_waterIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDisable(GL_BLEND);
}