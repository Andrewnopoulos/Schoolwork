#include "Tutorial6.h"
#include <glm/gtx/transform.hpp>

void Tutorial6::Startup()
{
	Gizmos::create();
	myCam.SetInputWindow(window);

	myCam.setPerspective(glm::pi<float>() * 0.25f, 16/9.0f, 0.1f, 1000.0f);
	myCam.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));

	myCam.setSpeed(5);

	currentTime = 0;
	previousTime = 0;

	//m_positions[0] = vec3(10, 5, 10);
	//m_positions[1] = vec3(-10, 0, -10);
	//m_rotations[0] = quat(vec3(0, -1, 0));
	//m_rotations[1] = quat(vec3(0, 1, 0));

	m_hipFrames[0].position = glm::vec3(0, 5, 0);
	m_hipFrames[0].rotation = glm::quat(glm::vec3(1, 0, 0));
	m_hipFrames[1].position = glm::vec3(0, 5, 0);
	m_hipFrames[1].rotation = glm::quat(glm::vec3(-1, 0, 0));

	m_kneeFrames[0].position = glm::vec3(0, -2.5f, 0);
	m_kneeFrames[0].rotation = glm::quat(glm::vec3(1, 0, 0));
	m_kneeFrames[1].position = glm::vec3(0, -2.5f, 0);
	m_kneeFrames[1].rotation = glm::quat(glm::vec3(0, 0, 0));

	m_ankleFrames[0].position = glm::vec3(0, -2.5f, 0);
	m_ankleFrames[0].rotation = glm::quat(glm::vec3(-1, 0, 0));
	m_ankleFrames[1].position = glm::vec3(0, -2.5f, 0);
	m_ankleFrames[1].rotation = glm::quat(glm::vec3(0, 0, 0));


}

void Tutorial6::Update()
{
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

	//// use time to animate between [0, 1]
	//float s = glm::cos(currentTime) * 0.5f + 0.5f;

	//// standard linear interpolation
	//vec3 p = (1.0f - s) * m_positions[0] + s * m_positions[1];

	////quaternion slerp
	//quat r = glm::slerp(m_rotations[0], m_rotations[1], s);

	////build a matrix
	//mat4 m = glm::translate(p) * glm::toMat4(r);

	//// draw a transform and box
	//Gizmos::addTransform(m);
	//Gizmos::addAABBFilled(p, vec3(0.5f), vec4(1, 0, 0, 1), &m);

	// animate leg
	float s = glm::cos(currentTime) * 0.5f + 0.5f;

	// linearly interpolate hip position
	glm::vec3 p = (1.0f - s) * m_hipFrames[0].position +
		s * m_hipFrames[1].position;
	// spherically interpolate hip rotation
	glm::quat r = glm::slerp(m_hipFrames[0].rotation,
		m_hipFrames[1].rotation, s);
	// update the hip bone
	m_hipBone = glm::translate(p) * glm::toMat4(r);

	p = (1.0f - s) * m_kneeFrames[0].position + s * m_kneeFrames[1].position;
	r = glm::slerp(m_kneeFrames[0].rotation, m_kneeFrames[1].rotation, s);
	m_kneeBone = glm::translate(p) * glm::toMat4(r);

	p = (1.0f - s) * m_ankleFrames[0].position + s * m_ankleFrames[1].position;
	r = glm::slerp(m_ankleFrames[0].rotation, m_ankleFrames[1].rotation, s);
	m_ankleBone = glm::translate(p) * glm::toMat4(r);

	m_kneeBone = m_hipBone * m_kneeBone;

	m_ankleBone = m_kneeBone * m_ankleBone;

	vec3 hipPos = glm::vec3(m_hipBone[3].x,
		m_hipBone[3].y,
		m_hipBone[3].z);
	vec3 kneePos = glm::vec3(m_kneeBone[3].x,
		m_kneeBone[3].y,
		m_kneeBone[3].z);
	vec3 anklePos = glm::vec3(m_ankleBone[3].x,
		m_ankleBone[3].y,
		m_ankleBone[3].z);

	vec3 half(0.5f);
	vec4 pink(1, 0, 1, 1);

	Gizmos::addAABBFilled(hipPos, half, pink, &m_hipBone);
	Gizmos::addAABBFilled(kneePos, half, pink, &m_kneeBone);
	Gizmos::addAABBFilled(anklePos, half, pink, &m_ankleBone);
}

void Tutorial6::Draw()
{
	Gizmos::draw(myCam.getProjectionView());
}

void Tutorial6::Destroy()
{
	Gizmos::destroy();
}