#include "camera.h"
#include <cmath>
#include <cstdio>

static const float MOVEMENT_SPEED = 10.0f;
static const float ROLL_SPEED = 1.0f;
static const float SENSITIVITY = 0.005F;
static const float ZOOM = 45.0f;

// "standard" camera position, sitting at origin, looking at -z.
static const glm::vec3 ORIGIN(0.0f, 0.0f, 0.0f);
static const glm::vec3 STD_FRONT(0.0f, 0.0f, -1.0f);
static const glm::vec3 STD_UP(0.0f, 1.0f, 0.0f);
static const glm::vec3 STD_RIGHT(1.0f, 0.0f, 0.0f);

Camera::Camera(): Camera(ORIGIN, STD_FRONT) {}

Camera::Camera(glm::vec3 position, glm::vec3 front)
        : position_(position), movement_speed_(MOVEMENT_SPEED),
          roll_speed_(ROLL_SPEED), mouse_sensitivity_(SENSITIVITY),
          zoom_(ZOOM) {
    front_ = glm::normalize(front);
    // The initial up vector is in the plane defined by front and +y.
    right_ = glm::normalize(glm::cross(front_, STD_UP));
    up_ = glm::normalize(glm::cross(right_, front_));
    // lookAt calculates the camera -> origin transform.
    // We want origin -> camera transform so need an inverse.
    glm::mat4 view = glm::lookAt(ORIGIN, front_, up_);
    rotation_ = glm::quat(glm::inverse(view));
}

glm::mat4 Camera::GetViewProjection(float aspect_ratio){
    glm::mat4 view = glm::lookAt(position_, position_ + front_, up_);
    glm::mat4 projection = glm::perspective(glm::radians(zoom_), aspect_ratio, 0.1f, 100.0f);
    // glm::mat4 projection = glm::ortho(-12.5f * aspect_ratio, 12.5f * aspect_ratio, -5.f, 20.f, -20.f, 20.f);
    return projection * view;
}

void Camera::Move(MovementDirection direction, float delta_time){
    float velocity = movement_speed_ * delta_time;
    switch (direction) {
        case UP:
            position_ += up_ * velocity;
            break;
        case DOWN:
            position_ -= up_ * velocity;
            break;
        case LEFT:
            position_ -= right_ * velocity;
            break;
        case RIGHT:
            position_ += right_ * velocity;
            break;
        case FORWARD:
            position_ += front_ * velocity;
            break;
        case BACKWARD:
            position_ -= front_ * velocity;
            break;
    }
    UpdateCameraVectors();
}

void Camera::ProcessKeyboardRoll(RollDirection direction, float delta_time) {
    float rad = roll_speed_ * delta_time;
    switch (direction) {
        case ROLL_LEFT:
            rotation_ = glm::angleAxis(-rad, front_) * rotation_;
            break;
        case ROLL_RIGHT:
            rotation_ = glm::angleAxis(rad, front_) * rotation_;
            break;
    }
    UpdateCameraVectors();
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    if (fabs(xoffset) < 1e-6 && fabs(yoffset) < 1e-6) return;
    // printf("[mouse movement] xoffset = %.2f, yoffset = %.2f\n", xoffset, yoffset);
    float dist = sqrtf(xoffset * xoffset + yoffset * yoffset);
    float rad = dist * mouse_sensitivity_;
    glm::vec3 mouse_move_direction = glm::normalize(glm::vec3(xoffset, yoffset, 0.0f));
    glm::vec3 axis = glm::normalize(glm::cross(STD_FRONT, mouse_move_direction));
    axis = rotation_ * axis;
    rotation_ = glm::angleAxis(rad, axis) * rotation_;
    UpdateCameraVectors();
}

void Camera::PrintDebugInfo() {
    printf("[camera] position = (%.2f, %.2f, %.2f), front = (%.2f, %.2f, %.2f), up = (%.2f, %.2f, %.2f)\n",
           position_.x, position_.y, position_.z, front_.x, front_.y, front_.z, up_.x, up_.y, up_.z);
}

void Camera::UpdateCameraVectors() {
    front_ = rotation_ * STD_FRONT;
    right_ = rotation_ * STD_RIGHT;
    up_ = rotation_ * STD_UP;
}
