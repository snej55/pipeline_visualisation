#version 410 core
layout (points) in;
layout (points, max_vertices = 1) out;

in VS_OUT {
    float Included;
} gs_in[];

out float fIncluded;

void main() {
    gl_PointSize = 2.0;
    fIncluded = gs_in[0].Included;

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    EndPrimitive();
}