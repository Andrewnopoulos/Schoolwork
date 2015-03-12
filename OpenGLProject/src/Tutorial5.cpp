#include "Tutorial5.h"
#include <stb_image.h>

void Tutorial5::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16/9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(5);

	currentTime = 0;
	previousTime = 0;

	// load diffuse map
	int imageWidth = 0, imageHeight = 0, imageFormat = 0;
	unsigned char* data = stbi_load("../data/textures/rock_diffuse.tga",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	// load normal map
	data = stbi_load("../data/textures/rock_normal.tga",
		&imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_normalmap);
	glBindTexture(GL_TEXTURE_2D, m_normalmap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight,
		0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	setupShader();

	setupVerts();

}

void Tutorial5::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);


}

void Tutorial5::Draw()
{
	// use our texture program
	glUseProgram(m_program);

	// bind the camera
	int loc = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		&(myCam.getProjectionView()[0][0]));

	// set texture slot
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalmap);

	// tell the shader where it is
	loc = glGetUniformLocation(m_program, "diffuse");
	glUniform1i(loc, 0);
	loc = glGetUniformLocation(m_program, "normal");
	glUniform1i(loc, 1);

	// bind the light
	vec3 light(sin(glfwGetTime()), 1, cos(glfwGetTime()));
	loc = glGetUniformLocation(m_program, "LightDir");
	glUniform3f(loc, light.x, light.y, light.z);

	// draw
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void Tutorial5::Destroy()
{

}

void Tutorial5::setupShader()
{
	const char* vsSource = "#version 410\n \
						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec2 TexCoord; \
						   layout(location=2) in vec4 Normal; \
						   layout(location=3) in vec4 Tangent; \
						   out vec2 vTexCoord; \
						   out vec3 vNormal; \
						   out vec3 vTangent; \
						   out vec3 vBiTangent; \
						   uniform mat4 ProjectionView; \
						   void main() { \
						   vTexCoord = TexCoord; \
						   vNormal = Normal.xyz; \
						   vTangent = Tangent.xyz; \
						   vBiTangent = cross(vNormal, vTangent); \
						   gl_Position= ProjectionView * Position;\
						   }";
	const char* fsSource = "#version 410\n \
						   in vec2 vTexCoord; \
						   in vec3 vNormal; \
						   in vec3 vTangent; \
						   in vec3 vBiTangent; \
						   out vec4 FragColor; \
						   uniform vec3 LightDir; \
						   uniform sampler2D diffuse; \
						   uniform sampler2D normal; \
						   void main() { \
						   mat3 TBN = mat3( \
						   normalize( vTangent ), \
						   normalize( vBiTangent ), \
						   normalize( vNormal ) ); \
						   vec3 N = texture(normal, \
						   vTexCoord).xyz * 2 - 1; \
						   float d = max( 0, dot( \
						   normalize( TBN * N ), \
						   normalize( LightDir ))); \
						   FragColor = texture(diffuse,vTexCoord);\
						   FragColor.rgb = FragColor.rgb * d; \
						   }";
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Tutorial5::setupVerts()
{
	Vertex vertexData[] = {
		{ -5, 0,  5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1 },
		{  5, 0,  5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1 },
		{  5, 0, -5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0 },
		{ -5, 0, -5, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0 },
	};

	unsigned int indexData[] = {
		0, 1, 2,
		0, 2, 3,
	};

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4,
		vertexData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6,
		indexData, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), ((char*)0)+48);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), ((char*)0)+16);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE,
		sizeof(Vertex), ((char*)0)+32);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}