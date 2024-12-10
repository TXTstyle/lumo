#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <iostream>
#include <array>

#include "Camera.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"

int main() {
    Vision::Renderer renderer;
    try {
        renderer.Init({1280, 720}, "dev");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        renderer.Shutdown();
        return EXIT_FAILURE;
    }

    Vision::Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
    shader.Use();

    Vision::Camera cam(renderer, {0.0f, 100.0f, 0.0f}, {0.1f, 200.0f}, 45.0f,
                       5.0f);

    std::array<float, 27> verts = {
        -0.75f, -0.75f, 0, 1.0f, 0.0f, 0.0f, //
        0.75f,  -0.75f, 0, 0.0f, 1.0f, 0.0f, //
        0.0f,   0.75f,  0, 0.0f, 0.0f, 1.0f, //
    };

    Vision::VertexBuffer buffer(verts.data(), verts.size() * sizeof(float));

    Vision::VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);

    Vision::VertexArray va;
    va.AddBuffer(buffer, layout);

    std::cout << "Start Rendering" << std::endl;
    while (!renderer.ShouldClose()) {
        renderer.Clear({0.8, 0.8, 0.8});
        cam.Controls(renderer);

        renderer.Draw(va, shader, 3);

        renderer.Swap();
    }
    std::cout << "End Rendering" << std::endl;

    renderer.Shutdown();
    return 0;
}
