#pragma once

#include "BindlessTexture.hpp"
#include <cstdint>
#include <string>
#include <vector>

struct alignas(4) MatData {
    uint32_t diffIndex;
    uint32_t roughIndex;
    uint32_t normIndex;
    float emissionStrength = 0;
};

class Material {
  private:
    uint32_t id;
    MatData data;

    Vision::BindlessTexture rough;
    Vision::BindlessTexture norm;
    Vision::BindlessTexture diff;
    float emissionStrength = 0;

  public:
    Material(uint32_t id, std::string diffPath, std::string roughPath,
             std::string normPath, uint32_t diffFormat, uint32_t roughFormat,
             uint32_t normFormat, float emissionStrength = 0);
    Material(Material&&) = default;
    Material(const Material&) = default;
    Material& operator=(Material&&) = default;
    Material& operator=(const Material&) = default;
    ~Material();

    inline uint32_t GetId() { return id; }
    void PushHandles(std::vector<uint64_t>& vec);

    inline MatData GetData() { return data; }
};
