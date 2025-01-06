#include "Loader.hpp"
#include "Triangle.hpp"
#include <cstddef>
#include <glm/geometric.hpp>
#include <glm/glm.hpp>
#include <loader/tiny_obj_loader.h>
#include <stdexcept>
#include <vector>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
};

Loader::Loader(std::string path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                          path.c_str())) {
        throw std::runtime_error(warn + err);
    }

    size_t currentFirst = 0;

    for (const auto& shape : shapes) {
        std::vector<Vertex> verts;
        MeshInfo mesh;

        for (const auto& index : shape.mesh.indices) {
            Vertex vert;

            vert.pos = {attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]};

            vert.normal = {attrib.vertices[3 * index.normal_index + 0],
                           attrib.vertices[3 * index.normal_index + 1],
                           attrib.vertices[3 * index.normal_index + 2]};

            vert.uv = {attrib.texcoords[2 * index.texcoord_index + 0],
                       attrib.texcoords[2 * index.texcoord_index + 1]};

            verts.push_back(vert);
        }

        size_t trigCount = 0;
        for (size_t i = 0; i < verts.size(); i += 3) {
            Triangle trig;

            // Pos
            trig.posA = verts[i + 0].pos;
            trig.posB = verts[i + 1].pos;
            trig.posC = verts[i + 2].pos;

            // Normal
            glm::vec3 pq = trig.posA - trig.posC;
            glm::vec3 pr = trig.posB - trig.posC;
            trig.normal = glm::normalize(glm::cross(pq, pr));

            // UV
            trig.uvA = verts[i + 0].uv;
            trig.uvB = verts[i + 1].uv;
            trig.uvC = verts[i + 2].uv;

            trigs.push_back(trig);
            trigCount++;
        }

        mesh.startIndex = currentFirst;
        mesh.size = trigCount;
        mesh.matID = shape.name == "light" ? 0 : 1;

        currentFirst += mesh.size;
        meshes.push_back(mesh);
    }
}

Loader::~Loader() {}
