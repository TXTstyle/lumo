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

    Vision::Camera cam({0.0f, 3.0f, -10.0f}, 45.0f, 5.0f);

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

    Loader loader("res/models/scene-2.obj");

    Vision::ShaderStorage ssbo(loader.GetTrigs().data(),
                               loader.GetTrigs().size() * sizeof(Triangle), 1);
    ssbo.Bind();
    Vision::ShaderStorage meshesBuffer(
        loader.GetMeshes().data(), loader.GetMeshes().size() * sizeof(MeshInfo),
        2);
    meshesBuffer.Bind();

    Vision::ComputeShader computeShader("res/shaders/Basic.comp");
    Vision::Texture textureOld(imgSize.x, imgSize.y, GL_RGBA32F, 1);
    textureOld.Bind(1);
    Vision::Texture texture(imgSize.x, imgSize.y, GL_RGBA32F, 0);
    texture.Bind(0);

    Material tiles(0, "res/textures/long_white_tiles_diff_2k.png",
                      "res/textures/long_white_tiles_rough_2k.png",
                      "res/textures/long_white_tiles_nor_gl_2k.png", //
                      GL_RGB8, GL_R8, GL_RGB8);
    Material plaster(1, "res/textures/painted_plaster_wall_diff_2k.png",
                      "res/textures/painted_plaster_wall_rough_2k.png",
                      "res/textures/painted_plaster_wall_nor_gl_2k.png", //
                      GL_RGB8, GL_R8, GL_RGB8);
    Material concrete(2, "res/textures/patterned_concrete_pavers_diff_2k.png",
                      "res/textures/patterned_concrete_pavers_rough_2k.png",
                      "res/textures/patterned_concrete_pavers_nor_gl_2k.png", //
                      GL_RGB8, GL_R8, GL_RGB8);
    Material planks(3, "res/textures/wood_planks_diff_2k.png",
                    "res/textures/wood_planks_rough_2k.png",
                    "res/textures/wood_planks_nor_gl_2k.png", //
                    GL_RGB8, GL_R8, GL_RGB8);

    std::array<MatData, 4> mats = {
        tiles.GetData(),
        plaster.GetData(),
        concrete.GetData(),
        planks.GetData(),
    };

    Vision::ShaderStorage matsBuffer(mats.data(), mats.size() * sizeof(MatData),
                                     3);
    matsBuffer.Bind();

    std::vector<uint64_t> handles;
    handles.reserve(12);
    plaster.PushHandles(handles);
    tiles.PushHandles(handles);
    concrete.PushHandles(handles);
    planks.PushHandles(handles);

    Vision::ShaderStorage textureHandles(
        handles.data(), handles.size() * sizeof(uint64_t),
        4);
    textureHandles.Bind();

    Vision::Texture envTex("res/textures/klippad_dawn_2_2k.exr", GL_RGBA32F);

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
        computeShader.SetFloat("uMaxBounce", 10);
        computeShader.SetFloat("uRayPerPixel", 75);
        computeShader.SetVec3f("uCamPos", cam.getPos());
        computeShader.SetVec2i("uRes", imgSize);
        envTex.Bind(31);

        computeShader.Dispatch({imgSize.x / 16, imgSize.y / 16, 1});

        // Render image quad
        renderer.Clear({0.0, 0.0, 0.0});
        shader.Use();
        texture.Bind();
        textureOld.Bind();

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
