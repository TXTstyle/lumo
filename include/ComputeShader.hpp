#pragma once

#include <cstdint>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Vision {
class ComputeShader {
  private:
    uint32_t id;

    void checkErrors(uint32_t object, std::string type);

  public:
    ComputeShader(const char* computePath);
    ~ComputeShader();

    void Use() const;
    inline uint32_t& GetID() { return id; }

    void SetInt(const std::string& name, int value, bool useShader = false);
    void SetIntArray(const std::string& name, int count, int* value,
                     bool useShader = false);
    void SetFloat(const std::string& name, float value, bool useShader = false);
    void SetVec4f(const std::string& name, glm::vec4 values,
                  bool useShader = false);
    void SetVec3f(const std::string& name, glm::vec3 values,
                  bool useShader = false);
    void SetVec2f(const std::string& name, glm::vec2 values,
                  bool useShader = false);
    void SetMat4f(const std::string& name, const glm::mat4& matrix,
                  bool useShader = false);

    void SetIntArrayInit(const std::string& name, bool useShader = true);
};
} // namespace Vision
