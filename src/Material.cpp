#include "Material.hpp"
#include <iostream>

Material::Material(uint32_t id, std::string diffPath, std::string roughPath,
                   std::string normPath, uint32_t diffFormat,
                   uint32_t roughFormat, uint32_t normFormat,
                   float emissionStrength)
    : id(id), diff(diffPath, diffFormat), rough(roughPath, roughFormat),
      norm(normPath, normFormat), emissionStrength(emissionStrength) {
    diff.Bind(id * 3 + 0);
    rough.Bind(id * 3 + 1);
    norm.Bind(id * 3 + 2);
    std::cout << "Material created, id: " << id << std::endl;
}

Material::~Material() {
    std::cout << "Material destroyed, id: " << id << std::endl;
}

void Material::Bind() {
    diff.Bind();
    rough.Bind();
    norm.Bind();
}
MatData Material::GetData() {
    MatData data;
    data.emissionStrength = emissionStrength;
    data.diffIndex = id * 3 + 0;
    data.roughIndex = id * 3 + 1;
    data.normIndex = id * 3 + 2;

    return data;
}
