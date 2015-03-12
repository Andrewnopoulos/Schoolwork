#include "Tutorial3.h"

void Tutorial3::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16/9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(20);

	setupShader1();

	std::string err = tinyobj::LoadObj(shapes, materials, "../data/bunny.obj");
	createOpenGLBuffers(shapes);
}

void Tutorial3::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);
}

void Tutorial3::Draw()
{
	glUseProgram(m_programID);
	
	int view_proj_uniform = glGetUniformLocation(m_programID, "projection_view");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(myCam.getProjectionView()));

	int light_dir_uniform = glGetUniformLocation(m_programID, "LightDir");
	glUniform3f(light_dir_uniform, 0, 1, 0);
	
	int light_colour_uniform = glGetUniformLocation(m_programID, "LightColour");
	glUniform3f(light_colour_uniform, 1, 1, 0);

	mat4 camera_matrix = myCam.getWorldTransform();
	int camera_pos_uniform = glGetUniformLocation(m_programID, "CameraPos");
	glUniform3f(camera_pos_uniform, camera_matrix[3][0], camera_matrix[3][1], camera_matrix[3][2]);

	int specular_uniform = glGetUniformLocation(m_programID, "SpecPow");
	glUniform1f(specular_uniform, 128);

	for (unsigned int i = 0; i < m_gl_info.size(); ++i)
	{
		glBindVertexArray(m_gl_info[i].m_VAO);
		glDrawElements(GL_TRIANGLES,m_gl_info[i].m_index_count,GL_UNSIGNED_INT, 0);
	}
}

void Tutorial3::Destroy()
{

}

void Tutorial3::createOpenGLBuffers(std::vector<tinyobj::shape_t>& shapes)
{
	m_gl_info.resize(shapes.size());
	for (unsigned int mesh_index = 0;
		mesh_index < shapes.size();
		++mesh_index)
	{
		glGenVertexArrays(1, &m_gl_info[mesh_index].m_VAO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_VBO);
		glGenBuffers(1, &m_gl_info[mesh_index].m_IBO);
		glBindVertexArray(m_gl_info[mesh_index].m_VAO);

		unsigned int float_count = shapes[mesh_index].mesh.positions.size();
		float_count += shapes[mesh_index].mesh.normals.size();
		float_count += shapes[mesh_index].mesh.texcoords.size();

		std::vector<float> vertex_data;
		vertex_data.reserve(float_count);

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.positions.begin(),
			shapes[mesh_index].mesh.positions.end());

		vertex_data.insert(vertex_data.end(),
			shapes[mesh_index].mesh.normals.begin(),
			shapes[mesh_index].mesh.normals.end());

		m_gl_info[mesh_index].m_index_count =
			shapes[mesh_index].mesh.indices.size();

		glBindBuffer(GL_ARRAY_BUFFER, m_gl_info[mesh_index].m_VBO);
		glBufferData(GL_ARRAY_BUFFER,
			vertex_data.size() * sizeof(float),
			vertex_data.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gl_info[mesh_index].m_IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			shapes[mesh_index].mesh.indices.size() * sizeof(unsigned int),
			shapes[mesh_index].mesh.indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normal data

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0,
			(void*)(sizeof(float)*shapes[mesh_index].mesh.positions.size()));

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

void Tutorial3::setupShader1()
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
						   layout(location=1) in vec3 Normal; \
						   out vec4 vNormal; \
						   out vec4 vPosition4; \
						   uniform mat4 projection_view; \
						   void main() { \
						   vPosition4 = vec4(Position, 1); \
						   vNormal = vec4(Normal, 1); \
						   gl_Position = projection_view * vPosition4; }";

	const char* fsSource = "#version 410\n \
						   in vec4 vNormal; \
						   in vec4 vPosition4; \
						   out vec4 FragColor; \
						   uniform vec3 LightDir; \
						   uniform vec3 LightColour; \
						   uniform vec3 CameraPos; \
						   uniform float SpecPow; \
						   void main() { \
						   float d = max(0, dot( normalize(vNormal.xyz), LightDir ) ); \
						   vec3 E = normalize( CameraPos - vPosition4.xyz ); \
						   vec3 R = reflect( -LightDir, vNormal.xyz ); \
						   float s = max( 0, dot( E, R ) ); \
						   s = pow( s, SpecPow ); \
						   FragColor = vec4(LightColour * d + LightColour * s, 1); }";

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