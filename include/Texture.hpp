#pragma once

#include <cstdint>
#include <string>
#include <glad/glad.h>

namespace Vision {
class Texture {
  private:
    unsigned int renderID;
    uint32_t width, height, BPP;
    uint32_t format, imgFormat;
    uint32_t wrapS, wrapT;
    uint32_t filterMin, filterMax;
    uint32_t index = 0;

    void Generate(uint32_t width, uint32_t height, unsigned char* data);
    void GenEXR(const std::string path, const uint32_t format);

  public:
    Texture(const std::string path, const uint32_t format);
    Texture(const uint32_t width, const uint32_t height, const uint32_t format, uint32_t index);
    ~Texture();

    void CopyTo(Texture &texture);

    inline uint32_t& GetID() { return renderID; }
    // -1 = use last
    void Bind(int index = -1);
};
} // namespace Vision
