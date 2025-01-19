#pragma once

#include <cstdint>
#include <string>

namespace Vision {
class BindlessTexture {
  private:
    uint32_t textureID;
    uint64_t handle;

  public:
    BindlessTexture(std::string path, uint32_t format);
    BindlessTexture(BindlessTexture& other) noexcept;
    BindlessTexture(const BindlessTexture& other) = default;
    BindlessTexture& operator=(BindlessTexture&&) noexcept;
    BindlessTexture& operator=(const BindlessTexture& other) = default;
    ~BindlessTexture();

    inline uint64_t GetHandle() { return handle; }
};
} // namespace Vision
