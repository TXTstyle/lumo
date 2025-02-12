#include "BindlessTexture.hpp"
#include "glad/glad.h"
#include "stb/stb_image.h"
#include <cstdint>
#include <iostream>

using namespace Vision;

BindlessTexture& BindlessTexture::operator=(BindlessTexture&& other) noexcept {
    if (this != &other) {
        glMakeTextureHandleNonResidentARB(handle);
        glDeleteTextures(1, &textureID);

        textureID = other.textureID;
        handle = other.handle;

        other.textureID = 0;
        other.handle = 0;
        std::cout << "[BINDLESS TEXTURE] moved, id: " << textureID
                  << ", handle: " << handle << std::endl;
    }
    return *this;
}

BindlessTexture::BindlessTexture(BindlessTexture& other) noexcept
    : textureID(other.textureID), handle(0) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Duplicate the OpenGL state of the other object's texture
    handle = glGetTextureHandleARB(textureID);
    glMakeTextureHandleResidentARB(handle);

    std::cout << "[BINDLESS TEXTURE] copied, id: " << textureID
              << ", handle: " << handle << std::endl;
}

BindlessTexture::BindlessTexture(std::string path, uint32_t format) {
    uint32_t imgFormat = GL_RGB;

    if (format == GL_RGBA8 || format == GL_RGBA32F) {
        imgFormat = GL_RGBA;
    }
    if (format == GL_R16F || format == GL_R8) {
        imgFormat = GL_RED;
    }

    int width, height, BPP;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &BPP, 0);

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, imgFormat,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    handle = glGetTextureHandleARB(textureID);
    glMakeTextureHandleResidentARB(handle);

    std::cout << "[BINDLESS TEXTURE] created, id: " << textureID
              << ", handle: " << handle << std::endl;
}

BindlessTexture::~BindlessTexture() {
    glMakeTextureHandleNonResidentARB(handle);
    glDeleteTextures(1, &textureID);

    std::cout << "[BINDLESS TEXTURE] destroyed, id: " << textureID
              << ", handle: " << handle << std::endl;
}
