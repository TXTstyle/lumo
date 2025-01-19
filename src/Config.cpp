#include "Config.hpp"
#include "Camera.hpp"
#include "Loader.hpp"
#include "Texture.hpp"
#include "toml++/toml.hpp"
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

GLint ToGLFormat(std::string& str) {
    if (str == "RGB8") {
        return GL_RGB8;
    }
    if (str == "R8") {
        return GL_R8;
    }
    return GL_RGB8;
}

Config::Config(std::string path) {
    toml::table tbl = toml::parse_file(path);
    std::cout << "[CONFIG] Scene file parsed" << std::endl;

    res.x = tbl["width"].value_or(0);
    res.y = tbl["height"].value_or(0);

    std::cout << "[CONFIG] Resolution set to: x: " << res.x << ", y: " << res.y
              << std::endl;

    glm::vec3 camPos;
    camPos.x = tbl["Camera"]["pos"]["x"].value_or(0.0f);
    camPos.y = tbl["Camera"]["pos"]["y"].value_or(0.0f);
    camPos.z = tbl["Camera"]["pos"]["z"].value_or(0.0f);

    float fov = tbl["Camera"]["fov"].value_or(45.0f);

    cam = Vision::Camera(camPos, fov, 0);
    std::cout << "[CONFIG] Camera set to: x: " << camPos.x << ", y: " << camPos.y
              << ", z: " << camPos.z << ", fov: " << fov << std::endl;

    gamma = tbl["gamma"].value_or(2.0f);
    std::cout << "[CONFIG] Gamma set to: " << gamma << std::endl;

    bounces = tbl["bounces"].value_or(8);
    std::cout << "[CONFIG] Bounces set to: " << bounces << std::endl;

    raysPerPixel = tbl["samples"].value_or(50);
    std::cout << "[CONFIG] Samples set to: " << raysPerPixel << std::endl;

    outputPath = tbl["output_path"].value_or(std::string("./saved-frame.png"));
    std::cout << "[CONFIG] Output path set to: " << outputPath << std::endl;

    std::string envPath = tbl["Enviroment"]["path"].value_or(std::string(""));
    envTex = Vision::Texture(envPath, GL_RGB32F);
    std::cout << "[CONFIG] Enviroment image loaded: " << envPath << std::endl;

    std::string meshPath = tbl["Mesh"]["path"].value_or(std::string(""));
    loader = Loader(meshPath);
    std::cout << "[CONFIG] Mesh loaded: " << meshPath << std::endl;

    auto mats = tbl["Materials"]["order"].as_array();
    std::vector<std::string> matNames;

    mats->for_each([&](auto&& el) {
        std::string name = el.value_or("");
        std::cout << "[CONFIG] Material name added: " << name << std::endl;
        matNames.push_back(name);
    });
    materials.reserve(matNames.size());

    for (size_t i = 0; i < matNames.size(); i++) {
        std::string& name = matNames[i];

        std::string diffPath =
            tbl["Materials"][name]["diffuse"]["path"].value_or("");
        std::string roughPath =
            tbl["Materials"][name]["roughness"]["path"].value_or("");
        std::string normPath =
            tbl["Materials"][name]["normal"]["path"].value_or("");

        std::string diffFormat =
            tbl["Materials"][name]["diffuse"]["format"].value_or("RGB8");
        std::string roughFormat =
            tbl["Materials"][name]["roughness"]["format"].value_or("R8");
        std::string normFormat =
            tbl["Materials"][name]["normal"]["format"].value_or("RGB8");

        materials.emplace_back(i, diffPath, roughPath, normPath,
                               ToGLFormat(diffFormat), ToGLFormat(roughFormat),
                               ToGLFormat(normFormat));

        std::cout << "[CONFIG] Material " << name << " added, index: " << i << std::endl;
        std::cout << "\t" << diffPath << " " << diffFormat << std::endl;
        std::cout << "\t" << roughPath << " " << roughFormat << std::endl;
        std::cout << "\t" << normPath << " " << normFormat << std::endl;
    }
}
