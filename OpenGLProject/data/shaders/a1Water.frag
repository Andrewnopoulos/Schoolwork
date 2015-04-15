#version 410

in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vTangent;
in vec3 vBiTangent;

out vec4 FragColour;

uniform sampler2D waterTexture;
uniform vec3 LightDir;
uniform vec3 CameraPos;

uniform float SpecPow;

vec4 oldSchool(vec4 diffuse)
{
//	mat3 TBN = mat3(
//		normalize( vTangent ),
//		normalize( vBiTangent ),
//		normalize( vNormal ));
		
//	vec3 N = vNormal;
	
//	float d = max(0.2, dot( normalize(TBN * N), normalize(LightDir) ) );
	
	float brightness = max(0.2, dot(LightDir, normalize(vNormal)));
	
	vec3 E = normalize( CameraPos - vPosition );
	vec3 R = reflect( -LightDir, vNormal );
	
	float s = max( 0, dot( E, R ) );
	s = pow( s, SpecPow );
	
//	float shad = max(0, dot(normalize(vNormal.xyz), LightDir));
//	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias) {
//		shad = 0;
//	}
	
//	vec4 outColour = texture(waterTexture, vTexCoord * 50);
	vec4 outColour = diffuse;
	outColour.rgb = outColour.rgb * brightness + s; 
	
//	outColour = outColour * shad;
	return outColour;
}

void main()
{
//	FragColour = texture(waterTexture, vTexCoord * 50);
	FragColour = oldSchool(texture(waterTexture, vTexCoord * 50));
	FragColour = vec4(FragColour.xyz, 0.75);
}