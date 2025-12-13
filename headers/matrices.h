#ifndef MATRICES_H
#define MATRICES_H

#include <glm/gtc/matrix_transform.hpp>

#include "drawing.h"

float PI = 3.141592653;
float aspect;
float X_FOV = PI/2;
float near = 0.1f;
float far = 100.0f;

glm::mat4 I = glm::mat4(1);
glm::vec3 X = glm::vec3 {1, 0, 0};
glm::vec3 Y = glm::vec3 {0, 1, 0};
glm::vec3 Z = glm::vec3 {0, 0, 1}; // May be unused

glm::mat4 proj;

glm::mat4 get_look_at(glm::vec3 pos, float pitch, float yaw) {
    glm::mat4 cam_translate = glm::translate(I, glm::vec3(-pos));
    glm::mat4 cam_rotate = glm::rotate(I, pitch, X) * glm::rotate(I, yaw, Y);
    glm::mat4 look_at = cam_rotate*cam_translate;
    return look_at;
}

#endif
