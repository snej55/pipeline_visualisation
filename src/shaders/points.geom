#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

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
    fIncluded = gs_in[0].Included;
    fCameraPos = gs_in[0].CameraPos;
    fFragPos = gs_in[0].FragPos;

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    fIncluded = gs_in[1].Included;
    fCameraPos = gs_in[1].CameraPos;
    fFragPos = gs_in[1].FragPos;

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    fIncluded = gs_in[2].Included;
    fCameraPos = gs_in[2].CameraPos;
    fFragPos = gs_in[2].FragPos;
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}