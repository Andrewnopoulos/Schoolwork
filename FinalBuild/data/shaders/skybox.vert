#version 410

layout (location = 0) in vec3 position;
out vec3 TexCoords;

uniform mat4 Projection;
uniform mat4 View;

void main()
{
    gl_Position = Projection * mat4(mat3(View)) * vec4(position, 1.0);  
    TexCoords = position;
}  