#ifndef MATRICES_H
#define MATRICES_H

#include <glm/gtc/matrix_transform.hpp>

int width = 800;
int height = 600;
float aspect = (float)width/height;

float PI = 3.141592653;
float X_FOV = PI/2;

float near = 0.1f;
float far = 100.0f;

std::vector<uint32_t> frame_buffer = std::vector<uint32_t>(width*height);
std::vector<float> depth_buffer = std::vector<float>(width*height);

glm::mat4 proj = {
    {1/tan(X_FOV/2), 0, 0, 0}, 
    {0, aspect/tan(X_FOV/2), 0, 0}, 
    {0, 0, -(far+near)/(far-near), -1}, 
    {0, 0, -(2*near*far)/(far-near), 0}
};
glm::mat4 I = glm::mat4(1);
glm::vec3 X = glm::vec3 {1, 0, 0};
glm::vec3 Y = glm::vec3 {0, 1, 0};

glm::mat4 get_look_at(glm::vec3 pos, float pitch, float yaw) {
    glm::mat4 cam_translate = glm::translate(I, glm::vec3(-pos));
    glm::mat4 cam_rotate = glm::rotate(I, pitch, X) * glm::rotate(I, yaw, Y);
    return cam_rotate*cam_translate;
}

#endif
