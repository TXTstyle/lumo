#pragma once

#include "Triangle.hpp"
#include <string>
#include <vector>
class Loader {
  private:
    std::vector<Triangle> trigs;
    std::vector<MeshInfo> meshes;

  public:
    Loader(std::string path);
    Loader() = default;
    Loader(Loader&&) = default;
    Loader(const Loader&) = default;
    Loader& operator=(Loader&&) = default;
    Loader& operator=(const Loader&) = default;
    ~Loader();

    inline std::vector<Triangle>& GetTrigs() { return trigs; }
    inline std::vector<MeshInfo>& GetMeshes() { return meshes; }
};
