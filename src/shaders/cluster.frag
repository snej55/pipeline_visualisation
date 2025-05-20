#version 410 core

out vec4 FragColor;

in VERTEX_DATA {
    vec3 FragPos;
    vec3 Normal;
} vs_in;

uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.0);
}