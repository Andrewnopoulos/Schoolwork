#version 410
in vec3 vNormal;
in vec3 vPosition;
in vec2 vTexCoord;
in vec3 vTangent;
in vec3 vBiTangent;
in vec4 vShadowCoord;

out vec4 FragColor;

uniform vec3 LightDir;
uniform vec3 LightColour;
uniform vec3 CameraPos;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D shadowMap;
uniform float SpecPow;

void main() 
{
//	mat3 TBN = mat3(
//		normalize( vTangent ),
//		normalize( vBiTangent ),
//		normalize( vNormal ));
		
//	vec3 N = texture( normal,
//		vTexCoord).xyz * 2 - 1;
	
//	float d = max(0, dot( normalize(TBN * N), normalize(LightDir) ) );
//	vec3 E = normalize( CameraPos - vPosition );
//	vec3 R = reflect( -LightDir, vNormal );
	
//	float s = max( 0, dot( E, R ) );
//	s = pow( s, SpecPow );
	
	float d = max(0, dot(normalize(vNormal.xyz), LightDir));
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z) {
		d = 0;
	}
	
	FragColor = texture(diffuse, vTexCoord);
	
	FragColor = FragColor * d;
	
	
	
//	FragColor.rgb = FragColor.rgb * LightColour * d + LightColour * s * 0.1; 
}
