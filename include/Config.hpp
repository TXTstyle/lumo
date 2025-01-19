#pragma once

#include "Camera.hpp"
#include "Loader.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Config {
  private:
    Vision::Texture envTex;
    std::vector<Material> materials;
    Vision::Camera cam;
    Loader loader;
    glm::vec2 res;
    uint32_t bounces;
    uint32_t raysPerPixel;
    std::string outputPath;
    float gamma = 2.0;

  public:
    Config(std::string path);
    Config(Config&&) = default;
    Config(const Config&) = default;
    Config& operator=(Config&&) = default;
    Config& operator=(const Config&) = default;
    ~Config() = default;

    inline uint32_t GetBounces() { return bounces; }
    inline uint32_t GetRaysPerPixel() { return raysPerPixel; }
    inline float GetGamma() { return gamma; }
    inline glm::vec2& GetRes() { return res; }
    inline std::string& GetOutputPath() { return outputPath; }
    inline Loader& GetLoader() { return loader; }
    inline Vision::Camera& GetCamera() { return cam; }
    inline Vision::Texture& GetEnv() { return envTex; }
    inline std::vector<Material>& GetMaterials() { return materials; }
};
