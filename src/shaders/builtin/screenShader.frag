#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float gamma = 2.2;

void main() {
    vec3 color = vec3(texture(screenTexture, TexCoords));

    // exposure tone mapping (hdr)
    vec3 mapped = vec3(1.0) - exp(-color * 5.0);
    // gamma correction
    mapped = pow(mapped, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}
