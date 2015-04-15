#version 410

layout(location=0) in vec4 Position; 
layout(location=1) in vec4 Colour; 
out vec4 vColour; 
uniform mat4 ProjectionView; 
uniform float time; 
uniform float heightScale; 
uniform float periodScale; 
void main() 
{ 
	vColour = Colour; 
	vec4 P = Position; 
	P.y += sin( periodScale * (time + Position.x) ) * heightScale + sin( periodScale * (time + Position.z) ) * heightScale + heightScale/2; 
	vColour.r = P.y / (1*heightScale); vColour.g = P.y / (4*heightScale); vColour.b = P.y / (8*heightScale);
	gl_Position = ProjectionView * P; 
}