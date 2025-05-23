// screen shader for ui
#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float gamma = 2.2;

void main() {
    vec3 color = vec3(texture(screenTexture, TexCoords));

    // subtle vignette
    float centerDis = pow(distance(vec2(0.5, 0.5), TexCoords), 6) * 0.2;
    color -= centerDis;

    // exposure tone mapping (hdr)
    vec3 mapped = vec3(1.0) - exp(-color * 5.0);
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}
