#version 410 core
layout (points) in;
layout (points, max_vertices = 1) out;

in VS_OUT {
    float Included;
    vec3 CameraPos;
    vec3 FragPos;
    float Time;
} gs_in[];

out float fIncluded;
out vec3 fCameraPos;
out vec3 fFragPos;

void main() {
    gl_PointSize = 5.0;
    fIncluded = gs_in[0].Included;
    fCameraPos = gs_in[0].CameraPos;
    fFragPos = gs_in[0].FragPos;

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    EndPrimitive();
}