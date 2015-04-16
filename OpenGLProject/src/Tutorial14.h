#pragma once

class BoundingSphere;
class Node;

class Tutorial14
{
private:
	void addChildren(Node* root, int childrenPerNode, int depth);
public:
	void Run();
};