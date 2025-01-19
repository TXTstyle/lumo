#include "Config.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdlib>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <stb/stb_image_write.h>

#include "Camera.hpp"
#include "ComputeShader.hpp"
#include "Framebuffer.hpp"
#include "Loader.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include "VertexBufferLayout.hpp"
#include "Texture.hpp"
#include "ShaderStorage.hpp"
#include "Triangle.hpp"

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

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "Lumo usage:\n\tLumo <path to scene file>\n\t Ex; Lumo ./scene.toml" << std::endl;
        exit(1);
    }
    std::string configPath = argv[1];

    Vision::Renderer renderer;
    try {
        renderer.Init({1280, 720}, std::string("Lumo - ").append(configPath));
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        renderer.Shutdown();
        return EXIT_FAILURE;
    }

    Vision::Shader shader("res/shaders/Basic.vert", "res/shaders/Basic.frag");
    shader.Use();
    shader.SetInt("uTex", 0);
    shader.SetInt("uTexOld", 1);

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

    Config config("scene.toml");
    std::cout << "Setup completed" << std::endl;
    glm::vec2& imgSize = config.GetRes();

    Vision::ShaderStorage ssbo(
        config.GetLoader().GetTrigs().data(),
        config.GetLoader().GetTrigs().size() * sizeof(Triangle), 1);
    ssbo.Bind();
    Vision::ShaderStorage meshesBuffer(
        config.GetLoader().GetMeshes().data(),
        config.GetLoader().GetMeshes().size() * sizeof(MeshInfo), 2);
    meshesBuffer.Bind();

    Vision::ComputeShader computeShader("res/shaders/Basic.comp");
    Vision::Texture textureOld(imgSize.x, imgSize.y, GL_RGBA32F, 1);
    textureOld.Bind(1);
    Vision::Texture texture(imgSize.x, imgSize.y, GL_RGBA32F, 0);
    texture.Bind(0);

    std::vector<uint64_t> handles;
    std::vector<MatData> mats;
    for (auto& mat : config.GetMaterials()) {
        mat.PushHandles(handles);
        mats.push_back(mat.GetData());
    }

    Vision::ShaderStorage matsBuffer(mats.data(), mats.size() * sizeof(MatData),
                                     3);
    matsBuffer.Bind();

    Vision::ShaderStorage textureHandles(handles.data(),
                                         handles.size() * sizeof(uint64_t), 4);
    textureHandles.Bind();

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
        if (fCounter > 10) {
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
                              config.GetOutputPath().c_str());
            textureOld.Bind(1);
            texture.Bind(0);
        }

        // Update camera controls
        config.GetCamera().Controls(renderer);

        // Generate image
        computeShader.Use();
        computeShader.SetFloat("uTime", totalFrames);
        computeShader.SetMat3f("uCamMat", config.GetCamera().getMat());
        computeShader.SetFloat("uFOV", config.GetCamera().getFov());
        computeShader.SetFloat("uMaxBounce", config.GetBounces());
        computeShader.SetFloat("uRayPerPixel", config.GetRaysPerPixel());
        computeShader.SetVec3f("uCamPos", config.GetCamera().getPos());
        computeShader.SetVec2i("uRes", imgSize);
        config.GetEnv().Bind(31);

        computeShader.Dispatch({imgSize.x / 16, imgSize.y / 16, 1});

        // Render image quad
        renderer.Clear({0.0, 0.0, 0.0});
        shader.Use();
        texture.Bind();
        textureOld.Bind();

        shader.SetFloat("uTime", totalFrames);
        shader.SetInt("uTex", 0);
        shader.SetInt("uTexOld", 1);
        shader.SetFloat("uGamma", config.GetGamma());

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
