#version 330 core
out vec4 FragColor;

in float fIncluded;
in vec3 fCameraPos;
in vec3 fFragPos;

const vec3 notIncluded = vec3(0.15, 0.1, 0.514);
const vec3 included = vec3(1.0, 0.5, 0.1);

const float lightConstant = 1.0;
const float lightLinear = 0.009;
const float lightQuadratic = 0.0032;

void main()
{
    float dist = length(fCameraPos - fFragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * dist + lightQuadratic * (dist * dist));

    vec3 color;
    if (fIncluded > 0.0) {
        color = included;
        color *= attenuation * 1.5;
    } else{
        color = notIncluded;
        color *= attenuation;
    }

    FragColor = vec4(color, 1.0);
}
