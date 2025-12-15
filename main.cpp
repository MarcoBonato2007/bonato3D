#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <chrono>

#include "headers/model.h"
#include "headers/drawing.h"
#include "headers/player.h"

static void err_cb(int error, const char* desc) {
    printf("GLFW error: %d: %s\n", error, desc);
}

void mainLoop(GLFWwindow* window) {
    double cursor_x, cursor_y;
    double prev_cursor_x, prev_cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);

    auto start = std::chrono::system_clock::now();
    int n = 0;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        look_at = get_look_at(pos, pitch, yaw);
        mvp = proj*look_at;

        std::fill(frame_buffer.begin(), frame_buffer.end(), 0);
        std::fill(depth_buffer.begin(), depth_buffer.end(), -1.0f);

        model.draw();

        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame_buffer.data());

        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        keyboard_handler(window);
        mouse_handler(cursor_x-prev_cursor_x, cursor_y-prev_cursor_y);
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;

        glfwSwapBuffers(window);

        n++;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::cout << (float)n/elapsed_seconds.count() << std::endl;
    
    glfwTerminate();
}

void printInfo() {
    std::cout << "Width: " << width << "\n" << "Height: " << height << std::endl;
    std::cout << "Aspect: " << aspect << std::endl;
}

void init(std::string pathToModel) {
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
        {0, 0, (near+far)/(near-far), -1}, 
        {0, 0, (2*near*far)/(near-far), 0}
    };
    GLFWwindow* window = glfwCreateWindow(800, 600, "Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // means we get precise input data

    frame_buffer = std::vector<uint32_t>(width*height); // see buffers.h
    depth_buffer = std::vector<float>(width*height);

    model = Model(); // no path provided -> standard cube model
    // model = Model(pathToModel); // see model.h
    
    printInfo();

    mainLoop(window);
}

int main(int argc, char *argv[]) {
    std::string pathToModel = "";
    if (argc == 1) {
        std::cout << "Must enter a single argument: the path to the file you want to display" << std::endl;
        return 1;
    }
    else {
        pathToModel = argv[1];
    }

    init(pathToModel);
}

// main is responsible for initializing glfw and starting a mainloop
// all glfw and opengl code is contained only inside here.

// TODO:
    // try rendering to a quad instead of using glDrawPixels
    // is it any faster??

    // Now try to remove all the glm use, and optimize math (barycentric interpolate depth and the left-ness thing)
    // Is it faster? good, keep it. Otherwise keep glm.
    // Now 

    // Then add more meshes / complicated stuff until it's slow.
    // Then work on basic optimizations:
        // automatically exclude backface triangles from drawing
        // Frustum culling
            // Check bounding boxes in world spaces
            // Check triangles in clip spaces
