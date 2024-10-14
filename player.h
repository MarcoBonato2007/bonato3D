#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "object.h"

struct Player: Object {
    Camera camera;
    float movement_speed;
    float rotation_speed;

    Player(
        glm::vec4 pos_, 
        glm::vec4 vel_, 
        glm::vec4 accel_, 
        float mass_, 
        float x_fov, 
        float aspect_ratio, 
        float near_, 
        float far_,
        float movement_speed_,
        float rotation_speed_
    ) {
        pos = pos_;
        vel = vel_;
        accel = glm::vec4 {0, -9.80665, 0, 1};
        mass = mass_;
        movement_speed = movement_speed_;
        rotation_speed = rotation_speed_;
        camera = Camera(x_fov, aspect_ratio, near_, far_);
    }

    void step(float time_step, glm::vec4 new_accel) {
        // Verlet integration
        pos += time_step * (vel + 1/2*time_step*new_accel);
        vel += 1/2*time_step*(accel + new_accel);
        camera.pos = pos;
    }

    void collision_handler() {
        if (pos.y < 0) {
            pos.y = 0;
            accel.y = 0;
            vel.y = 0;
        }
    }

    void keyboard_handler(GLFWwindow* window, float delta_time) {
        // When the player moves they add a constant velocity
        glm::vec4 delta_vel = {0, 0, 0, 0};

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            delta_vel.z += movement_speed;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            delta_vel.z -= movement_speed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            delta_vel.x += movement_speed;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            delta_vel.x -= movement_speed;
        }

        if (delta_vel.x and delta_vel.y) { // Normalize diagonal movement
            delta_vel.x /= sqrt(2);
            delta_vel.y /= sqrt(2);
        }

        delta_vel *= delta_time;

        // Now we must rotate this change in velocity to face the direction of the camera
        glm::mat4 cam_rotate = glm::rotate(I, -camera.pitch, X) * glm::rotate(I, -camera.yaw, Y);
        delta_vel = cam_rotate*delta_vel;
        delta_vel.y = 0; // Needed as otherwise the player could magically move upwards

        pos += delta_vel;
    }
    
    void mouse_handler(
        int screen_width, 
        int screen_height, 
        double dx, 
        double dy,
        float delta_time
    ) {
        glm::vec3 view_change = {
            dx*(2*camera.near*camera.tan_half_x_fov/screen_width), 
            -dy*(2*camera.near*camera.tan_half_y_fov/screen_height), 
            camera.near
        };

        view_change = glm::normalize(view_change);
        float cam_yaw_change = asin(-view_change.x);
        float cam_pitch_change = asin(view_change.y/cos(cam_yaw_change));

        camera.pitch += cam_pitch_change*rotation_speed;
        camera.yaw += cam_yaw_change*rotation_speed;
    }
};

// This will store things like a camera object and the model

#endif