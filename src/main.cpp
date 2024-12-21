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
#include <vector>
#include <stb/stb_image_write.h>

#include "Camera.hpp"
#include "ComputeShader.hpp"
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "ShaderStorage.hpp"

// Function to save OpenGL texture to a PNG file
bool saveTextureToFile(GLuint textureID, int width, int height,
                       const char* filename) {
    // Bind the texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Allocate a buffer to store the texture data
    std::vector<unsigned char> pixels(width * height *
                                      4); // Assuming 4 channels (RGBA)

    // Read the texture data into the buffer
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

    // Check for OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
        return false;
    }

    // Unbind the texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Flip the image vertically since OpenGL's origin is bottom-left
    for (int y = 0; y < height / 2; ++y) {
        int offset1 = y * width * 4;
        int offset2 = (height - y - 1) * width * 4;
        for (int x = 0; x < width * 4; ++x) {
            std::swap(pixels[offset1 + x], pixels[offset2 + x]);
        }
    }

    // Write the image to a file using stb_image_write
    if (stbi_write_png(filename, width, height, 4, pixels.data(), width * 4)) {
        std::cout << "Image saved to " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Failed to save image to file" << std::endl;
        return false;
    }
}

struct Sphere {
    glm::vec4 origin;
    glm::vec4 color;
    glm::vec4 info;
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
    shader.SetInt("uTexOld", 1);

    Vision::Camera cam({0.0f, 4.0f, -14.0f}, 45.0f, 5.0f);

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

    std::array<Sphere, 9> objects = {
        Sphere{
            {0.0f, 6.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {0.6f, 15.0f, 0.0f, 0.0f},
        },
        Sphere{
            {0.0f, -40.0f, 0.0f, 0.0f},
            {0.9f, 0.9f, 0.9f, 1.0f},
            {40.0f, 0.0f, 0.0f, 0.0f},
        },
        Sphere{
            {0.0f, 3.0f, 30.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {22.0f, 0.0f, 0.0f, 0.0f},
        },
        Sphere{
            {0.0f, 48.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {40.0f, 0.0f, 0.0f, 0.0f},
        },
        Sphere{
            {-45.0f, 3.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f, 1.0f},
            {40.0f, 0.0f, 0.0f, 0.0f},
        },
        Sphere{
            {45.0f, 3.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f, 1.0f},
            {40.0f, 0.0f, 0.0f, 0.0f},
        },
        Sphere{
            {2.0f, 1.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.4f, 0.0f},
        },
        Sphere{
            {2.0f, 2.6f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {0.6f, 0.0f, 0.6f, 0.0f},
        },
        Sphere{
            {-2.0f, 2.0f, 2.0f, 0.0f},
            {1.0f, 1.0f, 1.0f, 1.0f},
            {2.0f, 0.0f, 1.0f, 0.0f},
        },
    };

    Vision::ShaderStorage ssbo(objects.data(), objects.size() * sizeof(Sphere),
                               3);
    ssbo.Bind();

    Vision::ComputeShader computeShader("res/shaders/Basic.comp");
    Vision::Texture textureOld(imgSize.x, imgSize.y, GL_RGBA32F, 1);
    textureOld.Bind(1);
    Vision::Texture texture(imgSize.x, imgSize.y, GL_RGBA32F, 0);
    texture.Bind(0);

    std::vector<uint32_t> attachments = {textureOld.GetID()};

    Vision::Framebuffer oldFrame(attachments);

    float deltaTime = 0.0f; // time between current frame and last frame
    float lastFrame = 0.0f; // time of last frame
    int fCounter = 0;
    int totalFrames = 0;

    std::cout << "Start Rendering" << std::endl;
    while (!renderer.ShouldClose()) {
        // Set frame time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (fCounter > 200) {
            std::cout << "FPS: " << 1 / deltaTime << std::endl;
            fCounter = 0;
        } else {
            fCounter++;
        }
        totalFrames++;

        // Save frame to disk
        if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
            std::cout << "Saving..." << std::endl;
            saveTextureToFile(textureOld.GetID(), imgSize.x, imgSize.y,
                              "saved-frame.png");
            textureOld.Bind(1);
            texture.Bind(0);
        }

        // Update camera controls
        cam.Controls(renderer);

        // Generate image
        computeShader.Use();
        computeShader.SetFloat("uTime", totalFrames);
        computeShader.SetMat3f("uCamMat", cam.getMat());
        computeShader.SetFloat("uFOV", cam.getFov());
        computeShader.SetFloat("uMaxBounce", 15);
        computeShader.SetFloat("uRayPerPixel", 128);
        computeShader.SetVec3f("uCamPos", cam.getPos());
        computeShader.SetVec2i("uRes", imgSize);

        computeShader.Dispatch({imgSize.x, imgSize.y, 1});

        // Render image quad
        renderer.Clear({0.0, 0.0, 0.0});
        shader.Use();

        shader.SetFloat("uTime", totalFrames);
        shader.SetInt("uTex", 0);
        shader.SetInt("uTexOld", 1);

        renderer.Draw(va, shader, 4);

        // Copy new render to old render
        glm::vec2& winSize = renderer.GetWindowSize();
        oldFrame.CopyFromDefault(winSize);
        oldFrame.UnBind();

        renderer.Swap();
    }
    std::cout << "End Rendering" << std::endl;

    renderer.Shutdown();
    return 0;
}
