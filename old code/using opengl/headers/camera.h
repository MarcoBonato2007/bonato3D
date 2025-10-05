#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "constants.h"

struct Camera {
    float tan_half_x_fov;
    float tan_half_y_fov;
    float near_dist;
    float far_dist;
    glm::mat4 projection;
    float pitch = 0;
    float yaw = 0;
    glm::vec4 pos = {0, 0, 0, 1};

    Camera() {}

    Camera(float x_fov, float aspect_ratio, float near_, float far_) {
        tan_half_x_fov = tan(x_fov/2);
        tan_half_y_fov = tan_half_x_fov/aspect_ratio;
        near_dist = near_;
        far_dist = far_;
        recalculate_projection();
    }

    void recalculate_projection() {
        projection = {
            {1/tan_half_x_fov, 0, 0, 0}, 
            {0, 1/tan_half_y_fov, 0, 0}, 
            {0, 0, (far_dist+near_dist)/(far_dist-near_dist), 1}, 
            {0, 0, -2*far_dist*near_dist/(far_dist-near_dist), 0}
        };
    }

    void set_x_fov(float new_x_fov, float aspect_ratio) {
        tan_half_x_fov = tan(new_x_fov/2);
        set_aspect_ratio(aspect_ratio);
    }

    void set_far(float new_far) {
        far_dist = new_far;
        recalculate_projection();
    }

    void set_near(float new_near) {
        near_dist = new_near;
        recalculate_projection();
    }

    void set_aspect_ratio(float new_aspect_ratio) {
        tan_half_y_fov = tan_half_x_fov/new_aspect_ratio;
        recalculate_projection();
    }

    std::pair<glm::mat4, glm::mat4> get_look_at() {
        glm::mat4 cam_translate = glm::translate(I, glm::vec3(-pos));
        glm::mat4 cam_rotate = glm::rotate(I, pitch, X) * glm::rotate(I, yaw, Y);
        glm::mat4 look_at = cam_rotate*cam_translate;
        glm::mat4 normal_look_at = glm::transpose(glm::inverse(look_at));
        
        return std::make_pair(look_at, normal_look_at);
    }
};

#endif
