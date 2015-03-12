#include "Tutorial2.h"
#include "OBJLoader.h"
#include "tiny_obj_loader.h"
#include "ShaderManager.h"


void Tutorial2::generateGrid( unsigned int rows, unsigned int cols ) 
{
	Vertex_a* aoVertices = new Vertex_a[ rows * cols ];
	for( unsigned int r = 0; r < rows; ++r)
	{
		for( unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[ r * cols + c ].position = vec4((float)c, 0, (float)r, 1);

			// generate arbitrary colours for vertices
			vec3 colour = vec3( sinf( (c / (float)(cols-1)) * ( r / (float)(rows-1))) );
			aoVertices[ r * cols + c ].colour = vec4( colour, 1 );
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

	glBufferData(GL_ARRAY_BUFFER, ( rows * cols ) * sizeof(Vertex_a), aoVertices, GL_STATIC_DRAW );
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_a), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_a), (void*)(sizeof(vec4)));

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;
}

void Tutorial2::loadFromFile()
{
	OBJLoader loader;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<unsigned int> indices;
	bool res = loader.loadOBJ("../data/test.obj", vertices, uvs, normals, indices);

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	m_indexCount = indices.size();

	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void Tutorial2::setupShader1()
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
						   vColour.r = 0; vColour.g = 0; vColour.b = 0; \
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

void Tutorial2::setupShader2()
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

void Tutorial2::setupShader3()
{
	m_programID = m_shaderManager->LoadShader("Default", "../data/shaders/tut2.vert", "../data/shaders/tut2.frag");
}

void Tutorial2::Startup() 
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16/9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(20);
	myCam.setRotationSpeed(0.1f);

	setupShader3();

//	loadFromFile();

	generateGrid(100, 100);
}

void Tutorial2::Destroy() {}
void Tutorial2::Update() 
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);
}
void Tutorial2::Draw() 
{
	glUseProgram(m_programID);
	unsigned int projectionViewUniform = glGetUniformLocation(m_programID,"ProjectionView"); // find uniform values in shaders
	glUniformMatrix4fv(projectionViewUniform, 1, false, &myCam.getProjectionView()[0][0]); // set uniform values in shaders

	unsigned int timeUniform = glGetUniformLocation(m_programID, "time");
	glUniform1f(timeUniform, currentTime);

	unsigned int heightUniform = glGetUniformLocation(m_programID, "heightScale");
	glUniform1f(heightUniform, 5);

	unsigned int periodUniform = glGetUniformLocation(m_programID, "periodScale");
	glUniform1f(periodUniform, 0.5);
	
	glBindVertexArray(m_VAO);

	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // wireframe

	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}