#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include "TerrainGenerator.h"
#include "ShaderManager.h"
#include "FBXObject.h"
#include "SnowEmitter.h"
#include <vector>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::vector;

struct terrainVert
{
	vec4 position;
	vec4 normal;
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

	unsigned int m_dimension;
	vec3 m_LightDir;

	unsigned int m_terrainIndeces;
	unsigned int m_ter_VAO;
	unsigned int m_ter_VBO;
	unsigned int m_ter_IBO;

	float* m_terrainData;
	unsigned int m_terrainTextureMap;
	unsigned int m_terrainShader;

	unsigned int m_waterIndices;
	unsigned int m_water_VAO;
	unsigned int m_water_VBO;
	unsigned int m_water_IBO;

	unsigned int m_waterShader;

	unsigned int m_waterTexture;

	void GenerateWaterVerts(unsigned int rows, unsigned int cols);
	void SetupWaterShader();
	void DrawWater();

	void SetupScene();
	void GenerateTerrain(unsigned int dimensions, int method);
	void GenerateTerrainVerts(unsigned int rows, unsigned int cols);
	void SetupTerrainShader();
	void DrawTerrain();

	unsigned int m_grassTexture;
	unsigned int m_dirtTexture;
	unsigned int m_snowTexture;
	unsigned int m_sandTexture;

	void LoadTextures();

	//Skybox stuff
	void SetupSkyboxShader();
	void LoadSkybox();
	void DrawSkybox();

	unsigned int m_skyboxShader;
	unsigned int m_skyboxTexture;

	unsigned int m_skyboxVAO, m_skyboxVBO;

	FBXObject* m_rock1;
	FBXObject* m_rock2;
	FBXObject* m_spire;

	vector<mat4> m_rock1Locations;
	vector<mat4> m_rock2Locations;
	vector<mat4> m_spireLocations;

	void SetupRocks();
	void DrawRocks();

	void SetupSnow();
	SnowEmitter* m_snowEmitter;

};