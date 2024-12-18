#pragma once
#include "Renderer.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Vision {
class Camera {
  private:
    float camSpeed;
    float camFov;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float lastX = 0.0f, lastY = 0.0f;
    bool firstMouse = true;
    float sensitivity = 0.075f;

    glm::vec3 camPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float deltaTime;
    float lastFrame;

    glm::mat3 camMat;

    bool cursorOn = false;
    bool cursorToggle = false;

  public:
    Camera(glm::vec3 p_camPos, float p_fov, float p_camSpeed);
    ~Camera();

    inline glm::vec3& getPos() { return camPos; }
    inline glm::mat3& getMat() { return camMat; }
    inline float& getFov() { return camFov; }

    void Controls(Renderer& renderer);
};
} // namespace Vision
