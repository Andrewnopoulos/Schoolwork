#include "Tutorial16.h"
#include "Gizmos.h"
#include "fmod_errors.h"

void Tutorial16::Startup()
{
	Gizmos::create();

	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16 / 9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(5);
	myCam.setRotationSpeed(0.08f);

	currentTime = 0;
	previousTime = 0;

	InitSound();
}

bool Tutorial16::InitSound()
{
	// initialize fmod
	FMOD_RESULT result;
	m_soundSystem = NULL;

	// create main system object
	result = FMOD::System_Create(&m_soundSystem);

	if (result != FMOD_OK)
	{
		printf("FMOD ERROR (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	// initialize fmod with 512 channels
	result = m_soundSystem->init(512, FMOD_INIT_NORMAL, 0);

	if (result != FMOD_OK)
	{
		printf("FMOD ERROR (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	result = m_soundSystem->createSound("../data/Sounds/RowRow.mp3", FMOD_CREATESTREAM | FMOD_3D, 0, &m_sound);

	if (result != FMOD_OK)
	{
		printf("FMOD ERROR (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	result = m_soundSystem->createChannelGroup("MyChannelGroup", &m_channelGroup);
	if (result != FMOD_OK)
	{
		printf("FMOD ERROR (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	result = m_soundSystem->playSound(m_sound, m_channelGroup, false, &m_channel);
	if (result != FMOD_OK)
	{
		printf("FMOD ERROR (%d) %s\n", result, FMOD_ErrorString(result));
		return false;
	}

	FMOD_VECTOR soundposition = { 0, 0, 0 };
	FMOD_VECTOR soundvelocity = { 0, 0, 0 };

	result = m_channel->set3DAttributes(&soundposition, &soundvelocity, 0);

	return true;
}

void Tutorial16::Update()
{

	mat4 cameramatrix = myCam.getWorldTransform();
	FMOD_VECTOR position = { cameramatrix[3][0], cameramatrix[3][1], cameramatrix[3][2] };
	FMOD_VECTOR velocity = { 0, 0, 0 };
	FMOD_VECTOR forward = {cameramatrix[2][0], cameramatrix[2][1], cameramatrix[2][2]};
	FMOD_VECTOR up = { 0, 1, 0 };

	m_soundSystem->set3DListenerAttributes(0, &position, &velocity, &forward, &up);

	m_soundSystem->update();

	currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	Gizmos::clear();

	Gizmos::addTransform(glm::mat4(1));

	myCam.update(deltaTime);

	vec4 white(1);
	vec4 black(0, 0, 0, 1);

	for (int i = 0; i < 21; ++i)
	{
		Gizmos::addLine(
			vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);

		Gizmos::addLine(
			vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}

	Gizmos::addSphere(glm::vec3(0, 0, 0), 5.0f, 20, 20, black);
}

void Tutorial16::Draw()
{
	Gizmos::draw(myCam.getProjectionView());
}

void Tutorial16::Destroy()
{
	Gizmos::destroy();

	m_sound->release();
	m_soundSystem->close();
	m_soundSystem->release(); 
}