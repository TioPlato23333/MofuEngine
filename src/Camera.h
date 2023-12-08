#ifndef SRC_CAMERA_
#define SRC_CAMERA_

#include <glm/glm.hpp>

enum class CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    );
    Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw,
        float pitch);
    ~Camera() = default;

    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(CameraMovement direction, float delta_time);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrain_pitch = true);
    void ProcessMouseScroll(float yoffset);
    inline float Zoom() const;

private:
    void UpdateCameraVectors();

    float yaw_ = 0.0f;
    float pitch_ = 0.0f;
    float movement_speed_ = 0.0f;
    float mouse_sensitivity_ = 0.0f;
    float zoom_ = 0.0f;
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;

    static constexpr float YAW = -90.0f;
    static constexpr float PITCH = 0.0f;
    static constexpr float SPEED = 0.5f;
    static constexpr float SENSITIVITY = 0.1f;
    static constexpr float ZOOM = 45.0f;
};

float Camera::Zoom() const {
    return zoom_;
}

#endif  // SRC_CAMERA_
