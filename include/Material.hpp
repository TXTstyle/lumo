#pragma once

#include "Texture.hpp"
#include <cstdint>
#include <string>

struct alignas(4) MatData {
    uint32_t diffIndex;
    uint32_t roughIndex;
    uint32_t normIndex;
    float emissionStrength = 0;
};

class Material {
  private:
    uint32_t id;

    Vision::Texture diff;
    Vision::Texture rough;
    Vision::Texture norm;
    float emissionStrength = 0;

  public:
    Material(uint32_t id, std::string diffPath, std::string roughPath,
             std::string normPath, uint32_t diffFormat, uint32_t roughFormat, uint32_t normFormat, float emissionStrength = 0);
    Material(Material&&) = default;
    Material(const Material&) = default;
    Material& operator=(Material&&) = default;
    Material& operator=(const Material&) = default;
    ~Material();

    inline uint32_t GetId() { return id; }
    void Bind();

    MatData GetData();
};
