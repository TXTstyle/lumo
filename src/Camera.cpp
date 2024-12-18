#include "Camera.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <iostream>

using namespace Vision;

Camera::Camera(glm::vec3 p_camPos, float p_fov, float p_camSpeed)
    : camSpeed(p_camSpeed), camFov(p_fov), camPos(p_camPos) {}

Camera::~Camera() {}

float scroll = 1;
double mouseX, mouseY;

void Camera::Controls(Renderer& renderer) {
    float mouseX = 0;
    float mouseY = 0;

    if (firstMouse) // initially set to true
    {
        lastX = mouseX;
        lastY = mouseX;
        firstMouse = false;
    }

    mouseX = lastX;
    mouseY = lastY;
    if (!cursorOn) {
        mouseX = renderer.GetMousePos().x;
        mouseY = renderer.GetMousePos().y;
    }

    float xoffset = mouseX - lastX;
    // float yoffset = lastY - mouseY; // reversed since y-coordinates range from bottom to top
    float yoffset = mouseY - lastY;
    lastX = mouseX;
    lastY = mouseY;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(direction);

    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(renderer.GetWindow(), GLFW_TRUE);
        return;
    }

    if (glfwGetKey(renderer.GetWindow(), GLFW_KEY_LEFT_ALT) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    bool cursorToggleCurrent =
        glfwGetKey(renderer.GetWindow(), GLFW_KEY_I) == GLFW_PRESS;
    if (cursorToggleCurrent && !cursorToggle) {
        cursorOn = !cursorOn;
        int cursorState = cursorOn ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED;
        glfwSetInputMode(renderer.GetWindow(), GLFW_CURSOR, cursorState);
        glfwSetCursorPos(renderer.GetWindow(), 0, 0);
        mouseX = 0;
        mouseY = 0;
    }
    cursorToggle = cursorToggleCurrent;

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

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0) {
        scroll = 1.025;
    } else if (yoffset < 0) {
        scroll = 0.975f;
    }
}
