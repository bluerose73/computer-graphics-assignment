// Hand Example
// Author: Yi Kangrui <yikangrui@pku.edu.cn>

//#define DIFFUSE_TEXTURE_MAPPING

#include "gl_env.h"

#include <cstdlib>
#include <cstdio>
#include <config.h>

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#include <iostream>
#include <vector>

#include "skeletal_mesh.h"

#include <glm/gtc/matrix_transform.hpp>

#include "hand_animator.h"
#include "finger_animator.h"
#include "camera_patroller.h"
#include "light_source.h"
#include "phong_shader.h"
#include "particle_generator.h"

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    
    // Camera Patrol
    CameraPatroller* camera = (CameraPatroller*) glfwGetWindowUserPointer(window);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        camera->SetDestination(camera->position_, camera->rotation_);
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        camera->TogglePatrol(glfwGetTime());
}

static void ProcessHandGestureInput(GLFWwindow* window, HandAnimator* hand_animator){
    static const StraightenGesture straighten(0.8);
    static const BendGesture bend(0.8);
    static const FingerGesture idle;
    
    if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){   // fist
        hand_animator->SetGesture({&bend, &bend, &bend, &bend, &bend});
    }
    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){   // V
        hand_animator->SetGesture({&bend, &idle, &idle, &bend, &bend});
    }
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){   // open palm
        hand_animator->SetGesture({&straighten, &straighten, &straighten,
                                   &straighten, &straighten});
    }
    if(glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){   // pistol
        hand_animator->SetGesture({&straighten, &straighten, &bend, &bend, &bend});
    }
    if(glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){   // call
        hand_animator->SetGesture({&straighten, &bend, &bend, 
                                   &bend, &straighten});
    }
}

static void ProcessCameraKeyboardInput(GLFWwindow* window, Camera* camera, float delta_time) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera->Move(UP, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera->Move(LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera->Move(DOWN, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera->Move(RIGHT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera->ProcessKeyboardRoll(ROLL_LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera->ProcessKeyboardRoll(ROLL_RIGHT, delta_time);
    }
}

static void ProcessCameraMouseButtonInput(GLFWwindow* window, Camera* camera, float delta_time) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera->Move(FORWARD, delta_time);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        camera->Move(BACKWARD, delta_time);
    }
}

static void ProcessCameraCursorPosInput(GLFWwindow* window, Camera* camera) {
    static int first_mouse = 3;
    static float last_x, last_y;
    
    double xpos_in, ypos_in;
    glfwGetCursorPos(window, &xpos_in, &ypos_in);
    float xpos = (float) xpos_in, ypos = (float) ypos_in;
    if (first_mouse > 0) {
        last_x = xpos;
        last_y = ypos;
        first_mouse--;
    }

    float xoffset = xpos - last_x;
    float yoffset = last_y - ypos;
    last_x = xpos;
    last_y = ypos;
    camera->ProcessMouseMovement(xoffset, yoffset);
}

static void ProcessLightKeyboardInput(GLFWwindow* window, LightSource* light, float delta_time) {
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        light->Move(UP, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        light->Move(LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        light->Move(DOWN, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        light->Move(RIGHT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        light->Move(FORWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        light->Move(BACKWARD, delta_time);
    }
}

int main(int argc, char *argv[]) {
    GLFWwindow *window;
    GLuint vertex_shader, fragment_shader, program;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__ // for macos
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(800, 800, "OpenGL output", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit() != GLEW_OK)
        exit(EXIT_FAILURE);

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertex_shader, 1, &SkeletalAnimation::vertex_shader_330, NULL);
    glShaderSource(vertex_shader, 1, &phong_shading::vertex_shader_330, NULL);
    glCompileShader(vertex_shader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragment_shader, 1, &SkeletalAnimation::fragment_shader_330, NULL);
    glShaderSource(fragment_shader, 1, &phong_shading::fragment_shader_330, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment_shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int linkStatus;
    if (glGetProgramiv(program, GL_LINK_STATUS, &linkStatus), linkStatus == GL_FALSE)
        std::cout << "Error occured in glLinkProgram()" << std::endl;

    SkeletalMesh::Scene &sr = SkeletalMesh::Scene::loadScene("Hand", DATA_DIR"/Hand.fbx");
    if (&sr == &SkeletalMesh::Scene::error)
        std::cout << "Error occured in loadMesh()" << std::endl;

    sr.setShaderInput(program, "in_position", "in_texcoord", "in_normal", "in_bone_index", "in_bone_weight");

    float passed_time;
    SkeletalMesh::SkeletonModifier modifier;

    const FingerGesture idle;
    HandAnimator hand_animator({
        FingerAnimator(modifier["thumb_proximal_phalange"],
                       modifier["thumb_intermediate_phalange"],
                       modifier["thumb_distal_phalange"],
                       &idle
                       ),
        FingerAnimator(modifier["index_proximal_phalange"],
                       modifier["index_intermediate_phalange"],
                       modifier["index_distal_phalange"],
                       &idle),
        FingerAnimator(modifier["middle_proximal_phalange"],
                       modifier["middle_intermediate_phalange"],
                       modifier["middle_distal_phalange"],
                       &idle),
        FingerAnimator(modifier["ring_proximal_phalange"],
                       modifier["ring_intermediate_phalange"],
                       modifier["ring_distal_phalange"],
                       &idle),
        FingerAnimator(modifier["pinky_proximal_phalange"],
                       modifier["pinky_intermediate_phalange"],
                       modifier["pinky_distal_phalange"],
                       &idle)
    });

    CameraPatroller camera({0.0f, 0.0f, -40.0f}, {0.0f, 0.0f, 1.0f});
    camera.SetDestination({1.46, 43.47, -12.19}, {0.64, -0.77, 0.04, -0.00});
    glfwSetWindowUserPointer(window, &camera);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Let particles follow the source.
    LightSource light_source({0.0f, 0.0f, -35.0f}, {1.0f, 1.0f, 1.0f});
    ParticleGenerator particles({0.0f, 0.0f, -35.0f}, {0.0f, 10.0f, 0.0f},
                                {1.0f, 0.5f, 0.0f, 0.2f}, 0.5, 1.0, 5.0, 10000.0, 11000);

    glEnable(GL_DEPTH_TEST);
    int counter  = 0;
    float delta_time = 0;
    passed_time = (float) glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        counter ++;
        float cur_time = (float) glfwGetTime();
        delta_time = cur_time - passed_time;
        passed_time = cur_time;

        float metacarpals_angle = passed_time * (M_PI / 4.0f);

        ProcessHandGestureInput(window, &hand_animator);
        ProcessCameraKeyboardInput(window, &camera, delta_time);
        ProcessCameraMouseButtonInput(window, &camera, delta_time);
        ProcessCameraCursorPosInput(window, &camera);
        ProcessLightKeyboardInput(window, &light_source, delta_time);

        hand_animator.Update();
        camera.UpdatePose(passed_time);
        particles.position_ = light_source.position_;
        particles.Update(delta_time);

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glClearColor(0.5, 0.5, 0.5, 1.0);

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glm::fmat4 view_projection = camera.GetViewProjection(ratio);
        if (counter % 10000 == 0) {
            particles.PrintDebugInfo();
            // camera.PrintDebugInfo();
            // printf("lightPos = %f, %f, %f\n", light_source.position_.x, light_source.position_.y, light_source.position_.z);
            // printf("lightColor = %f, %f, %f\n", light_source.light_color_.x, light_source.light_color_.y, light_source.light_color_.z);
        }
        glUniformMatrix4fv(glGetUniformLocation(program, "u_view_projection"), 1, GL_FALSE, (const GLfloat *) &view_projection);
        glUniform1i(glGetUniformLocation(program, "u_diffuse"), SCENE_RESOURCE_SHADER_DIFFUSE_CHANNEL);
        glUniform3f(glGetUniformLocation(program, "lightPos"),
                    light_source.position_.x, light_source.position_.y, light_source.position_.z);
        glUniform3f(glGetUniformLocation(program, "lightColor"),
                    light_source.light_color_.x, light_source.light_color_.y, light_source.light_color_.z);
        glUniform3f(glGetUniformLocation(program, "viewPos"), camera.position_.x, camera.position_.y, camera.position_.z);

        SkeletalMesh::Scene::SkeletonTransf bonesTransf;
        sr.getSkeletonTransform(bonesTransf, modifier);
        if (!bonesTransf.empty())
            glUniformMatrix4fv(glGetUniformLocation(program, "u_bone_transf"), bonesTransf.size(), GL_FALSE,
                               (float *) bonesTransf.data());
        sr.render();
        particles.Draw(view_projection, camera.position_);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    SkeletalMesh::Scene::unloadScene("Hand");

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}