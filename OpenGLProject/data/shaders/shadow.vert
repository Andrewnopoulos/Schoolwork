#version 410

layout(location = 0) in vec4 Position;
layout(location = 4) in vec4 weights;
layout(location = 5) in vec4 indices;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];
uniform mat4 LightMatrix;

void main() {
	ivec4 index = ivec4(indices);
	vec4 P = bones[ index.x ] * Position * weights.x;
	P += bones[index.y] * Position * weights.y;
	P += bones[index.z] * Position * weights.z;
	P += bones[index.w] * Position * weights.w;
	gl_Position = LightMatrix * P;
}