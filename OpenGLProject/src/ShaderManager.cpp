#include "ShaderManager.h"

#include "gl_core_4_4.h"
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <string>

ShaderManager::ShaderManager() : m_ErrorShader(0)
{
	CreateErrorShader();
}

ShaderManager::~ShaderManager()
{
	for (auto it = m_shaderMap.begin(); it != m_shaderMap.end(); ++it)
	{
		glDeleteShader(it->second);
	}

	glDeleteShader(m_ErrorShader);
}

bool ShaderManager::DoesShaderExist(std::string& a_Name)
{
	if (m_shaderMap.find(a_Name) != m_shaderMap.end())
	{
		return true;
	}

	return false;
}

unsigned int ShaderManager::GetErrorShader()
{
	return m_ErrorShader;
}

void ShaderManager::CreateErrorShader()
{
	// create shaders
	char* vsSource = "#version 330\n\
					 layout(location=0)in vec4 Position; \
					 uniform mat4 Projection; \
					 uniform mat4 View; \
					 uniform mat4 Model; \
					 void main() { gl_Position = Projection * View * Model * Position; }";

	char* fsSource = "#version 330\n \
					  out vec4 outColour; \
					  void main()	{ outColour = vec4(1, 0, 1, 1); }";

	GLuint vsHandle = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsHandle = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vsHandle, 1, (const char**)&vsSource, 0);
	glCompileShader(vsHandle);

	glShaderSource(fsHandle, 1, (const char**)&fsSource, 0);
	glCompileShader(fsHandle);

	m_ErrorShader = glCreateProgram();
	glAttachShader(m_ErrorShader, vsHandle);
	glAttachShader(m_ErrorShader, fsHandle);
	glBindAttribLocation(m_ErrorShader, 0, "Position");
	glBindFragDataLocation(m_ErrorShader, 0, "outColour");
	glLinkProgram(m_ErrorShader);

	m_shaderMap["errorShader"] = m_ErrorShader;
}

unsigned int ShaderManager::LoadShader(std::string a_ShaderName,
	const char* a_VertexShader,
	const char* a_PixelShader,
	const char* a_GeometryShader,
	const char* a_TesselationControlShader,
	const char* a_TesselationEvalShader)
{
	if (DoesShaderExist(a_ShaderName))
	{
		return m_shaderMap[a_ShaderName];
	}

	GLuint vs = LoadShader(a_VertexShader, GL_VERTEX_SHADER);
	GLuint fs = LoadShader(a_PixelShader, GL_FRAGMENT_SHADER);

	GLuint gs = 0;
	GLuint tcs = 0;
	GLuint tes = 0;

	if (a_GeometryShader != nullptr)
	{
		gs = LoadShader(a_GeometryShader, GL_GEOMETRY_SHADER);
	}

	if (a_TesselationControlShader && a_TesselationEvalShader)
	{
		tcs = LoadShader(a_TesselationControlShader, GL_TESS_CONTROL_SHADER);
		tes = LoadShader(a_TesselationEvalShader, GL_TESS_EVALUATION_SHADER);
	}

	unsigned int ShaderID = CreateProgram(vs, tcs, tes, gs, fs);
	m_shaderMap[a_ShaderName] = ShaderID;

	return ShaderID;
}

unsigned int ShaderManager::LoadShader(const char* a_Name, unsigned int a_Type)
{
	int success = GL_FALSE;

	unsigned int handle = glCreateShader(a_Type);
	unsigned char* source = FileToBuffer(a_Name);

	glShaderSource(handle, 1, (const char**)&source, 0);
	glCompileShader(handle);

	glGetShaderiv(handle, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to compile shader!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}

	delete[] source;

	return handle;
}

void ShaderManager::SetProjectionMatrix(glm::mat4 a_ProjMat)
{
	for (auto it = m_shaderMap.begin(); it != m_shaderMap.end(); ++it)
	{
		glUseProgram(it->second);
		GLint loc = glGetUniformLocation(it->second, "Projection");
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(a_ProjMat));
	}
}

void ShaderManager::SetViewMatrix(glm::mat4 a_ViewMat)
{
	for (auto it = m_shaderMap.begin(); it != m_shaderMap.end(); ++it)
	{
		glUseProgram(it->second);
		GLint loc = glGetUniformLocation(it->second, "View");
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(a_ViewMat));
	}
}

void ShaderManager::SetEyePosition(glm::vec4 a_EyePos)
{
	//unsigned int uiError;
	for (auto it = m_shaderMap.begin(); it != m_shaderMap.end(); ++it)
	{
		glUseProgram(it->second);
		GLint loc = glGetUniformLocation(it->second, "EyePos");
		glUniform4fv(loc, 1, glm::value_ptr(a_EyePos));

		//unsigned int uiError = glGetError();
	}
}

unsigned char* ShaderManager::FileToBuffer(const char* a_Name)
{
	// open file for text reading
	FILE* pFile = fopen(a_Name, "rb");
	if (pFile == nullptr)
	{
		printf("Error: unable to open file '%s' for reading!\n", a_Name);
		return nullptr;
	}

	// get number of bytes in file
	fseek(pFile, 0, SEEK_END);
	unsigned int fileLength = (unsigned int)ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// allocate buffer and read file contents
	unsigned char* buffer = new unsigned char[fileLength + 1];
	memset(buffer, 0, fileLength + 1);
	fread(buffer, sizeof(unsigned char), fileLength, pFile);

	fclose(pFile);
	return buffer;
}

unsigned int ShaderManager::CreateProgram(GLuint vs, GLuint tcs, GLuint tes, GLuint gs, GLuint fs)
{
	int success = GL_FALSE;

	// create a shader program and attach the shaders to it
	unsigned int handle = glCreateProgram();
	glAttachShader(handle, vs);
	glAttachShader(handle, tcs);
	glAttachShader(handle, tes);
	glAttachShader(handle, gs);
	glAttachShader(handle, fs);

	// link the program together, log errors

	glLinkProgram(handle);

	glGetProgramiv(handle, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetShaderInfoLog(handle, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s", infoLog);
		printf("\n");
		delete[] infoLog;
		return 0;
	}

	return handle;
}