#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

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

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        std::fill(frame_buffer.begin(), frame_buffer.end(), 0);

        model.draw();

        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame_buffer.data());

        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        keyboard_handler(window);
        mouse_handler(cursor_x-prev_cursor_x, cursor_y-prev_cursor_y);
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;

        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
}

void printInfo() {
    std::cout << "Width: " << width << "\n" << "Height: " << height << std::endl;
    std::cout << "Aspect: " << aspect << std::endl;
}

void init(std::string pathToModel) {
    glfwInit();
    glfwSetErrorCallback(err_cb);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode -> width; // available globally in buffers.h
    height = mode -> height;
    aspect = (float)width/height;
    proj = {
        {1/tan(X_FOV/2), 0, 0, 0}, 
        {0, aspect/tan(X_FOV/2), 0, 0}, 
        {0, 0, (near+far)/(near-far), -1}, 
        {0, 0, (2*near*far)/(near-far), 0}
    };
    GLFWwindow* window = glfwCreateWindow(width, height, "Engine", monitor, NULL);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // means we get precise input data

    frame_buffer = std::vector<uint32_t>(width*height); // see buffers.h
    depth_buffer = std::vector<float>(width*height);
    std::fill(depth_buffer.begin(), depth_buffer.end(), -1);

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
// all glfw and opengl code is contained purely inside here.

// TODO:
    // Just get basic 3d drawing and projection done
    // Then add more meshes / complicated stuff until it's slow.
    // Then work on basic optimizations:
        // automatically exclude backface triangles from drawing
        // Frustum culling
            // Check bounding boxes in world spaces
            // Check triangles in clip spaces
