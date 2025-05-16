#version 410 core
out vec4 FragColor;

in float Included;

void main()
{
    if (Included > 0.0)
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    else
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}