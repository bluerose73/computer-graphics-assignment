#include "finger_animator.h"
#include <algorithm>
#include "glm/gtc/matrix_transform.hpp"

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

void FingerAnimator::Update(double cur_time){
    if (gesture_) gesture_->Update(
        begin_time_, cur_time,
        proximal_translation_,
        intermediate_translation_,
        distal_translation_
    );
}

void FingerGesture::Update(
    double begin_time, double cur_time,
    glm::fmat4 &proximal_translation,
    glm::fmat4 &intermediate_translation,
    glm::fmat4 &distal_translation
) const{
    auto translation = glm::identity<glm::mat4>();
    proximal_translation = intermediate_translation = distal_translation = translation;
}

void StraightenGesture::Update(
    double begin_time, double cur_time,
    glm::fmat4 &proximal_translation,
    glm::fmat4 &intermediate_translation,
    glm::fmat4 &distal_translation
) const{
    double delta_time = cur_time - begin_time;
    double angle = std::max(M_PI / 3 - delta_time * speed_, 0.0);
    auto translation = glm::rotate(
        glm::identity<glm::mat4>(), (float)angle, glm::fvec3(0.0, 0.0, 1.0)
    );
    proximal_translation = intermediate_translation = distal_translation = translation;
}

void BendGesture::Update(
    double begin_time, double cur_time,
    glm::fmat4 &proximal_translation,
    glm::fmat4 &intermediate_translation,
    glm::fmat4 &distal_translation
) const{
    double delta_time = cur_time - begin_time;
    double angle = std::min(delta_time * speed_, M_PI / 3);
    auto translation = glm::rotate(
        glm::identity<glm::mat4>(), (float)angle, glm::fvec3(0.0, 0.0, 1.0)
    );
    proximal_translation = intermediate_translation = distal_translation = translation;
}