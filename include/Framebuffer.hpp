#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Vision {
class Framebuffer {
  private:
    uint32_t RenderID;

  public:
    Framebuffer(std::vector<uint32_t>& attachments);
    ~Framebuffer();

    void CopyFromDefault(glm::vec2& size);

    void Bind();
    void UnBind();

    inline uint32_t GetID() { return RenderID; }
};
} // namespace Vision
