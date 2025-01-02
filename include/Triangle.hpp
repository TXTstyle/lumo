#pragma once

#include <glm/glm.hpp>

struct Info {
    glm::vec2 uvA;
    glm::vec2 uvB;
    glm::vec2 uvC;
    float useMat;
    float roughness;
};

struct Triangle {
    glm::vec4 posA;
    glm::vec4 posB;
    glm::vec4 posC;
    glm::vec4 normalA;
    glm::vec4 normalB;
    glm::vec4 normalC;
    glm::vec4 color;
    Info info;
};
