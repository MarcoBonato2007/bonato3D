#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "constants.h"

struct Camera {
    float fov; // vertical fov, in radians. horizontal fov scales with aspect ratio.
    float near_dist;
    float far_dist;
    float aspect_ratio;
    float speed;
    glm::mat4 projection;
    float tan_half_x_fov;
    float tan_half_y_fov;
    float pitch = 0;
    float yaw = 0;
    glm::vec4 pos = {0, 0, 0, 1};
    float rotation_speed = 1.0;

    // trailing underscores are used for parameters.

    Camera() {}

    Camera(float fov_, float aspect_ratio_, float near_, float far_, float speed_) {
        fov = fov_;
        aspect_ratio = aspect_ratio_;
        near_dist = near_;
        far_dist = far_;
        speed = speed_;
        tan_half_y_fov = tan(fov/2);
        tan_half_x_fov = aspect_ratio*tan_half_y_fov;
        set_projection();
    }

    glm::mat4 get_look_at() {
        glm::mat4 cam_translate = glm::translate(I, glm::vec3(-pos));
        glm::mat4 cam_rotate = glm::rotate(I, -pitch, X) * glm::rotate(I, -yaw, Y);
        glm::mat4 look_at = cam_rotate*cam_translate;
        // glm::mat4 normal_look_at = glm::transpose(glm::inverse(look_at));
        
        return look_at;
    }

    void mouse_rotate( // rotates camera based on mouse movement
        double dx, // mouse movement in pixels
        double dy,
        int screen_width, // in pixels
        int screen_height
    ) {
        // - sign in front of dy since glfw window has inverted y axis compared to opengl
        pitch += rotation_speed*atan(-dy*2*tan_half_y_fov/screen_height);
        if (pitch > PI/2) {
            pitch = PI/2;
        }
        yaw += rotation_speed*atan(-dx*2*tan_half_x_fov/screen_width);
    }

    void keyboard_handler(GLFWwindow* window, float deltaTime) {
        glm::vec4 velocity = {0, 0, 0, 0};
    
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            velocity.z -= 1;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            velocity.z += 1;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            velocity.x += 1;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            velocity.x -= 1;
        }

        velocity = glm::rotate(I, yaw, Y)*velocity;
        velocity.y = 0;

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            velocity.y += 1;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            velocity.y -= 1;
        }

        if (velocity.x != 0 || velocity.y != 0 || velocity.z != 0) {
            velocity = glm::normalize(velocity);
            velocity *= speed*deltaTime;
        }
        
        pos += velocity;
    }

    void set_projection() {
        projection = glm::perspective(fov, aspect_ratio, near_dist, far_dist);
        // projection = { // the matrix is inputted transposed
        //     {1/tan_half_x_fov, 0, 0, 0}, 
        //     {0, 1/tan_half_y_fov, 0, 0}, 
        //     {0, 0, (near+far)/(near-far), -1}, 
        //     {0, 0, 2*near*far/(near-far), 0}
        // };
    }

    void set_fov(float fov_) {
        fov = fov_;
        tan_half_y_fov = tan(fov/2);
        tan_half_x_fov = aspect_ratio*tan_half_y_fov;
        set_projection();
    }

    void set_aspect_ratio(float aspect_ratio_) {
        aspect_ratio = aspect_ratio_;
        set_projection();
    }

    void set_far(float far_) {
        far_dist = far_;
        set_projection();
    }

    void set_near(float near_) {
        near_dist = near_;
        set_projection();
    }
};

#endif
