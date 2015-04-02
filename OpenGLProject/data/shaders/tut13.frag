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
uniform float shadowBias;

uniform float roughness;
uniform float fresnel;

vec4 oldSchool()
{
	mat3 TBN = mat3(
		normalize( vTangent ),
		normalize( vBiTangent ),
		normalize( vNormal ));
		
	vec3 N = texture( normal,
		vTexCoord).xyz * 2 - 1;
	
	float d = max(0, dot( normalize(TBN * N), normalize(LightDir) ) );
	vec3 E = normalize( CameraPos - vPosition );
	vec3 R = reflect( -LightDir, vNormal );
	
	float s = max( 0, dot( E, R ) );
	s = pow( s, SpecPow );
	
	float shad = max(0, dot(normalize(vNormal.xyz), LightDir));
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias) {
		shad = 0;
	}
	
	vec4 output = texture(diffuse, vTexCoord);
	
	output.rgb = output.rgb * d + s * 0.1; 
	
	output = output * shad;
	return output;
}

vec4 realistic()
{
	vec3 L = LightDir;
	vec3 E = vPosition - CameraPos;
	vec3 N = vNormal;
	
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
	
	float shad = max(0, dot(normalize(vNormal.xyz), LightDir));
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z - shadowBias) {
		shad = 0;
	}
	
	vec4 output = texture(diffuse, vTexCoord);
	
	output.rgb = output.rgb * (OrenNayar + CookTorrance);
	output = output * shad;
	return output;
	
}

void main() 
{
	FragColor = realistic();
	
}
