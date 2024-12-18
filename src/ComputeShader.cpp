#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "ComputeShader.hpp"

using namespace Vision;

ComputeShader::ComputeShader(const char* computePath) {
    std::string computeCode;
    std::ifstream cShaderFile;

    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        cShaderFile.open(computePath);

        std::stringstream cShaderStream;
        cShaderStream << cShaderFile.rdbuf();
        cShaderFile.close();

        computeCode = cShaderStream.str();
    } catch (const std::ifstream::failure& e) {
        std::cerr << e.what() << std::endl;
    }
    const char* cShaderCode = computeCode.c_str();

    // compute shader
    uint32_t compute;
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    checkErrors(compute, "COMPUTE");

    // Shader program
    id = glCreateProgram();
    glAttachShader(id, compute);
    glLinkProgram(id);
    checkErrors(id, "PROGRAM");

    glDeleteShader(compute);

    std::cout << "Compute Shader created, id: " << id << std::endl;
}

ComputeShader::~ComputeShader() {
    std::cout << "Compute Shader destroyed, id: " << id << std::endl;
}

void ComputeShader::Use() const { glUseProgram(id); }

void ComputeShader::Dispatch(glm::vec3 groupSize) {
    glDispatchCompute((unsigned int)groupSize.x, (unsigned int)groupSize.y, (unsigned int)groupSize.z);
    // make sure to writing to image has finished before read
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void ComputeShader::SetInt(const std::string& name, int value, bool useShader) {
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}
void ComputeShader::SetIntArray(const std::string& name, int count, int* value,
                                bool useShader) {
    if (useShader)
        this->Use();
    glUniform1iv(glGetUniformLocation(id, name.c_str()), count, value);
}
void ComputeShader::SetFloat(const std::string& name, float value,
                             bool useShader) {
    if (useShader)
        this->Use();
    glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}
void ComputeShader::SetVec4f(const std::string& name, glm::vec4 values,
                             bool useShader) {
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(id, name.c_str()), values.x, values.y,
                values.z, values.w);
}
void ComputeShader::SetVec3f(const std::string& name, glm::vec3 values,
                             bool useShader) {
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(id, name.c_str()), values.x, values.y,
                values.z);
}
void ComputeShader::SetVec2f(const std::string& name, glm::vec2 values,
                             bool useShader) {
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(id, name.c_str()), values.x, values.y);
}
void ComputeShader::SetVec2i(const std::string& name, glm::ivec2 values,
                             bool useShader) {
    if (useShader)
        this->Use();
    glUniform2i(glGetUniformLocation(id, name.c_str()), values.x, values.y);
}
void ComputeShader::SetMat4f(const std::string& name, const glm::mat4& matrix,
                             bool useShader) {
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                       &matrix[0][0]);
}
void ComputeShader::SetMat3f(const std::string& name, const glm::mat3& matrix,
                             bool useShader) {
    if (useShader)
        this->Use();
    glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE,
                       &matrix[0][0]);
}

void ComputeShader::SetIntArrayInit(const std::string& name, bool useShader) {
    int samplers[32];
    for (int i = 0; i < 32; i++)
        samplers[i] = i;
    SetIntArray(name, 32, samplers, useShader);
}

void ComputeShader::checkErrors(uint32_t object, std::string type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success) {
            int32_t len = 0;
            glGetShaderInfoLog(object, sizeof(infoLog), &len, infoLog);
            if (len != 0) {
                std::cout << "| ERROR::SHADER: Compile-time error: Type: "
                          << type << "\n"
                          << infoLog << "\n------ len: " << len << std::endl;
                exit(1);
            }
        }
    } else {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(object, sizeof(infoLog), NULL, infoLog);
            std::cout << "| ERROR::SHADER: Link-time error: Type: " << type
                      << "\n"
                      << infoLog << "\n------" << std::endl;
            exit(1);
        }
    }
}
