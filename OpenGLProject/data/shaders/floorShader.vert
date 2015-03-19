#version 410
layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Normal; 

out vec4 vNormal; 

uniform mat4 ProjectionView; 

void main() {
	vNormal = Normal; 
	gl_Position = ProjectionView * Position;
}