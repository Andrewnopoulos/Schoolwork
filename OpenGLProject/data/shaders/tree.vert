#version 410
layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoord;
layout(location=3) in vec4 Tangent;

out vec3 vNormal;
out vec3 vPosition;
out vec3 vBiTangent;
out vec3 vTangent;
out vec2 vTexCoord;
out vec4 vShadowCoord;

uniform mat4 ProjectionView;
uniform mat4 Local;
uniform mat4 LightMatrix;

void main() 
{ 
	vNormal = Normal.xyz;
	vTexCoord = TexCoord;
	vTangent = Tangent.xyz;
	vBiTangent = cross(vNormal, vTangent);
	vPosition = Position.xyz;
	
	gl_Position = ProjectionView * Local * Position; 
	
	vShadowCoord = LightMatrix * Local * Position;
}