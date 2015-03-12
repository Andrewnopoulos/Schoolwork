#include "Tutorial8.h"

void Tutorial8::Startup()
{
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(20);

	m_emitter = new ParticleEmitter();
	m_emitter->initialise(1000, 500,
		0.1f, 1.0f,
		1, 5,
		1, 0.1f,
		vec4(1, 0, 0, 1), vec4(1, 1, 0, 1));

	setupShader();
}

void Tutorial8::Update()
{
	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	myCam.update(deltaTime);

	m_emitter->Update(deltaTime, myCam.getWorldTransform());
}

void Tutorial8::Draw()
{
	glUseProgram(m_programID);

	int view_proj_uniform = glGetUniformLocation(m_programID, "projectionView");
	glUniformMatrix4fv(view_proj_uniform, 1, GL_FALSE, glm::value_ptr(myCam.getProjectionView()));

	m_emitter->Draw();
}

void Tutorial8::Destroy()
{
	delete m_emitter;
}

void Tutorial8::setupShader()
{
	const char* vsSource = "#version 410\n \
						   in vec4 Position; \
						   in vec4 Colour; \
						   out vec4 colour; \
						   uniform mat4 projectionView; \
						   void main() { \
						   colour = Colour; \
						   gl_Position = projectionView * Position;}";
	const char* fsSource = "#version 410\n \
						   in vec4 colour; \
						   void main() { \
						   gl_FragColor = colour;}";
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(vs, 1, &vsSource, nullptr);
	glShaderSource(fs, 1, &fsSource, nullptr);
	glCompileShader(fs);
	glCompileShader(vs);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vs);
	glAttachShader(m_programID, fs);
	glLinkProgram(m_programID);
	glDeleteShader(fs);
	glDeleteShader(vs);

}