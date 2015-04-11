#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "TerrainGenerator.h"
#include "ShaderManager.h"

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

struct terrainVert
{
	vec4 position;
	vec2 texIndex;
};

class Assignment1 : public Application
{
public:
	virtual void Startup();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

private:
	FlyCamera* m_camera;

	TerrainGenerator* m_terrainGen;
	
	float previousTime;
	float deltaTime;

	unsigned int m_terrainIndeces;
	unsigned int m_ter_VAO;
	unsigned int m_ter_VBO;
	unsigned int m_ter_IBO;

	unsigned int m_terrainTextureMap;
	unsigned int m_terrainShader;

	void SetupScene();
	void GenerateTerrain(unsigned int dimensions, int method);
	void GenerateGrid(unsigned int rows, unsigned int cols);
	void SetupTerrainShader();
	void DrawTerrain();

	unsigned int m_grassTexture;
	unsigned int m_sandTexture;
	unsigned int m_snowTexture;

	void LoadTextures();
	
};