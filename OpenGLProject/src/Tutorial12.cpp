#include "Tutorial12.h"
#include "OBJLoader.h"
#include "tiny_obj_loader.h"
#include "ShaderManager.h"

#include <stack>
#include <vector>
#include <list>
#include <algorithm>    // std::random_shuffle

void Tutorial12::generatePerlin(unsigned int dimension)
{
	float* perlin_data = new float[dimension * dimension];
	float scale = (1.0f / dimension) * 3;
	int octaves = 6;
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
					glm::perlin(vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;
				perlin_data[y * dimension + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimension, dimension, 0, GL_RED, GL_FLOAT, perlin_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

float Tutorial12::randomNumber(float LO, float HI)
{
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

void Tutorial12::generateDiamondSquare(unsigned int dimension)
{
	float* diamond_square_data = new float[dimension * dimension];
	memset(diamond_square_data, 0, sizeof(float) * dimension * dimension);

	float* diamond_square_temp = new float[dimension * dimension];
	memset(diamond_square_temp, 0, sizeof(float)* dimension * dimension);

	float initials = 0.7f;
	float roughness = 0.03f;

	struct DiamondSquareData
	{
		unsigned int uiTopLeftIndex;
		unsigned int uiTopRightIndex;
		unsigned int uiBottomLeftIndex;
		unsigned int uiBottomRightIndex;
	};

	std::stack<DiamondSquareData> diamondStack;

	std::list<DiamondSquareData> diamondList;

	DiamondSquareData d;
	d.uiTopLeftIndex = 0;
	d.uiTopRightIndex = dimension - 1;
	d.uiBottomLeftIndex = (dimension * (dimension - 1));
	d.uiBottomRightIndex = (dimension * dimension) - 1;

//	diamondStack.push(d);
//	DiamondSquareData init = diamondStack.top();

	diamondList.push_front(d);
	DiamondSquareData init = diamondList.front();

	diamond_square_data[init.uiTopLeftIndex] = randomNumber(-initials, initials);
	diamond_square_data[init.uiTopRightIndex] = randomNumber(-initials, initials);
	diamond_square_data[init.uiBottomLeftIndex] = randomNumber(-initials, initials);
	diamond_square_data[init.uiBottomRightIndex] = randomNumber(-initials, initials);

	while (!diamondList.empty())
	{
//		DiamondSquareData dtop = diamondStack.top();
//		diamondStack.pop();

		DiamondSquareData dtop = diamondList.front();
		diamondList.pop_front();

		unsigned int sideLength = dtop.uiTopRightIndex - dtop.uiTopLeftIndex;

		unsigned int halflength = sideLength / 2;
		
		if (sideLength >= 2)
		{
			float randomRange = roughness * sideLength;

			unsigned int middleIndex = (dtop.uiTopLeftIndex + dtop.uiBottomRightIndex) / 2;

			diamond_square_data[middleIndex] =
				(diamond_square_data[dtop.uiTopLeftIndex] +
				diamond_square_data[dtop.uiTopRightIndex] +
				diamond_square_data[dtop.uiBottomLeftIndex] +
				diamond_square_data[dtop.uiBottomRightIndex]) / 4 + randomNumber(-randomRange, randomRange);

			unsigned int topIndex = dtop.uiTopLeftIndex + halflength;
			unsigned int leftIndex = middleIndex - halflength;
			unsigned int rightIndex = middleIndex + halflength;
			unsigned int bottomIndex = dtop.uiBottomLeftIndex + halflength;

			//diamond_square_data[topIndex] =
			//	//(diamond_square_data[middleIndex] +
			//	(diamond_square_data[dtop.uiTopLeftIndex] +
			//	diamond_square_data[dtop.uiTopRightIndex]) / 2;

			//diamond_square_data[leftIndex] =
			//	//(diamond_square_data[middleIndex] +
			//	(diamond_square_data[dtop.uiTopLeftIndex] +
			//	diamond_square_data[dtop.uiBottomLeftIndex]) / 2;

			//diamond_square_data[rightIndex] =
			//	//(diamond_square_data[middleIndex] +
			//	(diamond_square_data[dtop.uiBottomRightIndex] +
			//	diamond_square_data[dtop.uiTopRightIndex]) / 2;

			//diamond_square_data[bottomIndex] =
			//	//(diamond_square_data[middleIndex] +
			//	(diamond_square_data[dtop.uiBottomLeftIndex] +
			//	diamond_square_data[dtop.uiBottomRightIndex]) / 2;

			int outTop = (middleIndex - dimension * sideLength);
			int outBot = (middleIndex + dimension * sideLength);
			int outLeft = (middleIndex - sideLength);
			int outRight = (middleIndex + sideLength);

			if (outTop < 0)
			{
				//outTop = (dimension * dimension) + outTop;
				diamond_square_data[topIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiTopRightIndex]) / 3 + randomNumber(-randomRange, randomRange);
			}
			else
			{
				diamond_square_data[topIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiTopRightIndex] +
					diamond_square_data[outTop]) / 4 + randomNumber(-randomRange, randomRange);
			}

			if (outBot >= (dimension * dimension))
			{
				//outBot -= (dimension * dimension);
				diamond_square_data[bottomIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex] +
					diamond_square_data[dtop.uiBottomRightIndex]) / 3 + randomNumber(-randomRange, randomRange);
			}
			else
			{
				diamond_square_data[bottomIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex] +
					diamond_square_data[dtop.uiBottomRightIndex] +
					diamond_square_data[outBot]) / 4 + randomNumber(-randomRange, randomRange);
			}

			int horizmin = (int)(middleIndex / dimension) * dimension;
			int horizmax = horizmin + dimension - 1;

			if (outLeft < horizmin)
			{
				//outLeft += dimension;
				diamond_square_data[leftIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex]) / 3 + randomNumber(-randomRange, randomRange);
			}
			else
			{
				diamond_square_data[leftIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiTopLeftIndex] +
					diamond_square_data[dtop.uiBottomLeftIndex] +
					diamond_square_data[outLeft]) / 4 + randomNumber(-randomRange, randomRange);
			}

			if (outRight > horizmax)
			{
				//outRight -= dimension;
				diamond_square_data[rightIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomRightIndex] +
					diamond_square_data[dtop.uiTopRightIndex]) / 3 + randomNumber(-randomRange, randomRange);
			} 
			else
			{
				diamond_square_data[rightIndex] =
					(diamond_square_data[middleIndex] +
					diamond_square_data[dtop.uiBottomRightIndex] +
					diamond_square_data[dtop.uiTopRightIndex] +
					diamond_square_data[outRight]) / 4 + randomNumber(-randomRange, randomRange);
			}

			//if (diamond_square_data[outTop] == 0)
			//{
			//	diamond_square_data[outTop] = randomNumber(-randomRange, randomRange);
			//}
			//if (diamond_square_data[outBot] == 0)
			//{
			//	diamond_square_data[outBot] = randomNumber(-randomRange, randomRange);
			//}
			//if (diamond_square_data[outLeft] == 0)
			//{
			//	diamond_square_data[outLeft] = randomNumber(-randomRange, randomRange);
			//}
			//if (diamond_square_data[outRight] == 0)
			//{
			//	diamond_square_data[outRight] = randomNumber(-randomRange, randomRange);
			//}

			//if (diamond_square_data[outTop] == 0)
			//{
			//	diamond_square_data[outTop] = 
			//		(diamond_square_data[middleIndex] +
			//		diamond_square_data[dtop.uiTopLeftIndex] +
			//		diamond_square_data[dtop.uiTopRightIndex]) / 3;
			//}
			//if (diamond_square_data[outBot] == 0)
			//{
			//	diamond_square_data[outBot] = 
			//		(diamond_square_data[middleIndex] +
			//		diamond_square_data[dtop.uiBottomLeftIndex] +
			//		diamond_square_data[dtop.uiBottomRightIndex]) / 3;
			//}
			//if (diamond_square_data[outLeft] == 0)
			//{
			//	diamond_square_data[outLeft] = 
			//		(diamond_square_data[middleIndex] +
			//		diamond_square_data[dtop.uiTopLeftIndex] +
			//		diamond_square_data[dtop.uiBottomLeftIndex]) / 3;
			//}
			//if (diamond_square_data[outRight] == 0)
			//{
			//	diamond_square_data[outRight] = 
			//		(diamond_square_data[middleIndex] +
			//		diamond_square_data[dtop.uiBottomRightIndex] +
			//		diamond_square_data[dtop.uiTopRightIndex]) / 3;
			//}

			//diamond_square_data[topIndex] =
			//	(diamond_square_data[middleIndex] +
			//	diamond_square_data[dtop.uiTopLeftIndex] +
			//	diamond_square_data[dtop.uiTopRightIndex] + 
			//	diamond_square_data[outTop]) / 4 + randomNumber(-randomRange, randomRange);
			//
			//diamond_square_data[leftIndex] =
			//	(diamond_square_data[middleIndex] +
			//	diamond_square_data[dtop.uiTopLeftIndex] +
			//	diamond_square_data[dtop.uiBottomLeftIndex] +
			//	diamond_square_data[outLeft]) / 4 + randomNumber(-randomRange, randomRange);
			//
			//diamond_square_data[rightIndex] =
			//	(diamond_square_data[middleIndex] +
			//	diamond_square_data[dtop.uiBottomRightIndex] +
			//	diamond_square_data[dtop.uiTopRightIndex] +
			//	diamond_square_data[outRight]) / 4 + randomNumber(-randomRange, randomRange);
			//
			//diamond_square_data[bottomIndex] =
			//	(diamond_square_data[middleIndex] +
			//	diamond_square_data[dtop.uiBottomLeftIndex] +
			//	diamond_square_data[dtop.uiBottomRightIndex] +
			//	diamond_square_data[outBot]) / 4 + randomNumber(-randomRange, randomRange);

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

				//std::vector<DiamondSquareData> tempVec;
				//tempVec.push_back(topLeft);
				//tempVec.push_back(bottomLeft);
				//tempVec.push_back(topRight);
				//tempVec.push_back(bottomRight);

				//std::random_shuffle(tempVec.begin(), tempVec.end());

				//for (auto it = tempVec.begin(); it != tempVec.end(); ++it)
				//{
				//	diamondStack.push(*it);
				//}

				diamondList.push_back(topLeft);
				diamondList.push_back(bottomLeft);
				diamondList.push_back(topRight);
				diamondList.push_back(bottomRight);
			}
		}
	}



	for (int i = 0; i < dimensions; i++)
	{
		for (int j = 0; j < dimensions; j++)
		{
			if (i == 0 || j == 0 || i == dimensions - 1 || j == dimensions - 1)
			{
				diamond_square_temp[i + dimensions*j] = diamond_square_data[i+dimensions*j];
			}
			else
			{
				float tex = diamond_square_data[i + dimensions*j];
				tex = tex * 4;
				tex += diamond_square_data[(i - 1) + dimensions*(j - 1)];
				tex += diamond_square_data[(i - 1) + dimensions*(j)] * 2;
				tex += diamond_square_data[(i - 1) + dimensions*(j + 1)];
				tex += diamond_square_data[(i)+dimensions*(j - 1)] * 2;
				tex += diamond_square_data[(i)+dimensions*(j + 1)] * 2;
				tex += diamond_square_data[(i + 1) + dimensions*(j - 1)];
				tex += diamond_square_data[(i + 1) + dimensions*(j)] * 2;
				tex += diamond_square_data[(i + 1) + dimensions*(j + 1)];
				tex = tex / 16.0f;
				diamond_square_temp[i + dimensions*j] = tex;
			}

			
		}
	}

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimension, dimension, 0, GL_RED, GL_FLOAT, diamond_square_temp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void Tutorial12::generateGrid( unsigned int rows, unsigned int cols ) 
{
	Vertex_b* aoVertices = new Vertex_b[ rows * cols ];
	for( unsigned int r = 0; r < rows; ++r)
	{
		for( unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[ r * cols + c ].position = vec4((float)c, 0, (float)r, 1);
			aoVertices[ r * cols + c ].texIndex = vec2(((float)r / (rows - 1)), ((float)c / (cols - 1)));
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[ (rows - 1) * (cols - 1) * 6 ];

	// creates index array
	unsigned int index = 0;
	for( unsigned int r = 0; r < (rows - 1); ++r)
	{
		for( unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1
			auiIndices[ index++ ] = r * cols + c;
			auiIndices[ index++ ] = (r + 1) * cols + c;
			auiIndices[ index++ ] = (r + 1) * cols + (c + 1);

			// triangle 2
			auiIndices[ index++ ] = r * cols + c;
			auiIndices[ index++ ] = (r + 1) * cols + (c + 1);
			auiIndices[ index++ ] = r * cols + (c + 1);
		}
	}

	m_indexCount = index;

	// create and bind buffers to a vertex array object
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);
	
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	glBufferData(GL_ARRAY_BUFFER, ( rows * cols ) * sizeof(Vertex_b), aoVertices, GL_STATIC_DRAW );
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_b), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_b), (void*)(sizeof(vec4)));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}

void Tutorial12::setupShader1()
{
	// create shaders
	/*const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Colour; \
						   out vec4 vColour; \
						   uniform mat4 ProjectionView; \
						   void main() { vColour = Colour; gl_Position = ProjectionView * Position; }"; */

	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec3 Position; \
						   out vec4 vColour; \
						   uniform mat4 ProjectionView; \
						   void main() { \
						   vColour.r = 1; vColour.g = 1; vColour.b = 1; \
						   vec4 Position4 = vec4(Position, 1); \
						   gl_Position = ProjectionView * Position4; }";

	const char* fsSource = "#version 410\n \
						   in vec4 vColour; \
						   out vec4 FragColor; \
						   void main() { FragColor = vColour; }";

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
		glGetShaderiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void Tutorial12::setupShader2()
{
	// create shaders
	/*const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Colour; \
						   out vec4 vColour; \
						   uniform mat4 ProjectionView; \
						   void main() { vColour = Colour; gl_Position = ProjectionView * Position; }"; */

	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec4 Colour; \
						   out vec4 vColour; \
						   uniform mat4 ProjectionView; \
						   uniform float time; \
						   uniform float heightScale; \
						   uniform float periodScale; \
						   void main() { vColour = Colour; \
						   vec4 P = Position; \
						   P.y += sin( periodScale * (time + Position.x) ) * heightScale + sin( periodScale * (time + Position.z) ) * heightScale + heightScale/2; \
						   vColour.r = P.y / (1*heightScale); vColour.g = P.y / (4*heightScale); vColour.b = P.y / (8*heightScale);\
						   gl_Position = ProjectionView * P; }";

	const char* fsSource = "#version 410\n \
						   in vec4 vColour; \
						   out vec4 FragColor; \
						   void main() { FragColor = vColour; }";

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
		glGetShaderiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n",infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void Tutorial12::setupShader3()
{
	m_programID = m_shaderManager->LoadShader("Default", "../data/shaders/tut2.vert", "../data/shaders/tut2.frag");
}

void Tutorial12::setupShader4()
{
	m_programID = m_shaderManager->LoadShader("Procedure", "../data/shaders/proc.vert", "../data/shaders/proc.frag");
}

void Tutorial12::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16/9.0f, 0.1f, 3000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(100);
	myCam.setRotationSpeed(0.1f);

	setupShader4();

//	loadFromFile();

	dimensions = 257;
	//generatePerlin(dimensions);
	generateDiamondSquare(dimensions);
	generateGrid(dimensions, dimensions);

}

void Tutorial12::Destroy() {}

void Tutorial12::Update() 
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);
}
void Tutorial12::Draw() 
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID,"ProjectionView"); // find uniform values in shaders
	glUniformMatrix4fv(projectionViewUniform, 1, false, &myCam.getProjectionView()[0][0]); // set uniform values in shaders

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	unsigned int texloc = glGetUniformLocation(m_programID, "perlin_texture");
	glUniform1i(texloc, 0);
	
	glBindVertexArray(m_VAO);

	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}