#pragma once

#include <cstdint>
namespace Vision {
class ShaderStorage {
  private:
    uint32_t id;

  public:
    ShaderStorage(const void* data, uint32_t size, const uint32_t bindIdx);
    ~ShaderStorage();

    void SubData(uint32_t offset, uint32_t size, const void* data);

    void Bind() const;
    void Unbind() const;
};
} // namespace Vision
