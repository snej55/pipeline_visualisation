#version 330 core
out vec4 FragColor;

in float fIncluded;
in vec3 fCameraPos;
in vec3 fFragPos;

const vec3 red = vec3(0.8, 0.322, 0.314);
const vec3 green = vec3(0.13, 0.537, 0.431);

const float lightConstant = 1.0;
const float lightLinear = 0.009;
const float lightQuadratic = 0.0032;

void main()
{
    float dist = length(fCameraPos - fFragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * dist + lightQuadratic * (dist * dist));

    vec3 color;
    if (fIncluded > 0.0)
        color = green;
    else
        color = red;
    // color *= attenuation;
    FragColor = vec4(color, 1.0);
}