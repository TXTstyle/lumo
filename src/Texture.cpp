#include "Texture.hpp"
#include "stb/stb_image.h"
#include <cstdint>
#include "glad/glad.h"
#include <iostream>
#include <string>

#define IMATH_HALF_NO_LOOKUP_TABLE
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <vector>

using namespace Vision;

Texture::Texture()
    : renderID(0), width(0), height(0), format(GL_RGB8), imgFormat(GL_RGB),
      wrapS(GL_REPEAT), wrapT(GL_REPEAT), filterMin(GL_LINEAR),
      filterMax(GL_LINEAR) {}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        // Clean up current resources
        if (renderID != 0) {
            glDeleteTextures(1, &renderID);
        }

        // Transfer ownership from `other`
        renderID = other.renderID;
        width = other.width;
        height = other.height;
        format = other.format;
        imgFormat = other.imgFormat;
        wrapS = other.wrapS;
        wrapT = other.wrapT;
        filterMin = other.filterMin;
        filterMax = other.filterMax;
        index = other.index;

        // Reset the moved-from object's texture ID
        other.renderID = 0;
    }
    return *this;
}

Texture::Texture(const std::string path, const uint32_t format)
    : width(0), height(0), format(format), imgFormat(GL_RGB), wrapS(GL_REPEAT),
      wrapT(GL_REPEAT), filterMin(GL_LINEAR), filterMax(GL_LINEAR) {

    if (format == GL_RGBA8 || format == GL_RGBA32F) {
        imgFormat = GL_RGBA;
    }

    if (format == GL_R16F) {
        imgFormat = GL_RED;
    }

    if (path.ends_with(".exr")) {
        GenEXR(path, format);
        std::cout << "[TEXTURE] created, id: " << renderID << std::endl;
        return;
    }

    int width, height, BPP;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &BPP, 0);

    Generate(width, height, data);

    stbi_image_free(data);
    std::cout << "[TEXTURE] created, id: " << renderID << std::endl;
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
    std::cout << "[TEXTURE] created, id: " << renderID << std::endl;
}

Texture::~Texture() {
    std::cout << "[TEXTURE] destroyed, id: " << renderID << std::endl;
}

void Texture::GenEXR(const std::string path, const uint32_t format) {
    try {
        // OpenEXR: Read the file
        Imf::RgbaInputFile file(path.c_str());
        Imath::Box2i dw = file.dataWindow();
        width = dw.max.x - dw.min.x + 1;
        height = dw.max.y - dw.min.y + 1;

        // Allocate space for the pixels
        std::vector<Imf::Rgba> pixels(width * height);

        // Read pixel data
        file.setFrameBuffer(pixels.data() - dw.min.x - dw.min.y * width, 1,
                            width);
        file.readPixels(dw.min.y, dw.max.y);

        // Convert to OpenGL-compatible format
        std::vector<float> glPixels(width * height * 4); // RGBA = 4 channels
        for (size_t i = 0; i < pixels.size(); ++i) {
            glPixels[i * 4 + 0] = pixels[i].r; // Red
            glPixels[i * 4 + 1] = pixels[i].g; // Green
            glPixels[i * 4 + 2] = pixels[i].b; // Blue
            glPixels[i * 4 + 3] = pixels[i].a; // Alpha
        }

        // Generate the texture
        glGenTextures(1, &renderID);
        glBindTexture(GL_TEXTURE_2D, renderID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, imgFormat,
                     GL_FLOAT, glPixels.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

    } catch (const std::exception& e) {
        std::cerr << "Error reading EXR file " << path << ": " << e.what()
                  << std::endl;
    }
}

void Texture::Bind(int index) {
    if (index != -1) {
        this->index = index;
    }
    glActiveTexture(GL_TEXTURE0 + this->index);
    glBindTexture(GL_TEXTURE_2D, renderID);
}

void Texture::CopyTo(Texture& texture) {
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
