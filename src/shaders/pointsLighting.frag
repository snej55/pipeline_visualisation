#version 330 core
out vec4 FragColor;

in VS_OUT {
    float Included;
    vec3 CameraPos;
    vec3 FragPos;
    vec3 Normal;
    float Time;
    float Counter;
} vs_in;

const vec3 notIncluded = vec3(1.0, 0.0, 0.0);
const vec3 included = vec3(0.0, 1.0, 0.0);
const vec3 lightColor = vec3(1.0);

const float lightConstant = 1.0;
const float lightLinear = 0.0009;
const float lightQuadratic = 0.00032;

const float ambientStrength = 0.01;

uniform int lastIndex;

void main()
{
    float dist = length(vs_in.CameraPos - vs_in.FragPos);
    float attenuation = 1.0 / (lightConstant + lightLinear * dist + lightQuadratic * (dist * dist));

    vec3 color;
    if (vs_in.Included > 0.0)
        color = included;
    else
        color = notIncluded;

    float explored = 0.0;
    float progress = vs_in.Time;
    if (progress > vs_in.Counter)
        explored = 1.0;
    
    // check if this paper was explored
    if (vs_in.Counter > lastIndex)
        explored = 0.0;

    if (explored < 1.0)
        color = vec3(0.05);
    
    // ambient lighting
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(vs_in.Normal);
    vec3 lightDir = normalize(vs_in.CameraPos - vs_in.FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    diffuse *= attenuation;

    float intensity = vs_in.Counter / 149945.0;
    vec3 result = (ambient + diffuse) * color;

    FragColor = vec4(result, 1.0);
}