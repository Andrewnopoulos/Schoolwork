#version 410

in vec2 frag_texcoord;
in vec4 vPos;

out vec4 FragColor;

uniform sampler2D perlin_texture;

void main() {
	//FragColor = texture(perlin_texture, frag_texcoord).rrrr;
	vec4 white = vec4(1);
	vec4 black = vec4(0,0,0,1);
	
	vec4 outColour = mix(black, white, vPos.y/60 + 0.7);
	FragColor = outColour;
	FragColor.a = 1; 
}