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

uniform float waterLevel;

uniform float roughness;
uniform float fresnel;

uniform vec3 CameraPos;
uniform vec3 LightDir;

vec4 realistic(vec4 diffuseColour)
{
	vec3 position = vPos.xyz;
	vec3 L = LightDir;
	vec3 E = position - CameraPos;
	vec3 N = vNormal.xyz;
	
	float R2 = roughness * roughness;
	
	float NdL = max( 0.0f, dot( N, L ) );
	float NdE = max( 0.0f, dot( N, E ) );
	
	// Oren-Nayar Diffuse Term
	float A = 1.0f - 0.5f * R2 / (R2 + 0.33f);
	float B = 0.45f * R2 / (R2 + 0.09f);
	
	// CX = Max(0, cos(l,e))
	vec3 lightProjected = normalize( L - N * NdL );
	vec3 viewProjected = normalize( E - N * NdE);
	float CX = max( 0.0f, dot( lightProjected, viewProjected ) );
	
	// DX = sin(alpha) * tan(beta)
	float alpha = sin( max( acos( NdE ), acos( NdL ) ) );
	float beta = tan( min( acos( NdE ), acos( NdL ) ) );
	float DX = alpha * beta;
	
	float OrenNayar = NdL * (A + B * CX * DX);
	
	vec3 H = normalize( L + E ); // light and view half vector
	float NdH = max( dot( N, H ), 0.0f );
	float NdH2 = NdH * NdH;
	float HdE = dot( H, E );
	float e = 2.71828182845904523536028747135f;
	float pi = 3.1415926535897932384626433832f;

	float exponent = -(1 - NdH2) / (NdH2 * R2);
	float D = pow( e, exponent ) / (R2 * NdH2 * NdH2);
	
	// Fresnel Term F
	float F = mix( pow( 1 - HdE, 5 ), 1, fresnel );
	
	// Geometric Attenuation Factor G
	float X = 2.0f * NdH / dot( E, H );
	float G = min(1, min(X * NdL, X * NdE));
	
	// Calculate Cook-Torrance
	float CookTorrance = max( (D*G*F) / (NdE * pi), 0.0f );
	
//	float shad = max(0, dot(normalize(vNormal.xyz), LightDir));
//	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias) {
//		shad = 0;
//	}
	
	vec4 outColour = diffuseColour;
	
	float Brightness = OrenNayar + CookTorrance;
	
	Brightness = max(0.2, Brightness);
	
	outColour.rgb = outColour.rgb * Brightness;
//	outColour = outColour * shad;
	return outColour;
	
}

void main() {
	//FragColor = texture(heightMap, frag_texcoord).rrrr;
	vec4 white = vec4(1);
	vec4 black = vec4(0,0,0,1);
	
	vec4 dirt = texture(dirtTexture, frag_texcoord * 16);
	vec4 grass = texture(grassTexture, frag_texcoord * 16);
	vec4 snow = texture(snowTexture, frag_texcoord * 16);
	vec4 sand = texture(sandTexture, frag_texcoord * 16);
	
	float fscale = vPos.y / (maxHeight);
	
	float wscale = waterLevel / maxHeight;
	
	fscale = fscale - wscale;
	
	float range0 = 0;
	float range1 = 0.1;
	float range2 = 0.4;
	float range3 = 0.5;
	float range4 = 0.55;
	float range5 = 0.65;
	
	const float cliffBound = 0.8;
	
	float dotNorm = dot(normalize(vNormal.xyz), vec3(0, 1, 0));
	
	vec4 outColour = black;
	if (fscale >= 0.0 && fscale <= range0)
	{
		outColour = sand;
		if (dotNorm < cliffBound)
		{
			outColour = mix(dirt, outColour, dotNorm);
		}
	} else if (fscale <= range1)
	{
		fscale -= range0;
		fscale /= (range1 - range0);
		float fscale2 = fscale;
		fscale = 1.0 - fscale;
		
		outColour += sand * fscale;
		outColour += grass * fscale2;
		if (dotNorm < cliffBound)
		{
			outColour = mix(dirt, outColour, dotNorm);
		}
	}
	else if (fscale <= range2)
	{
		outColour = grass;
		if (dotNorm < cliffBound)
		{
			outColour = mix(dirt, outColour, dotNorm);
		}
	}else if (fscale <= range3)
	{
		fscale -= range2;
		fscale /= (range3 - range2);
		float fscale2 = fscale;
		fscale = 1.0 - fscale;
		
		outColour += grass * fscale;
		outColour += dirt * fscale2;
		if (dotNorm < cliffBound)
		{
			outColour = mix(dirt, outColour, dotNorm);
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
	
//	float d = max(0, dot(normalize(vNormal.xyz), vec3(0, 1, 0)));
	// vec4 outColour = mix(black, white, vPos.y/60 + 0.7);
	
	FragColor = realistic(outColour);
	//FragColor.rgb = FragColor.rgb * d;
	FragColor.a = 1; 
}