#include <glm\glm.hpp>
#include <Windows.h>
#include <vector>
#include <iostream>
#include "Tutorial14.h"

using namespace std;

class BoundingSphere {
public:
	BoundingSphere() : centre(0), radius(1) {}
	~BoundingSphere() {}
	void fit(const BoundingSphere& sphere) {
		float d = glm::distance(centre, sphere.centre) +
			sphere.radius;
		if (d > radius)
			radius = d;
	}
	glm::vec4 centre;
	float radius;
};class Node {
public:
	Node() : parent(nullptr), localTransform(1) {}
	virtual ~Node() { for (auto child : children) delete child; }
	void addChild(Node* child) {
		children.push_back(child);
	}
	virtual void update() {
		updateWorldTransform();
		for (auto child : children)
			child->update();
		updateWorldBounds();
	}
	void updateWorldTransform() {
		if (parent == nullptr)
			worldTransform = localTransform;
		else
			worldTransform = parent->worldTransform *
			localTransform;
	}
	void updateWorldBounds() {
		worldBounds.centre = worldTransform * localBounds.centre;
		worldBounds.radius = localBounds.radius;
		for (auto child : children)
			worldBounds.fit(child->worldBounds);
	}
protected:
	Node* parent;
	BoundingSphere localBounds;
	BoundingSphere worldBounds;
	glm::mat4 localTransform;
	glm::mat4 worldTransform;
	std::vector<Node*> children;
};

void Tutorial14::addChildren(Node* root, int childrenPerNode, int depth) {
	if (depth > 0) {
		for (int i = 0; i < childrenPerNode; i++) {
			Node* child = new Node();
			addChildren(child, childrenPerNode, depth - 1);
			root->addChild(child);
		}
	}
}void Tutorial14::Run(){	Node* root = new Node();
	addChildren(root, 5, 6);

	// query the speed of the CPU clock cycles
	LARGE_INTEGER perfFreq;
	QueryPerformanceFrequency(&perfFreq);
	LARGE_INTEGER startTime, endTime;
	// get start time
	QueryPerformanceCounter(&startTime);
	// run code to be timed
	root->update();
	// get end time
	QueryPerformanceCounter(&endTime);
	// length of time taken in seconds
	double duration = (endTime.QuadPart - startTime.QuadPart) /
		(double)perfFreq.QuadPart;

	cout << duration << endl;

	delete root;}