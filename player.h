#ifndef PLAYER_H
#define PLAYER_H

#include "camera.h"
#include "object.h"

struct Player: Object {
    Camera camera;
    float rotation_speed;

    float max_movement_speed;
    float movement_acceleration_mag;
    glm::vec4 movement_accel = {0, 0, 0, 0};
    glm::vec4 movement_velocity = {0, 0, 0, 0};

    Player(
        glm::vec4 pos_, 
        glm::vec4 vel_, 
        glm::vec4 accel_, 
        float mass_, 
        float x_fov, 
        float aspect_ratio, 
        float near_, 
        float far_,
        float max_movement_speed_,
        float movement_acceleration_mag_,
        float rotation_speed_
    ) {
        pos = pos_;
        vel = vel_;
        accel = glm::vec4 {0, -9.80665, 0, 0};
        mass = mass_;
        max_movement_speed = max_movement_speed_;
        movement_acceleration_mag = movement_acceleration_mag_;
        rotation_speed = rotation_speed_;
        camera = Camera(x_fov, aspect_ratio, near_, far_);
    }

    void step(GLFWwindow* window, float delta_time) {
        // Verlet integration
        glm::vec4 new_accel = {0, -9.80665, 0, 1};
        glm::vec4 new_movement_accel = keyboard_handler(window, delta_time);

        glm::vec4 effective_velocity = vel + movement_velocity;
        glm::vec4 effective_accel = new_accel + new_movement_accel;

        // Drag deceleration formula: 1/2*air density*area*drag coefficient * velocity squared / mass
        effective_accel -= 0.5f*1.225f*1*1*glm::length(effective_velocity)*effective_velocity/mass;

        pos += delta_time * (effective_velocity + 0.5f*delta_time*effective_accel);

        vel += 0.5f*delta_time*(accel + new_accel);
        movement_velocity += 0.5f*delta_time*(movement_accel + new_movement_accel);

        accel = new_accel;
        movement_accel = new_movement_accel;
        camera.pos = pos;

        // Limit the maximum movement velocity to max_movement_speed
        float movement_mag = glm::length(movement_velocity);
        if (movement_mag > max_movement_speed) {
            movement_velocity *= max_movement_speed/movement_mag;
        }

    }

    void collision_handler() {
        if (pos.y < 0.5) {
            pos.y = 0.5;
            accel.y = 0;
            vel.y = 0;
        }
    }

    glm::vec4 keyboard_handler(GLFWwindow* window, float delta_time) {
        // When the player moves they add an acceleration to themselves
        glm::vec4 new_movement_accel = {0, 0, 0, 0};

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS and abs(accel.y) < 0.1) {
            vel.y += 5; // Give an initial jump velocity upwards
        }
    
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            new_movement_accel.z += movement_acceleration_mag;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            new_movement_accel.z -= movement_acceleration_mag;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            new_movement_accel.x += movement_acceleration_mag;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            new_movement_accel.x -= movement_acceleration_mag;
        }

        if (new_movement_accel.x and new_movement_accel.y) { // Normalize diagonal movement
            new_movement_accel.x /= sqrt(2);
            new_movement_accel.y /= sqrt(2);
        }

        // We rotate the acceleration to face the direction of the camera
        glm::mat4 cam_rotate = glm::rotate(I, -camera.pitch, X) * glm::rotate(I, -camera.yaw, Y);
        new_movement_accel = cam_rotate*new_movement_accel;
        new_movement_accel.y = 0; // Needed as otherwise the player could magically move upwards
        
        // If they didn't click anything, decelerate them towards zero (opposite direction to vel)
        // This only happens if they are grounded
        if (not (new_movement_accel.x or new_movement_accel.y) and glm::length(movement_velocity) > 0 and abs(accel.y) < 0.1) {
            new_movement_accel = -movement_velocity/glm::length(movement_velocity)*movement_acceleration_mag;

            // We check if applying this acceleration woudl cause the player to stop moving
            // If so, we set the velocity and movement accel to 0
            if (glm::length(0.5f*delta_time*(movement_accel + new_movement_accel)) > glm::length(movement_velocity)) {
                movement_velocity = {0, 0, 0, 0};
                new_movement_accel = {0, 0, 0, 0};
            }
        }

        return new_movement_accel;
    }
    
    void mouse_handler(
        int screen_width, 
        int screen_height, 
        double dx, 
        double dy,
        float delta_time
    ) {
        glm::vec3 view_change = {
            dx*(2*camera.near_dist*camera.tan_half_x_fov/screen_width), 
            -dy*(2*camera.near_dist*camera.tan_half_y_fov/screen_height), 
            camera.near_dist
        };

        view_change = glm::normalize(view_change);
        float cam_yaw_change = asin(-view_change.x);
        float cam_pitch_change = asin(view_change.y/cos(cam_yaw_change));

        camera.pitch += cam_pitch_change*rotation_speed;
        camera.yaw += cam_yaw_change*rotation_speed;
    }
};

#endif
