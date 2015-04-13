#version 410
layout(location=0) in vec4 Position;
layout(location=1) in vec4 Normal;
layout(location=2) in vec2 TexCoord;

out vec2 vTexCoord;

uniform mat4 ProjectionView;

uniform float time;
uniform float heightScale;
uniform float periodScale;

void main()
{
	vTexCoord = TexCoord;
	
	vec4 P = Position;
	
	P.y += sin( periodScale * (time + Position.x)) * heightScale + sin( periodScale * (time + Position.z)) * heightScale;
	
	gl_Position = ProjectionView * P;
}