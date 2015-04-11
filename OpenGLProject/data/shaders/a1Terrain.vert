#version 410

layout(location=0) in vec4 Position;
layout(location=1) in vec2 texcoord;

out vec4 vPos;
out vec2 frag_texcoord;

uniform sampler2D perlin_texture;
uniform mat4 ProjectionView;

void main() {

	vec4 pos = Position;
	pos.y += texture(perlin_texture, texcoord).r * 10;

	frag_texcoord = texcoord;
	gl_Position = ProjectionView * pos; 
	vPos = pos;
}