#include "TerrainGenerator.h"
#include <glm\glm.hpp>
#include "glm\gtc\noise.hpp"
#include <string>
#include <list>
#include <stdexcept>

TerrainGenerator::TerrainGenerator(float a_heightMax)
{
	SetMaxHeight(a_heightMax);
}

void TerrainGenerator::SetMaxHeight(float a_heightMax)
{
	heightMax = a_heightMax;
}

void TerrainGenerator::Normalize(float * data, unsigned int size)
{
	float largestOffset = 0;
	// find largest offset
	for (int i = 0; i < size; i++)
	{
		if (abs(data[i]) > largestOffset)
		{
			largestOffset = abs(data[i]);
		}
	}

	// normalize and multiply by height max
	for (int i = 0; i < size; i++)
	{
		data[i] /= largestOffset;
		data[i] *= heightMax;
	}

}

float* TerrainGenerator::GeneratePerlin(const int dimension, const int octaves)
{
	float* perlin_data = new float[dimension * dimension];
	float scale = (1.0f / dimension) * 3;
	for (int x = 0; x < dimension; ++x)
	{
		for (int y = 0; y < dimension; ++y)
		{
			float amplitude = 1.0f;
			float persistence = 0.3f;
			perlin_data[y*dimension + x] = 0;

			for (int o = 0; o < octaves; ++o)
			{
				float freq = powf(2, (float)o);
				float perlin_sample =
					glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y * dimension + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}

	Normalize(perlin_data, dimension * dimension);

	return perlin_data;
}

float* TerrainGenerator::GenerateDiamondSquare(const int dimension, const float roughness, const bool smoothing)
{
	int dminus1 = dimension - 1;
	
	// this will be zero if it's a power of 2, non-zero if it isn't
	int power2minus1 = dminus1 & (dminus1 - 1);

	// if true, it's not a power of 2, therefore throw an exception
	if (power2minus1)
	{
		throw std::invalid_argument("dimension not a power of 2 + 1");
	}

	float* diamond_square_data = new float[dimension * dimension];
	memset(diamond_square_data, 0, sizeof(float) * dimension * dimension);

	float* diamond_square_smooth = new float[dimension * dimension];
	memset(diamond_square_smooth, 0, sizeof(float)* dimension * dimension);

	// square data points
	struct DiamondSquareData
	{
		unsigned int uiTopLeftIndex;
		unsigned int uiTopRightIndex;
		unsigned int uiBottomLeftIndex;
		unsigned int uiBottomRightIndex;
	};

	// list of squares to be processed
	std::list<DiamondSquareData> diamondList;

	// initial square data points
	DiamondSquareData d;
	d.uiTopLeftIndex = 0;
	d.uiTopRightIndex = dimension - 1;
	d.uiBottomLeftIndex = (dimension * (dimension - 1));
	d.uiBottomRightIndex = (dimension * dimension) - 1;

	diamondList.push_front(d);
	DiamondSquareData init = diamondList.front();

	// random seeding of corners
	diamond_square_data[init.uiTopLeftIndex] = RandomNumber(-roughness, roughness);
	diamond_square_data[init.uiTopRightIndex] = RandomNumber(-roughness, roughness);
	diamond_square_data[init.uiBottomLeftIndex] = RandomNumber(-roughness, roughness);
	diamond_square_data[init.uiBottomRightIndex] = RandomNumber(-roughness, roughness);

	while (!diamondList.empty())
	{

		DiamondSquareData dtop = diamondList.front();
		diamondList.pop_front();

		unsigned int sideLength = dtop.uiTopRightIndex - dtop.uiTopLeftIndex;

		unsigned int halflength = sideLength / 2;

		if (sideLength >= 2)
		{
			// random offsets are proportional to the sidelength of the square
			float randomRange = roughness * sideLength;

			unsigned int middleIndex = (dtop.uiTopLeftIndex + dtop.uiBottomRightIndex) / 2;

			// calculate middle index value by averaging corners of square and adding random offset
			diamond_square_data[middleIndex] =
				(diamond_square_data[dtop.uiTopLeftIndex] +
				diamond_square_data[dtop.uiTopRightIndex] +
				diamond_square_data[dtop.uiBottomLeftIndex] +
				diamond_square_data[dtop.uiBottomRightIndex]) / 4 + RandomNumber(-randomRange, randomRange);

			// calculate midpoints of the sides of the square
			unsigned int topIndex = dtop.uiTopLeftIndex + halflength;
			unsigned int leftIndex = middleIndex - halflength;
			unsigned int rightIndex = middleIndex + halflength;
			unsigned int bottomIndex = dtop.uiBottomLeftIndex + halflength;

			// calculate outside corners of diamonds
			int outTop = (middleIndex - dimension * sideLength);
			int outBot = (middleIndex + dimension * sideLength);
			int outLeft = (middleIndex - sideLength);
			int outRight = (middleIndex + sideLength);

			// check if diamond extends beyond bounds of the terrain and calculate midpoints accordingly
			if (outTop < 0)
			{
				diamond_square_data[topIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiTopRightIndex]) / 3 + RandomNumber(-randomRange, randomRange);
			}
			else
			{
				diamond_square_data[topIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiTopRightIndex] +
					diamond_square_data[outTop]) / 4 + RandomNumber(-randomRange, randomRange);
			}

			if (outBot >= (dimension * dimension))
			{
				diamond_square_data[bottomIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex] +
					diamond_square_data[dtop.uiBottomRightIndex]) / 3 + RandomNumber(-randomRange, randomRange);
			}
			else
			{
				diamond_square_data[bottomIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex] +
					diamond_square_data[dtop.uiBottomRightIndex] +
					diamond_square_data[outBot]) / 4 + RandomNumber(-randomRange, randomRange);
			}

			int horizmin = (int)(middleIndex / dimension) * dimension;
			int horizmax = horizmin + dimension - 1;

			if (outLeft < horizmin)
			{
				diamond_square_data[leftIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex]) / 3 + RandomNumber(-randomRange, randomRange);
			}
			else
			{
				diamond_square_data[leftIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex] +
					diamond_square_data[outLeft]) / 4 + RandomNumber(-randomRange, randomRange);
			}

			if (outRight > horizmax)
			{
				diamond_square_data[rightIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomRightIndex] +
					diamond_square_data[dtop.uiTopRightIndex]) / 3 + RandomNumber(-randomRange, randomRange);
			}
			else
			{
				diamond_square_data[rightIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomRightIndex] +
					diamond_square_data[dtop.uiTopRightIndex] +
					diamond_square_data[outRight]) / 4 + RandomNumber(-randomRange, randomRange);
			}

			// if we can go deeper, push more squares on
			if (sideLength >= 3)
			{
				DiamondSquareData topLeft;
				DiamondSquareData topRight;
				DiamondSquareData bottomLeft;
				DiamondSquareData bottomRight;

				topLeft.uiTopLeftIndex = dtop.uiTopLeftIndex;
				topLeft.uiBottomRightIndex = middleIndex;
				topLeft.uiTopRightIndex = topIndex;
				topLeft.uiBottomLeftIndex = leftIndex;

				topRight.uiTopLeftIndex = topIndex;
				topRight.uiTopRightIndex = dtop.uiTopRightIndex;
				topRight.uiBottomLeftIndex = middleIndex;
				topRight.uiBottomRightIndex = rightIndex;

				bottomLeft.uiTopLeftIndex = leftIndex;
				bottomLeft.uiTopRightIndex = middleIndex;
				bottomLeft.uiBottomLeftIndex = dtop.uiBottomLeftIndex;
				bottomLeft.uiBottomRightIndex = bottomIndex;

				bottomRight.uiTopLeftIndex = middleIndex;
				bottomRight.uiTopRightIndex = rightIndex;
				bottomRight.uiBottomLeftIndex = bottomIndex;
				bottomRight.uiBottomRightIndex = dtop.uiBottomRightIndex;

				diamondList.push_back(topLeft);
				diamondList.push_back(bottomLeft);
				diamondList.push_back(topRight);
				diamondList.push_back(bottomRight);
			}
		}
	}

	Normalize(diamond_square_data, dimension * dimension);

	if (smoothing)
	{
		for (int i = 0; i < dimension; i++)
		{
			for (int j = 0; j < dimension; j++)
			{
				if (i == 0 || j == 0 || i == dimension - 1 || j == dimension - 1)
				{
					diamond_square_smooth[i + dimension*j] = diamond_square_data[i + dimension*j];
				}
				else
				{
					float tex = diamond_square_data[i + dimension*j];
					tex = tex * 4;
					tex += diamond_square_data[(i - 1) + dimension*(j - 1)];
					tex += diamond_square_data[(i - 1) + dimension*(j)] * 2;
					tex += diamond_square_data[(i - 1) + dimension*(j + 1)];
					tex += diamond_square_data[(i)+dimension*(j - 1)] * 2;
					tex += diamond_square_data[(i)+dimension*(j + 1)] * 2;
					tex += diamond_square_data[(i + 1) + dimension*(j - 1)];
					tex += diamond_square_data[(i + 1) + dimension*(j)] * 2;
					tex += diamond_square_data[(i + 1) + dimension*(j + 1)];
					tex = tex / 16.0f;
					diamond_square_smooth[i + dimension*j] = tex;
				}
			}
		}
		return diamond_square_smooth;
	}
	else
	{
		return diamond_square_data;
	}

}