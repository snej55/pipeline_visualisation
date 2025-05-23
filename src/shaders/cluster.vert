#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMat;

out VERTEX_DATA {
    vec3 FragPos;
    vec3 Normal;
} vs_out;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = normalMat * aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}