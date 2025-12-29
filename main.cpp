#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <chrono>

#include "headers/model.h"
#include "headers/drawing.h"
#include "headers/player.h"

void mainLoop(GLFWwindow* window) {
    double cursor_x, cursor_y;
    double prev_cursor_x, prev_cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);

    int n = 0;
    auto start = std::chrono::system_clock::now();

    auto cur_time = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        look_at = get_look_at(pos, pitch, yaw);
        mvp = proj*look_at;

        std::fill(frame_buffer.begin(), frame_buffer.end(), 0);
        std::fill(depth_buffer.begin(), depth_buffer.end(), 1.0f);

        model.draw();

        auto new_time = std::chrono::steady_clock::now();
        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame_buffer.data());
        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        keyboard_handler(window, (new_time-cur_time).count());
        mouse_handler(cursor_x-prev_cursor_x, cursor_y-prev_cursor_y);
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;
        cur_time = new_time;

        glfwSwapBuffers(window);

        n++;

        // auto end = std::chrono::system_clock::now();
        // std::chrono::duration<double> elapsed_seconds = end-start;
        // if (elapsed_seconds.count() >= 5) {
        //     break;
        // }
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << (float)n/elapsed_seconds.count() << std::endl;
    
    glfwTerminate();
}

inline static void err_cb(int error, const char* desc) {
    printf("GLFW error: %d: %s\n", error, desc);
}

int main() {
    glfwInit();
    glfwSetErrorCallback(err_cb);

    // GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    // const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    // width = mode -> width; // available globally in buffers.h
    // height = mode -> height;

    width = 800;
    height = 600;
    aspect = (float)width/height;
    proj = {
        {1/tan(X_FOV/2), 0, 0, 0}, 
        {0, aspect/tan(X_FOV/2), 0, 0}, 
        {0, 0, -(far+near)/(far-near), -1}, 
        {0, 0, -(2*near*far)/(far-near), 0}
    };
    GLFWwindow* window = glfwCreateWindow(800, 600, "Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // means we get precise input data

    frame_buffer = std::vector<uint32_t>(width*height); // see buffers.h
    depth_buffer = std::vector<float>(width*height);

    model = Model("suzanne.blend"); // "suzanne.blend"
    
    mainLoop(window);
}

// main is responsible for initializing glfw and starting a mainloop
// all glfw and opengl code is contained only inside here.

// TODO:
    // check for graphical bugs (try going inside suzanne, moving around, etc.)
    // loook aroud while close

    // add a top/bottom skybox

    // clean up code
    // can you optimize the drawing loop more?