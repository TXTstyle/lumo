#version 450 core

layout(location = 0) out vec4 FragColor;

in vec2 vTexCoords;

uniform sampler2D uTex;
uniform sampler2D uTexOld;

uniform float uTime;

void main() {
    vec3 texColor = texture(uTex, vTexCoords).rgb;
    vec3 texColorOld = texture(uTexOld, vTexCoords).rgb;

    float weight = 1.0 / (uTime + 1);
    vec3 texColorAvg = texColorOld * (1 - weight) + texColor * weight;


    vec3 res;
    if (vTexCoords.x <= 0.5) {
        res = texColor;
    } else {
        res = texColorOld;
    }

    FragColor = vec4(texColorAvg, 1.0);
}
