#include "Camera.h"

Camera::Camera()
{
	projectionSet = false;
}

void Camera::setPerspective(float fieldOfView, float aspectRatio, float near, float far)
{
	projectionTransform = glm::perspective(fieldOfView, aspectRatio, near, far);
	projectionSet = true;
	updateProjectionViewTransform();
}

void Camera::setLookAt(vec3& from, vec3& to, vec3& up)
{
	viewTransform = glm::lookAt(from, to, up);
	worldTransform = glm::inverse(viewTransform);
	updateProjectionViewTransform();
}

void Camera::setPosition(vec3 position)
{
	worldTransform[3][0] = position[0];
	worldTransform[3][1] = position[1];
	worldTransform[3][2] = position[2];
	updateProjectionViewTransform();
}

void Camera::SetTransform(mat4 transform)
{
	worldTransform = transform;
	updateProjectionViewTransform();
}

mat4& Camera::getProjection()
{
	return projectionTransform;
}

mat4& Camera::getView()
{
	return viewTransform;
}

mat4& Camera::getWorldTransform()
{
	return worldTransform;
}

mat4& Camera::getProjectionView()
{
	return projectionViewTransform;
}

void Camera::updateProjectionViewTransform()
{
	if (projectionSet)
	{
		viewTransform = glm::inverse(worldTransform);
		projectionViewTransform = projectionTransform * viewTransform;
	}
}