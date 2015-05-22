#pragma once

#include "Application.h"
#include "FlyCamera.h"
#include <glm\glm.hpp>
#include <fmod.hpp>

using glm::vec3;
using glm::vec4;
using glm::mat4;

class Tutorial16 : public Application
{
public:
	virtual void Startup();
	virtual void Destroy();
	virtual void Update();
	virtual void Draw();

private:
	FlyCamera myCam;
	float previousTime;
	float currentTime;
	mat4 view;
	mat4 projection;
	mat4 inverse;

	FMOD::System* m_soundSystem;
	FMOD::Sound* m_sound;
	FMOD::Channel* m_channel;
	FMOD::ChannelGroup* m_channelGroup;

	bool InitSound();
	
};