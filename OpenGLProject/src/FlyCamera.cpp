#include "FlyCamera.h"
#include "glm\glm.hpp"

void FlyCamera::HandleKeyboardInput(float deltaTime)
{
	vec3 Right = worldTransform[0].xyz;
	vec3 Up = worldTransform[1].xyz;
	vec3 Forward = worldTransform[2].xyz;

	vec3 moveDir(0.0f);

	if (glfwGetKey(myWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir -= Forward;
	}

	if (glfwGetKey(myWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir += Forward;
	}

	if (glfwGetKey(myWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir -= Right;
	}

	if (glfwGetKey(myWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir += Right;
	} 

	if (glfwGetKey(myWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		moveDir += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (glfwGetKey(myWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveDir -= glm::vec3(0.0f, 1.0f, 0.0f);
	}

	float fLength = glm::length(moveDir);
	if (fLength > 0.01f)
	{
		moveDir = ((float)deltaTime * speed) * glm::normalize(moveDir);
		setPosition(getWorldTransform()[3].xyz + moveDir);
	}

}

void FlyCamera::HandleMouseInput(float deltaTime)
{
	if (glfwGetMouseButton(myWindow, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
	{
		if (viewButtonClicked == false)
		{
			int width, height;
			glfwGetFramebufferSize(myWindow, &width, &height);

			cursorX = width / 2.0;
			cursorY = height / 2.0;

			glfwSetCursorPos(myWindow, width / 2, height / 2);

			viewButtonClicked = true;
		}
		else
		{
			double mouseX, mouseY;
			glfwGetCursorPos(myWindow, &mouseX, &mouseY);

			double xOffset = mouseX - cursorX;
			double yOffset = mouseY - cursorY;

			CalculateRotation(deltaTime, xOffset, yOffset);

		}

		int width, height;
		glfwGetFramebufferSize(myWindow, &width, &height);
		glfwSetCursorPos(myWindow, width / 2, height / 2);
	}
	else
	{
		viewButtonClicked = false;
	}
}

void FlyCamera::CalculateRotation(double dt, double xOffset, double yOffset)
{
	if (xOffset != 0.0)
	{
		glm::mat4 rot = glm::rotate((float)(rotationVelocity * dt * -xOffset), glm::vec3(0, 1, 0));

		
		SetTransform(getWorldTransform() * rot);
	}

	if (yOffset != 0.0)
	{
		glm::mat4 rot = glm::rotate((float)(rotationVelocity * dt * -yOffset), glm::vec3(1, 0, 0));

		SetTransform(getWorldTransform() * rot);
	}

	//Clean up rotation
	glm::mat4 oldTrans = getWorldTransform();

	glm::mat4 trans;

	glm::vec3 worldUp = glm::vec3(0, 1, 0);

	//Right
	trans[0] = glm::normalize(glm::vec4(glm::cross(worldUp, oldTrans[2].xyz()), 0));
	//Up
	trans[1] = glm::normalize(glm::vec4(glm::cross(oldTrans[2].xyz(), trans[0].xyz()), 0));
	//Forward
	trans[2] = glm::normalize(oldTrans[2]);

	//Position
	trans[3] = oldTrans[3];

	SetTransform(trans);
}

void FlyCamera::update(float deltaTime)
{
	HandleKeyboardInput(deltaTime);
	HandleMouseInput(deltaTime);
}

void FlyCamera::setSpeed(float a_speed)
{
	speed = a_speed;
}

void FlyCamera::setRotationSpeed(float a_rot)
{
	rotationVelocity = a_rot;
}