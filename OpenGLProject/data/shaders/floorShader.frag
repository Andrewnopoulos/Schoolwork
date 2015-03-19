#version 410

in vec4 vNormal;

out vec4 FragColor;

uniform vec3 lightDir;

void main() { 
	float d = max(0, dot(normalize(vNormal.xyz), lightDir));
	FragColor = vec4(1, 0, 0, 1); //d, d, d, 1);
}