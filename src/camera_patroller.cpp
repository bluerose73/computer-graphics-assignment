#include "camera_patroller.h"
#include <cstdio>

static const float PATROL_SPEED = 5.0f;

CameraPatroller::CameraPatroller(glm::vec3 position, glm::vec3 front)
        : Camera(position, front), patrol_speed_(PATROL_SPEED), state_(INIT) {}

void CameraPatroller::SetDestination(glm::vec3 position, glm::quat orientation) {
    switch (state_) {
    case INIT:
    case DESTINATION_SET:
        printf("[patrol] destination set\n");
        dest_position_ = position;
        dest_orientation_ = orientation;
        state_ = DESTINATION_SET;
        break;
    case PATROLLING:
        break;  // Do nothing.
    }
}

void CameraPatroller::TogglePatrol(float cur_time) {
    switch (state_) {
        case INIT:
            break;
        case DESTINATION_SET: {
            printf("[patrol] start patrolling\n");
            state_ = PATROLLING;
            start_position_ = position_;
            start_orientation_ = rotation_;
            patrol_start_time_ = cur_time;
            float patrol_dist = (dest_position_ - position_).length() * 2;
            patrol_period_ = patrol_dist / patrol_speed_;
            break;
        }
        case PATROLLING:
            printf("[patrol] stop patrolling\n");
            state_ = DESTINATION_SET;
            break;
    }
}

void CameraPatroller::UpdatePose(float cur_time) {
    if (state_ == PATROLLING) {
        float time_in_period = fmod(cur_time - patrol_start_time_, patrol_period_);
        float place = 1.0f - abs(time_in_period / (patrol_period_ * 0.5f) - 1.0f);
        position_ = glm::mix(start_position_, dest_position_, place);
        rotation_ = glm::mix(start_orientation_, dest_orientation_, place);
        UpdateCameraVectors();
    }
}

CameraPatroller::PatrollerState CameraPatroller::GetState() {
    return state_;
}

// glm::mat4 CameraPatroller::GetDestinationTransfer() {
//     if (state_ == INIT) return glm::identity<glm::mat4>();
//     return glm::translate(glm::identity<glm::mat4>(), position_) * glm::mat4(dest_orientation_);
// }

void CameraPatroller::Move(MovementDirection direction, float delta_time) {
    if (state_ == PATROLLING) return;
    Camera::Move(direction, delta_time);
}

void CameraPatroller::ProcessKeyboardRoll(RollDirection direction, float delta_time) {
    if (state_ == PATROLLING) return;
    Camera::ProcessKeyboardRoll(direction, delta_time);
}

void CameraPatroller::ProcessMouseMovement(float xoffset, float yoffset) {
    if (state_ == PATROLLING) return;
    Camera::ProcessMouseMovement(xoffset, yoffset);
}

void CameraPatroller::PrintDebugInfo() {
    Camera::PrintDebugInfo();
    printf("[patrol] start orientation = (%.2f, %.2f, %.2f, %.2f), dest orientation = (%.2f, %.2f, %.2f, %.2f)\n",
           start_orientation_.x, start_orientation_.y, start_orientation_.z, start_orientation_.w,
           dest_orientation_.x, dest_orientation_.y, dest_orientation_.z, dest_orientation_.w);
}
