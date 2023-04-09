#ifndef HAND_ANIMATOR_H
#define HAND_ANIMATOR_H

#include "finger_animator.h"
#include <vector>

class HandAnimator{
public:
    HandAnimator(std::vector<FingerAnimator> finger_animator_list): 
        finger_animator_list_(finger_animator_list) {}

    void SetGesture(std::vector<const FingerGesture*> finger_gesture_list);

    void Update();
private:
    std::vector<FingerAnimator> finger_animator_list_;
};

#endif