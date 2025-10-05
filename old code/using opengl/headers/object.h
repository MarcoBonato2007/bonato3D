#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>

struct Object {
    glm::vec4 pos = {0, 0, 0, 1};
    glm::vec4 vel = {0, 0, 0, 0};
    glm::vec4 accel = {0, 0, 0, 0};
    float mass = 0;

    Object() {}

    Object(glm::vec4 pos_, glm::vec4 vel_, glm::vec4 accel_, float mass_) {
        pos = pos_;
        vel = vel_;
        accel = accel_;
        mass = mass_;
    }

    void step(float time_step, glm::vec4 new_accel) {
        // Verlet integration

        // Drag formula: 1/2*air density*area*drag coefficient * velocity squared
        new_accel -= 0.5f*1.225f*1*1*glm::length(vel)*vel/mass;

        pos += time_step * (vel + 0.5f*time_step*accel);
        vel += 0.5f*time_step*(accel + new_accel);
    }
};

// This will store things like a camera object and the model

#endif