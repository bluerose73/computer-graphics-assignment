#ifndef HAND_LIGHT_SOURCE_H
#define HAND_LIGHT_SOURCE_H

#include <glm/glm.hpp>
#include "movable_object.h"

class LightSource: public MovableObject {
public:
    glm::vec3 light_color_;

    LightSource(glm::vec3 position, glm::vec3 light_color);
};

#endif  // HAND_LIGHT_SOURCE_H