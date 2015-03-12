#pragma once

#include "Camera.h"
#include <GLFW\glfw3.h>

class FlyCamera : public Camera
{
protected:
	float speed;
	float rotationVelocity;
	vec3 up;

	GLFWwindow* myWindow;
	bool viewButtonClicked;
	double cursorX, cursorY;

	void HandleKeyboardInput(float deltaTime);
	void HandleMouseInput(float deltaTime);

	void CalculateRotation(double dt, double xOffset, double yOffset);

public:
	void update(float deltaTime);
	void setSpeed(float speed);
	void setRotationSpeed(float rot);

	void SetInputWindow(GLFWwindow* a_Window) { myWindow = a_Window; }
};