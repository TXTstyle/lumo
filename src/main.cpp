#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <iostream>
#include <array>

#include "Camera.hpp"
#include "ComputeShader.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"

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
    shader.SetInt("uTex", 0);

    Vision::Camera cam(renderer, {0.0f, 100.0f, 0.0f}, {0.1f, 200.0f}, 45.0f,
                       5.0f);

    std::array<float, 20> verts = {
        -1.0f, 1.0f,  0.0f, 0.0f, 1.0f, //
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
        1.0f,  1.0f,  0.0f, 1.0f, 1.0f, //
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, //
    };

    Vision::VertexBuffer buffer(verts.data(), verts.size() * sizeof(float));

    Vision::VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(2);

    Vision::VertexArray va;
    va.AddBuffer(buffer, layout);

    glm::uvec2 imgSize = {512, 512};

    Vision::ComputeShader computeShader("res/shaders/Basic.comp");
    Vision::Texture texture(imgSize.x, imgSize.y, GL_RGBA32F);
    texture.Bind();

    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f; // time of last frame
    int fCounter = 0;

    std::cout << "Start Rendering" << std::endl;
    while (!renderer.ShouldClose()) {
        // Set frame time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (fCounter > 500) {
            std::cout << "FPS: " << 1 / deltaTime << std::endl;
            fCounter = 0;
        } else {
            fCounter++;
        }

        cam.Controls(renderer);

        computeShader.Use();
        computeShader.SetFloat("uTime", currentFrame);
        glDispatchCompute((unsigned int)imgSize.x, (unsigned int)imgSize.y, 1);
        // make sure to writing to image has finished before read
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        renderer.Clear({0.0, 0.0, 0.0});
        shader.Use();

        renderer.Draw(va, shader, 4);

        renderer.Swap();
    }
    std::cout << "End Rendering" << std::endl;

    renderer.Shutdown();
    return 0;
}
