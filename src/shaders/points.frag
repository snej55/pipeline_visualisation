#version 410 core
out vec4 FragColor;

in float fIncluded;

const vec3 red = vec3(0.8, 0.322, 0.314);
const vec3 green = vec3(0.13, 0.537, 0.431);

void main()
{
    if (fIncluded > 0.0)
        FragColor = vec4(green, 1.0);
    else
        FragColor = vec4(red, 1.0);
}