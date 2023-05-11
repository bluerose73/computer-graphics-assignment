// A 6 degrees of freedom camera.
// Code based on LearnOpenGL's camera class.
// See https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h

#ifndef CAMERA_H
#define CAMERA_H

#include <vector>
#include "movable_object.h"

class Camera: public MovableObject {
public:
    float zoom_;

    Camera();
    Camera(glm::vec3 position, glm::vec3 front);

    glm::mat4 GetViewProjection(float aspect_ratio);
};

#endif  // CAMERA_H