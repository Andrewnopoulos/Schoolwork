#version 410

in vec2 vTexCoord;

out vec4 FragColour;

uniform sampler2D waterTexture;

void main()
{
	FragColour = texture(waterTexture, vTexCoord * 50);
	FragColour = vec4(FragColour.xyz, 0.65);
}