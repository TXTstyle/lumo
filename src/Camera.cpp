#include "Camera.hpp"
#include <glm/ext/matrix_transform.hpp>

using namespace Vision;

Camera::Camera(glm::vec3 p_camPos, float p_fov, float p_camSpeed)
    : camSpeed(p_camSpeed), camFov(p_fov), camPos(p_camPos) {}

Camera::~Camera() {}

void Camera::Controls(Renderer& renderer) {
    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(renderer.GetWindow(), GLFW_TRUE);
        return;
    }

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float speed = deltaTime * camSpeed;
    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed *= 10.0f;
    } else if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_LEFT_CONTROL) ==
               GLFW_PRESS) {
        speed *= 0.5f;
    } else {
        speed = deltaTime * camSpeed;
    }

    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        camPos -= speed * camFront;
    }
    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        camPos += speed * camFront;
    }
    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        camPos -= -glm::normalize(glm::cross(camFront, camUp)) * speed;
    }
    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        camPos += -glm::normalize(glm::cross(camFront, camUp)) * speed;
    }

    glm::vec3 camF = glm::normalize(glm::vec3((camPos + camFront) - camPos));
    glm::vec3 camR = glm::normalize(glm::cross(glm::vec3(0, 1, 0), camF));
    glm::vec3 camU = glm::cross(camF, camR);
    camMat = glm::mat3(camR, camU, camF);
}
