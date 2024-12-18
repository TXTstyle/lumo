#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <glm/trigonometric.hpp>
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
#include "ShaderStorage.hpp"

struct Sphere {
    glm::vec4 origin;
    glm::vec4 color;
};

int main() {
    Vision::Renderer renderer;
    glm::uvec2 imgSize = {1280, 720};
    try {
        renderer.Init(imgSize, "dev");
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        renderer.Shutdown();
        return EXIT_FAILURE;
    }

    Vision::Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
    shader.Use();
    shader.SetInt("uTex", 0);

    Vision::Camera cam({0.0f, 0.0f, 0.0f}, 45.0f, 5.0f);

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

    std::array<Sphere, 3> objects = {
        Sphere{
            {1.0f, 2.0f, 1.0f, 0.5f},
            {0.0f, 1.0f, 0.0f, 1.0f},
        },
        Sphere{
            {-1.0f, 1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 1.0f, 1.0f},
        },
        Sphere{
            {0.0f, -2.0f, 0.0f, 2.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
        },
    };
    // std::array<float, 21> objects = {
    //         1.0f, 2.0f, 1.0f,
    //     0.5f,
    //         0.0f, 1.0f, 0.0f,
    //         -1.0f, 1.0f, 1.0f,
    //         0.0f, 0.0f, 1.0f,
    //         2.0f,
    //         0.0f, -2.0f, 0.0f,
    //         1.0f, 0.0f, 0.0f,
    //         1.0f,
    // };

    Vision::ShaderStorage ssbo(objects.data(), objects.size() * sizeof(Sphere),
                               3);
    ssbo.Bind();

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
            std::cout << "FPS: " << 1 / deltaTime
                      << " CamPos: " << cam.getPos().x << " " << cam.getPos().y
                      << " " << cam.getPos().z << std::endl;
            fCounter = 0;
        } else {
            fCounter++;
        }
        cam.Controls(renderer);

        computeShader.Use();
        // computeShader.SetFloat("uTime", currentFrame);
        computeShader.SetMat3f("uCamMat", cam.getMat());
        computeShader.SetFloat("uFOV", cam.getFov());
        computeShader.SetVec3f("uCamPos", cam.getPos());
        computeShader.SetVec2i("uRes", imgSize);

        computeShader.Dispatch({imgSize.x, imgSize.y, 1});

        renderer.Clear({0.0, 0.0, 0.0});
        shader.Use();

        renderer.Draw(va, shader, 4);

        renderer.Swap();
    }
    std::cout << "End Rendering" << std::endl;

    renderer.Shutdown();
    return 0;
}
