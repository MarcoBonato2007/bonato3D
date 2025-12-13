#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>
#include <glfw/glfw3.h>

#include "matrices.h"

glm::vec3 pos = {0, 0, 0};
float pitch = 0;
float yaw = 0;

void mouse_handler(double dx, double dy) {
    glm::vec3 view_change = {
        dx*(2*near*tan(X_FOV/2)/width), 
        -dy*(2*near*tan(X_FOV/2)/height), 
        near
    };

    view_change = glm::normalize(view_change);
    float yaw_change = asin(-view_change.x);
    float pitch_change = asin(view_change.y/cos(yaw_change));      

    pitch += pitch_change; // * rotation speed
    if (pitch > 90) {
        pitch = 90;
    }
    yaw += yaw_change; // * rotation speed
}

void keyboard_handler(GLFWwindow* window) {
    glm::vec3 pos_change = {0, 0, 0};

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        pos_change.z += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        pos_change.z -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        pos_change.x += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        pos_change.x -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        pos_change.y += 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        pos_change.y -= 0.1;
    }

    pos += pos_change;
}

#endif