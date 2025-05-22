#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aOffset;
layout (location = 3) in float aIncluded;
layout (location = 4) in float aCounter;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 camerapos;

uniform float time;

out VS_OUT {
    float Included;
    vec3 CameraPos;
    vec3 FragPos;
    vec3 Normal;
    float Time;
    float Counter;
} vs_out;

void main()
{
    vs_out.Counter = aCounter;
    vs_out.Included = aIncluded;
    vs_out.CameraPos = camerapos;
    vec3 uv = aPos; // can be modified
//    float angle = aCounter;
//    mat3 rotmat = mat3 (
//            0.0, 0.0, 0.0,
//            0.0, cos(angle), sin(angle),
//            0.0, -sin(angle), cos(angle)
//    ) * mat3(
//            cos(angle * 1.5), 0.0, sin(angle * 1.5),
//            0.0, 1.0, 0.0,
//            -sin(angle * 1.5), 0.0, cos(angle * 1.5)
//    );
    vs_out.Normal = aNormal;
    vs_out.FragPos = vec3(model * vec4(uv + aOffset, 1.0));
    vs_out.Time = time;
    gl_Position = projection * view * model * vec4(uv + aOffset, 1.0);
}