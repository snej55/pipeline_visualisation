#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in float aIncluded;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out VS_OUT {
    float Included;
} vs_out;

void main()
{
    vs_out.Included = aIncluded;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}