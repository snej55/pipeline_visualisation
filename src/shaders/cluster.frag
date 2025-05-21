#version 410 core

out vec4 FragColor;

in VERTEX_DATA {
    vec3 FragPos;
    vec3 Normal;
} vs_in;

uniform vec3 color;
uniform vec3 CameraPos;
uniform int lighting;

const vec3 lightColor = vec3(1.0);

const float lightConstant = 1.0;
const float lightLinear = 0.0009;
const float lightQuadratic = 0.00032;

const float ambientStrength = 0.01;

void main()
{
    float dist = length(CameraPos - vs_in.FragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * dist + lightQuadratic * (dist * dist));
    
    // ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(vs_in.Normal);
    vec3 lightDir = normalize(CameraPos - vs_in.FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    diffuse *= attenuation;

    vec3 result = (ambient + diffuse) * color;
    if (lighting == 1)
        FragColor = vec4(result, 0.3);
    else
        FragColor = vec4(color * 1.5 * attenuation, 1.0);
}