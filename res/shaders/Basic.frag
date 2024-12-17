#version 450 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D uTex;

void main() {
    vec3 texColor = texture(uTex, vTexCoords).rgb;
    FragColor = vec4(texColor, 1.0);
}
