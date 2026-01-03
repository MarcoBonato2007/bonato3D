#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <chrono>

#include "headers/model.h"
#include "headers/player.h"
#include "headers/drawing.h"

void mainLoop(GLFWwindow* window) {
    double cursor_x, cursor_y;
    double prev_cursor_x, prev_cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);

    auto cur_time = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glm::mat4 mvp = proj*get_look_at(pos, pitch, yaw);;

        std::fill(frame_buffer.begin(), frame_buffer.end(), 0);
        std::fill(depth_buffer.begin(), depth_buffer.end(), 1.0f);

        model.draw(mvp);

        auto new_time = std::chrono::steady_clock::now();

        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame_buffer.data());

        glfwGetCursorPos(window, &cursor_x, &cursor_y);

        keyboard_handler(window, (new_time-cur_time).count());
        mouse_handler(cursor_x-prev_cursor_x, cursor_y-prev_cursor_y);

        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;
        cur_time = new_time;

        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
}

int main() {
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(width, height, "Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // means we get precise input data

    frame_buffer = std::vector<uint32_t>(width*height); // see buffers.h
    depth_buffer = std::vector<float>(width*height);

    model = Model("suzanne.blend");
    
    mainLoop(window);
}

// main is responsible for initializing glfw and starting a mainloop
// all glfw and opengl code is contained only inside here.
