#include "Framebuffer.hpp"
#include <glad/glad.h>

using namespace Vision;

Framebuffer::Framebuffer(std::vector<u_int32_t> &attachments) {
    glGenFramebuffers(1, &RenderID);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderID);

    for (int i = 0; i < attachments.size(); i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, attachments[i], 0);
    }

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    u_int32_t colorAttachments[attachments.size()];
    for (int i = 0; i < attachments.size(); i++) {
        colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    glDrawBuffers(attachments.size(), colorAttachments);
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &RenderID);
}

void Framebuffer::CopyFromDefault(glm::vec2& size) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, RenderID);
        glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x,
                          size.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Framebuffer::Bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, RenderID);
}

void Framebuffer::UnBind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
