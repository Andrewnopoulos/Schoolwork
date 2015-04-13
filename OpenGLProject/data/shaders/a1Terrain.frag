#version 410

in vec2 frag_texcoord;
in vec4 vPos;
in vec4 vNormal;

out vec4 FragColor;

uniform sampler2D heightMap;
uniform sampler2D sandTexture;
uniform sampler2D grassTexture;
uniform sampler2D snowTexture;
uniform sampler2D dirtTexture;

uniform float maxHeight;

void main() {
	//FragColor = texture(heightMap, frag_texcoord).rrrr;
	vec4 white = vec4(1);
	vec4 black = vec4(0,0,0,1);
	
	vec4 dirt = texture(dirtTexture, frag_texcoord * 16);
	vec4 grass = texture(grassTexture, frag_texcoord * 16);
	vec4 snow = texture(snowTexture, frag_texcoord * 16);
	vec4 sand = texture(sandTexture, frag_texcoord * 16);
	
	float fscale = vPos.y / (maxHeight);
	
	const float range0 = 0.12f;
	const float range1 = 0.16f;
	const float range2 = 0.4f;
	const float range3 = 0.5f;
	const float range4 = 0.65f;
	const float range5 = 0.75f;
	
	const float cliffBound = 0.8;
	
	vec4 outColour = black;
	if (fscale >= 0.0 && fscale <= range0)
	{
		outColour = sand;
		if (dot(normalize(vNormal.xyz), vec3(0, 1, 0)) < cliffBound)
		{
			outColour = dirt;
		}
	} else if (fscale <= range1)
	{
		fscale -= range0;
		fscale /= (range1 - range0);
		float fscale2 = fscale;
		fscale = 1.0 - fscale;
		
		outColour += sand * fscale;
		outColour += grass * fscale2;
		if (dot(normalize(vNormal.xyz), vec3(0, 1, 0)) < cliffBound)
		{
			outColour = dirt;
		}
	}
	else if (fscale <= range2)
	{
		outColour = grass;
		if (dot(normalize(vNormal.xyz), vec3(0, 1, 0)) < cliffBound)
		{
			outColour = dirt;
		}
	}else if (fscale <= range3)
	{
		fscale -= range2;
		fscale /= (range3 - range2);
		float fscale2 = fscale;
		fscale = 1.0 - fscale;
		
		outColour += grass * fscale;
		outColour += dirt * fscale2;
		if (dot(normalize(vNormal.xyz), vec3(0, 1, 0)) < cliffBound)
		{
			outColour = dirt;
		}
	}else if (fscale <= range4)
	{
		outColour = dirt;
	}else if (fscale <= range5)
	{
		fscale -= range4;
		fscale /= (range5 - range4);
		float fscale2 = fscale;
		fscale = 1.0 - fscale;
		
		outColour += dirt * fscale;
		outColour += snow * fscale2;
	}else
	{
		outColour = snow;
	}
	
	float d = max(0, dot(normalize(vNormal.xyz), vec3(0, 1, 0)));
	// vec4 outColour = mix(black, white, vPos.y/60 + 0.7);
	
	FragColor = outColour;
	FragColor.rgb = FragColor.rgb * d;
	FragColor.a = 1; 
}