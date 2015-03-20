#version 410

in vec2 fTexCoord;
out vec4 FragColour;

uniform sampler2D target;

// simple box blur
vec4 BoxBlur() {
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	// 9-tap box kernel
	vec4 colour = texture(target, fTexCoord);
	colour += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(-texel.x, 0));
	colour += texture(target, fTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, fTexCoord + vec2(0, texel.y));
	colour += texture(target, fTexCoord + vec2(0, -texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, 0));
	colour += texture(target, fTexCoord + vec2(texel.x, -texel.y));
	return colour / 9;
}

vec4 Gaussian() {
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	// 9-tap box kernel
	vec4 colour = texture(target, fTexCoord) * 4;
	colour += texture(target, fTexCoord + vec2(-texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(-texel.x, 0)) * 2;
	colour += texture(target, fTexCoord + vec2(-texel.x, -texel.y));
	colour += texture(target, fTexCoord + vec2(0, texel.y)) * 2;
	colour += texture(target, fTexCoord + vec2(0, -texel.y)) * 2;
	colour += texture(target, fTexCoord + vec2(texel.x, texel.y));
	colour += texture(target, fTexCoord + vec2(texel.x, 0)) * 2;
	colour += texture(target, fTexCoord + vec2(texel.x, -texel.y));
	return colour / 16;
}

vec4 Edgy()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	
	vec4 colour = texture(target, fTexCoord) * 8;
	colour += texture(target, fTexCoord + vec2(-texel.x, texel.y)) * -1;
	colour += texture(target, fTexCoord + vec2(-texel.x, 0)) * -1;
	colour += texture(target, fTexCoord + vec2(-texel.x, -texel.y)) * -1;
	colour += texture(target, fTexCoord + vec2(0, texel.y)) * -1;
	colour += texture(target, fTexCoord + vec2(0, -texel.y)) * -1;
	colour += texture(target, fTexCoord + vec2(texel.x, texel.y)) * -1;
	colour += texture(target, fTexCoord + vec2(texel.x, 0)) * -1;
	colour += texture(target, fTexCoord + vec2(texel.x, -texel.y)) * -1;
	return colour;
}

vec4 Sharpen()
{
	vec2 texel = 1.0f / textureSize(target, 0).xy;
	
	vec4 colour = texture(target, fTexCoord) * 5;
	colour += texture(target, fTexCoord + vec2(-texel.x, 0)) * -1;
	colour += texture(target, fTexCoord + vec2(0, texel.y)) * -1;
	colour += texture(target, fTexCoord + vec2(0, -texel.y)) * -1;
	colour += texture(target, fTexCoord + vec2(texel.x, 0)) * -1;
	return colour;
}

vec4 Distort() {
	vec2 mid = vec2(0.5f);
	float distanceFromCentre = distance(fTexCoord, mid);
	vec2 normalizedCoord = normalize(fTexCoord - mid);
	float bias = distanceFromCentre +
	sin(distanceFromCentre * 15) * 0.02f;
	vec2 newCoord = mid + bias * normalizedCoord;
	return texture(target, newCoord);
}

vec4 Simple()
{
	return texture(target, fTexCoord);
}

void main() 
{
	FragColour = Distort() ;
}

