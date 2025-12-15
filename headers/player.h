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
        dy*(2*near*tan(X_FOV/2)/height), 
        near
    };

    view_change = glm::normalize(view_change);
    float yaw_change = asin(view_change.x);
    float pitch_change = asin(view_change.y/cos(yaw_change));      

    pitch += pitch_change; // * rotation speed
    if (pitch > 90) {
        pitch = 90;
    }
    yaw += yaw_change; // * rotation speed
}

void keyboard_handler(auto ev) {
    glm::vec3 pos_change = {0, 0, 0};

    if (ev.key.keysym.sym == SDLK_w) {
        pos_change.z -= 0.1;
    }
    if (ev.key.keysym.sym == SDLK_s) {
        pos_change.z += 0.1;
    }
    if (ev.key.keysym.sym == SDLK_d) {
        pos_change.x += 0.1;
    }
    if (ev.key.keysym.sym == SDLK_a) {
        pos_change.x -= 0.1;
    }
    if (ev.key.keysym.sym == SDLK_UP) {
        pos_change.y += 0.1;
    }
    if (ev.key.keysym.sym == SDLK_DOWN) {
        pos_change.y -= 0.1;
    }

    pos += pos_change;
}

#endif