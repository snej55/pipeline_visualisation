#version 410 core
out vec4 FragColor;

in float Included;

void main()
{
    if (Included > 0.0)
        FragColor = vec4(0.13, 0.537, 0.431, 1.0);
    else
        FragColor = vec4(0.8, 0.322, 0.314, 1.0);
}