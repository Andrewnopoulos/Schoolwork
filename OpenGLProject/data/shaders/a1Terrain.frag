#version 410

in vec2 frag_texcoord;
in vec4 vPos;

out vec4 FragColor;

uniform sampler2D heightMap;
uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D snowTexture;

uniform float maxHeight;

void main() {
	//FragColor = texture(heightMap, frag_texcoord).rrrr;
	vec4 white = vec4(1);
	vec4 black = vec4(0,0,0,1);
	
	vec4 sand = texture(sandTexture, frag_texcoord * 16);
	vec4 grass = texture(grassTexture, frag_texcoord * 16);
	vec4 snow = texture(snowTexture, frag_texcoord * 16);
	
	float fscale = vPos.y / (maxHeight*8);
	
	const float range1 = 0.4f;
	const float range2 = 0.5f;
	const float range3 = 0.65f;
	const float range4 = 0.85f;
	
	vec4 outColour = black;
	
	if (fscale >= 0.0 && fscale <= range1)
	{
		outColour = grass;
	}else if (fscale <= range2)
	{
		fscale -= range1;
		fscale /= (range2 - range1);
		float fscale2 = fscale;
		fscale = 1.0 - fscale;
		
		outColour += grass * fscale;
		outColour += sand * fscale2;
	}else if (fscale <= range3)
	{
		outColour = sand;
	}else if (fscale <= range4)
	{
		fscale -= range3;
		fscale /= (range4 - range3);
		float fscale2 = fscale;
		fscale = 1.0 - fscale;
		
		outColour += sand * fscale;
		outColour += snow * fscale2;
	}else
	{
		outColour = snow;
	}
	
	// vec4 outColour = mix(black, white, vPos.y/60 + 0.7);
	
	FragColor = outColour;
	FragColor.a = 1; 
}