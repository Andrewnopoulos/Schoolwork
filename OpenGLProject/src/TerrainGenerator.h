#pragma once

#include <stdlib.h>
#include <glm\glm.hpp>

class TerrainGenerator
{
public:

	TerrainGenerator(float a_heightMax);

	void SetMaxHeight(float a_heightMax);

	inline float GetMaxHeight()
	{
		return heightMax;
	}

	inline glm::vec3 GetHighestPoint()
	{
		return maxLocation;
	}

	/*
		Generates an array of size dimensions x dimensions using the Perlin Noise generation technique
		Changing the number of octaves changes the level of detail in the perlin noise function
	*/
	float* GeneratePerlin(const int dimensions, const int octaves);

	/*
		Generates an array of size dimensions x dimensions using the Diamond Square algorithm
		dimensions must be a power of 2 plus 1 otherwise the algorithm will not function successfully.
		'roughness' controls the size of the random discrepencies while 'smoothing' determines whether
		or not to apply an approximation of a Gaussian Blur to the array.
	*/
	float* GenerateDiamondSquare(const int dimensions, const float roughness, const bool smoothing);
private:

	glm::vec3 maxLocation;
	float heightMax;
	// returns a random float located between low and high.
	const float RandomNumber(const float low, const float high)
	{
		return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	}

	void Normalize(float * value, unsigned int size);

};