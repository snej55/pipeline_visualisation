#version 330 core
out vec4 FragColor;

in VS_OUT {
    float Included;
    vec3 CameraPos;
    vec3 FragPos;
    vec3 Normal;
    float Time;
} vs_in;

const vec3 notIncluded = vec3(0.15, 0.1, 0.514);
const vec3 included = vec3(1.0, 0.5, 0.1);
const vec3 lightColor = vec3(1.0);

const float lightConstant = 1.0;
const float lightLinear = 0.0009;
const float lightQuadratic = 0.00032;

const float ambientStrength = 0.01;

void main()
{
    float dist = length(vs_in.CameraPos - vs_in.FragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * dist + lightQuadratic * (dist * dist));

    vec3 color;
    if (vs_in.Included > 0.0)
        color = included;
    else
        color = notIncluded;
    
    // ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(vs_in.Normal);
    vec3 lightDir = normalize(vs_in.CameraPos - vs_in.FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    diffuse *= attenuation;

    vec3 result = (ambient + diffuse) * color;

    FragColor = vec4(result, 1.0);
}