#include "Camera.hpp"
#include <glm/ext/matrix_transform.hpp>

using namespace Vision;

Camera::Camera() : camSpeed(0), camFov(45.0f), camPos({0.0, 0.0, 0.0}) {}

Camera::Camera(glm::vec3 p_camPos, float p_fov, float p_camSpeed)
    : camSpeed(p_camSpeed), camFov(p_fov), camPos(p_camPos) {}

Camera::Camera(Camera&& other) noexcept
    : camSpeed(std::move(other.camSpeed)), camFov(std::move(other.camFov)),
      yaw(std::move(other.yaw)), pitch(std::move(other.pitch)),
      lastX(std::move(other.lastX)), lastY(std::move(other.lastY)),
      firstMouse(std::move(other.firstMouse)),
      sensitivity(std::move(other.sensitivity)),
      camPos(std::move(other.camPos)), camFront(std::move(other.camFront)),
      camUp(std::move(other.camUp)), deltaTime(std::move(other.deltaTime)),
      lastFrame(std::move(other.lastFrame)), camMat(std::move(other.camMat)),
      cursorOn(std::move(other.cursorOn)),
      cursorToggle(std::move(other.cursorToggle)) {
    other.yaw = -90.0f;
    other.pitch = 0.0f;
    other.firstMouse = true;
}

Camera& Camera::operator=(Camera&& other) noexcept {
        if (this != &other) {  // Self-assignment check
            camSpeed = std::move(other.camSpeed);
            camFov = std::move(other.camFov);
            yaw = std::move(other.yaw);
            pitch = std::move(other.pitch);
            lastX = std::move(other.lastX);
            lastY = std::move(other.lastY);
            firstMouse = std::move(other.firstMouse);
            sensitivity = std::move(other.sensitivity);
            camPos = std::move(other.camPos);
            camFront = std::move(other.camFront);
            camUp = std::move(other.camUp);
            deltaTime = std::move(other.deltaTime);
            lastFrame = std::move(other.lastFrame);
            camMat = std::move(other.camMat);
            cursorOn = std::move(other.cursorOn);
            cursorToggle = std::move(other.cursorToggle);

            // Reset the moved-from state of 'other'
            other.yaw = -90.0f;
            other.pitch = 0.0f;
            other.firstMouse = true;
        }
        return *this;
    }

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
