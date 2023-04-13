#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "camera.h"

class CameraPatroller: public Camera {
public:
    enum PatrollerState {
        INIT,
        DESTINATION_SET,
        PATROLLING
    };
    
    float patrol_speed_;

    CameraPatroller(glm::vec3 position, glm::vec3 front);
    void SetDestination(glm::vec3 position, glm::quat orientation);
    void TogglePatrol(float cur_time);
    void UpdatePose(float cur_time);
    PatrollerState GetState();
    // glm::mat4 GetDestinationTransfer();
    
    virtual void Move(MovementDirection direction, float delta_time);
    virtual void ProcessKeyboardRoll(RollDirection direction, float delta_time);
    virtual void ProcessMouseMovement(float xoffset, float yoffset);
    virtual void PrintDebugInfo();

private:
    PatrollerState state_;
    glm::vec3 start_position_;
    glm::quat start_orientation_;
    glm::vec3 dest_position_;
    glm::quat dest_orientation_;
    float patrol_start_time_;
    float patrol_period_;
};