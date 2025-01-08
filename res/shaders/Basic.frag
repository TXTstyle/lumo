#version 450 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D uTex;
uniform sampler2D uTexOld;

uniform float uTime;

void main() {
    vec3 texColor = texture(uTex, vTexCoords).rgb;

    // reinhard tone mapping
    vec3 mapped = texColor / (texColor + vec3(1.0));
    // gamma correction 
    mapped = pow(mapped, vec3(1.0 / 2));

    vec3 texColorOld = texture(uTexOld, vTexCoords).rgb;

    float weight = 1.0 / (uTime + 1);
    vec3 texColorAvg = texColorOld * (1 - weight) + mapped * weight;

    FragColor = vec4(texColorAvg, 1.0);
}
