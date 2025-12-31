#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <glfw/glfw3.h>

#include "globals.h"

glm::vec3 pos = {0, 0, 2}; // start at 2 so at to not be inside suzanne
float pitch = 0;
float yaw = 0;

float nanotoseconds = 1e-9;
float VEL_MAG = 7.5f;

void mouse_handler(double dx, double dy) {
    glm::vec3 view_change = {
        dx*(2*near*tan(X_FOV/2)/width), 
        dy*(2*near*tan(X_FOV/2)/height), 
        near
    };

    view_change = glm::normalize(view_change);
    float yaw_change = asin(view_change.x);
    float pitch_change = asin(view_change.y/cos(yaw_change));      

    pitch += pitch_change;
    yaw += yaw_change;
}

void keyboard_handler(GLFWwindow* window, float delta_time) {
    glm::vec4 vel = {0, 0, 0, 0};

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vel.z -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vel.z += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        vel.x += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        vel.x -= 1;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        vel.y += 1;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        vel.y -= 1;
    }

    vel = glm::rotate(I, -yaw, Y)*glm::rotate(I, -pitch, X)*vel;

    int num_comps = (vel.x != 0) + (vel.y != 0) + (vel.z != 0);
    if (num_comps != 0) {
        // add optimized displacement to position
        // note that sqrt(num_comps) = |vel|
        pos += glm::vec3((float)((delta_time*nanotoseconds)*VEL_MAG/sqrt(num_comps))*vel);
    }
}

#endif