#include "Texture.hpp"
#include "stb/stb_image.h"
#include <cstdint>
#include <glad/glad.h>
#include <iostream>
#include <string>

using namespace Vision;

Texture::Texture(const std::string path, const bool alpha)
    : width(0), height(0), format(GL_RGB), imgFormat(GL_RGB), wrapS(GL_REPEAT),
      wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMax(GL_LINEAR) {

    if (alpha) {
        format = GL_RGBA8;
        imgFormat = GL_RGBA;
    }

    int width, height, BPP;
    stbi_set_flip_vertically_on_load(0);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &BPP, 0);

    Generate(width, height, data);

    stbi_image_free(data);
}

Texture::Texture(const uint32_t width, const uint32_t height,
                 const uint32_t format, uint32_t index)
    : width(width), height(height), format(format) {
    glGenTextures(1, &renderID);
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, renderID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_FLOAT,
                 NULL);

    glBindImageTexture(0, renderID, 0, GL_FALSE, 0, GL_READ_WRITE, format);
    std::cout << "Texture created, id: " << renderID << std::endl;
}

Texture::~Texture() {
    std::cout << "Texture destroyed, id: " << renderID << std::endl;
}

void Texture::Bind(int index){
    if (index != -1) {
        this->index = index;
    }
    glActiveTexture(GL_TEXTURE0 + this->index);
    glBindTexture(GL_TEXTURE_2D, renderID);
}

void Texture::CopyTo(Texture &texture) {
    glCopyImageSubData(renderID, GL_TEXTURE_2D, 0, 0, 0, 0, texture.GetID(),
                       GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1);
}

void Texture::Generate(uint32_t width, uint32_t height, unsigned char* data) {
    this->width = width;
    this->height = height;

    glGenTextures(1, &renderID);

    // Create Texture
    glBindTexture(GL_TEXTURE_2D, renderID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, imgFormat,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMax);

    glBindTexture(GL_TEXTURE_2D, 0);
}
