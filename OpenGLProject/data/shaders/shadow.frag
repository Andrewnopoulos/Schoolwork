#version 410
in vec4 vNormal;
in vec4 vShadowCoord;

out vec4 FragColour;

uniform vec3 lightDir;
uniform sampler2D shadowMap;

void main() {
	float d = max(0, dot(normalize(vNormal.xyz), lightDir));
	if (texture(shadowMap, vShadowCoord.xy).r < vShadowCoord.z) {
		d = 0;
	}
	FragColour = vec4(d, d, d, 1);
}