#version 410

in vec4 Colour;
in vec2 vTexCoord;
out vec4 fragColour;

uniform sampler2D snowTexture;

void main() 
{
	fragColour = texture(snowTexture, vTexCoord);
	//fragColour = Colour;
}