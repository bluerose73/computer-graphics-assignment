#include "hand_animator.h"
#include "GLFW/glfw3.h"
#include <algorithm>

void HandAnimator::SetGesture(std::vector<const FingerGesture *> finger_gesture_list){
    int sz = std::min(finger_gesture_list.size(), finger_animator_list_.size());
    for (int i = 0; i < sz; i++){
        finger_animator_list_[i].SetGesture(finger_gesture_list[i]);
    }
}

void HandAnimator::Update(){
    float cur_time = glfwGetTime();
    for (FingerAnimator &finger : finger_animator_list_){
        finger.Update(cur_time);
    }
}