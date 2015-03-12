#pragma once

#include <glm\glm.hpp>
#include <glm\ext.hpp>

using glm::vec3;
using glm::mat4;

class Camera
{
protected:
	mat4 worldTransform;
	mat4 viewTransform;
	mat4 projectionTransform;
	mat4 projectionViewTransform;
	
	bool projectionSet;

	void updateProjectionViewTransform();

public:
	Camera();
	virtual void update(float deltaTime) = 0;
	void setPerspective(float fieldOfView, float aspectRatio, float near, float far);
	void setLookAt(vec3& from, vec3& to, vec3& up);
	void setPosition(vec3 position);
	void SetTransform(mat4 transform);
	mat4& getWorldTransform();
	mat4& getView();
	mat4& getProjection();
	mat4& getProjectionView();
	bool getProjectionSet();
};