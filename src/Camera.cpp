#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED), mouse_sensitivity_(SENSITIVITY),
    zoom_(ZOOM) {
    position_ = position;
    world_up_ = up;
    yaw_ = yaw;
    pitch_ = pitch;
    UpdateCameraVectors();
}

Camera::Camera(float pos_x, float pos_y, float pos_z, float up_x, float up_y, float up_z, float yaw,
    float pitch) : front_(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed_(SPEED),
                   mouse_sensitivity_(SENSITIVITY), zoom_(ZOOM) {
    position_ = glm::vec3(pos_x, pos_y, pos_z);
    world_up_ = glm::vec3(up_x, up_y, up_z);
    yaw_ = yaw;
    pitch_ = pitch;
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() {
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::GetModelMatrix() {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::rotate(model, glm::radians(model_yaw_), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-model_pitch_), glm::vec3(0.0f, 0.0f, 1.0f));
    return model;
}

void Camera::ProcessKeyboard(CameraMovement direction, float delta_time) {
    float velocity = movement_speed_ * delta_time;
    if (direction == CameraMovement::FORWARD) {
        position_ += front_ * velocity;
    }
    if (direction == CameraMovement::BACKWARD) {
        position_ -= front_ * velocity;
    }
    if (direction == CameraMovement::LEFT) {
        position_ -= right_ * velocity;
    }
    if (direction == CameraMovement::RIGHT) {
        position_ += right_ * velocity;
    }
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrain_pitch) {
    xoffset *= mouse_sensitivity_;
    yoffset *= mouse_sensitivity_;

    /*
    // TODO: affect model yaw/pitch instead of camera
    yaw_ += xoffset;
    pitch_ += yoffset;

    if (constrain_pitch) {
        if (pitch_ > 89.0f) {
            pitch_ = 89.0f;
        }
        if (pitch_ < -89.0f) {
            pitch_ = -89.0f;
        }
    }

    UpdateCameraVectors();
    */
    model_yaw_ += xoffset;
    model_pitch_ += yoffset;

    if (constrain_pitch) {
        if (model_pitch_ > 89.0f) {
            model_pitch_ = 89.0f;
        }
        if (model_pitch_ < -89.0f) {
            model_pitch_ = -89.0f;
        }
    }
}

void Camera::ProcessMouseScroll(float yoffset) {
    zoom_ -= yoffset * 0.01;
    if (zoom_ < 1.0f) {
        zoom_ = 1.0f;
    }
    if (zoom_ > 45) {
        zoom_ = 45.0f;
    }
}

void Camera::UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
