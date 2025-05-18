#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in float aIncluded;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 camerapos;

uniform float time;

out VS_OUT {
    float Included;
    vec3 CameraPos;
    vec3 FragPos;
    float Time;
} vs_out;

void main()
{
    vs_out.Included = aIncluded;
    vs_out.CameraPos = camerapos;
    vs_out.FragPos = vec3(model * vec4(aPos + aOffset, 1.0));
    vs_out.Time = time;
    gl_Position = projection * view * model * vec4(aPos + aOffset, 1.0);
}