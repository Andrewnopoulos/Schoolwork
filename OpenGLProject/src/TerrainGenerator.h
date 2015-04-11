#pragma once

#include <stdlib.h>

class TerrainGenerator
{
public:

	/*
		Generates an array of size dimensions x dimensions using the Perlin Noise generation technique
		Changing the number of octaves changes the level of detail in the perlin noise function
	*/
	const float* GeneratePerlin(const int dimensions, const int octaves);

	/*
		Generates an array of size dimensions x dimensions using the Diamond Square algorithm
		dimensions must be a power of 2 plus 1 otherwise the algorithm will not function successfully.
		'roughness' controls the size of the random discrepencies while 'smoothing' determines whether
		or not to apply an approximation of a Gaussian Blur to the array.
	*/
	const float* GenerateDiamondSquare(const int dimensions, const float roughness, const bool smoothing);
private:

	// returns a random float located between low and high.
	const float RandomNumber(const float low, const float high)
	{
		return low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (high - low)));
	}
};