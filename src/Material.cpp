#include "Material.hpp"
#include <iostream>
#include <vector>

Material::Material(uint32_t id, std::string diffPath, std::string roughPath,
                   std::string normPath, uint32_t diffFormat,
                   uint32_t roughFormat, uint32_t normFormat,
                   float emissionStrength)
    : id(id), rough(roughPath, roughFormat), norm(normPath, normFormat),
      diff(diffPath, diffFormat), emissionStrength(emissionStrength) {

    data.emissionStrength = emissionStrength;
    data.roughIndex = id * 3 + 0;
    data.normIndex = id * 3 + 1;
    data.diffIndex = id * 3 + 2;

    std::cout << "Material created, id: " << id << std::endl;
}

Material::~Material() {
    std::cout << "Material destroyed, id: " << id << std::endl;
}

void Material::PushHandles(std::vector<uint64_t>& vec) {
    vec.push_back(rough.GetHandle());
    vec.push_back(norm.GetHandle());
    vec.push_back(diff.GetHandle());
}
