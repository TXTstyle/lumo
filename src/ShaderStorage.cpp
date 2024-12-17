#include "ShaderStorage.hpp"
#include <glad/glad.h>

using namespace Vision;

ShaderStorage::ShaderStorage(const void* data, uint32_t size) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

ShaderStorage::~ShaderStorage() {
}

void ShaderStorage::SubData(uint32_t offset, uint32_t size, const void* data) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, size, data);
}

void ShaderStorage::Bind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
}

void ShaderStorage::Unbind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
