#ifndef FINGER_ANIMATOR_H
#define FINGER_ANIMATOR_H

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

class FingerGesture;

class FingerAnimator{
public:
    FingerAnimator(
        glm::fmat4 &proximal_translation,
        glm::fmat4 &intermediate_translation,
        glm::fmat4 &distal_translation,
        const FingerGesture *gesture
    ):proximal_translation_(proximal_translation),
      intermediate_translation_(intermediate_translation),
      distal_translation_(distal_translation),
      gesture_(gesture), begin_time_(0.0) {}

    void SetGesture(const FingerGesture *gesture){
        gesture_ = gesture;
        begin_time_ = glfwGetTime();
    }

    void Update(double cur_time);
    
private:
    glm::fmat4 &proximal_translation_;
    glm::fmat4 &intermediate_translation_;
    glm::fmat4 &distal_translation_;

    const FingerGesture *gesture_;
    double begin_time_;
};

class FingerGesture{
public:
    virtual void Update(
        double begin_time, double cur_time,
        glm::fmat4 &proximal_translation,
        glm::fmat4 &intermediate_translation,
        glm::fmat4 &distal_translation
    ) const;
};

class StraightenGesture: public FingerGesture{
public:
    StraightenGesture(double speed): speed_(speed) {}
    virtual void Update(
        double begin_time, double cur_time,
        glm::fmat4 &proximal_translation,
        glm::fmat4 &intermediate_translation,
        glm::fmat4 &distal_translation
    ) const;
private:
    double speed_;    // rad per second
};

class BendGesture: public FingerGesture{
public:
    BendGesture(double speed): speed_(speed) {}
    virtual void Update(
        double begin_time, double cur_time,
        glm::fmat4 &proximal_translation,
        glm::fmat4 &intermediate_translation,
        glm::fmat4 &distal_translation
    ) const;
private:
    double speed_;    // rad per second
};

#endif  // FINGER_ANIMATOR_H