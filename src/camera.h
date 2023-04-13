// A 6 degrees of freedom camera.
// Code based on LearnOpenGL's camera class.
// See https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

enum MovementDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    FORWARD,
    BACKWARD
};

enum RollDirection {
    ROLL_LEFT,
    ROLL_RIGHT
};

class Camera {
public:
    glm::vec3 position_;
    glm::quat rotation_;  // rotation from standard camera orientation to current orientation.
    float movement_speed_;
    float roll_speed_;
    float mouse_sensitivity_;
    float zoom_;

    Camera();
    Camera(glm::vec3 position, glm::vec3 front);

    glm::mat4 GetViewProjection(float aspect_ratio);
    virtual void Move(MovementDirection direction, float delta_time);
    virtual void ProcessKeyboardRoll(RollDirection direction, float delta_time);
    virtual void ProcessMouseMovement(float xoffset, float yoffset);
    virtual void PrintDebugInfo();

protected:
    void UpdateCameraVectors();
    
private:
    glm::vec3 up_;
    glm::vec3 front_;
    glm::vec3 right_;
};

#endif  // CAMERA_H