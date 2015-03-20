#version 410
layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoord;
layout(location=3) in vec4 Tangent;
layout(location=4) in vec4 weights;
layout(location=5) in vec4 indices;

out vec3 vNormal;
out vec3 vPosition;
out vec3 vBiTangent;
out vec3 vTangent;
out vec2 vTexCoord;
out vec4 vShadowCoord;

uniform mat4 ProjectionView;
const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

uniform mat4 LightMatrix;

void main() 
{ 
	vNormal = Normal.xyz;
	vTexCoord = TexCoord;
	vTangent = Tangent.xyz;
	vBiTangent = cross(vNormal, vTangent);
	vPosition = Position.xyz;
	ivec4 index = ivec4(indices);
	vec4 P = bones[ index.x ] * Position * weights.x;
	P += bones[index.y] * Position * weights.y;
	P += bones[index.z] * Position * weights.z;
	P += bones[index.w] * Position * weights.w;
	gl_Position = ProjectionView * P; 
	
	vShadowCoord = LightMatrix * P;
}