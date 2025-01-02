#pragma once

#include <cstdint>
#include <glm/glm.hpp>

struct MeshInfo {
    uint32_t startIndex;
    uint32_t size;
    uint32_t matID;
};

struct Triangle {
    alignas(16) glm::vec3 posA;
    alignas(16) glm::vec3 posB;
    alignas(16) glm::vec3 posC;
    alignas(16) glm::vec3 normalA;
    alignas(16) glm::vec3 normalB;
    alignas(16) glm::vec3 normalC;
    alignas(8) glm::vec2 uvA;
    alignas(8) glm::vec2 uvB;
    alignas(8) glm::vec2 uvC;
    alignas(8) glm::vec2 padding;
};
