#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>

#include "headers/model.h"
#include "headers/drawing.h"

void mainLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        draw(); // this is the core of the program. check drawing.h

        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, frame_buffer.data());

        // keyboard handling to go here in future

        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
}

void init(std::string pathToModel) {
    glfwInit();

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode -> width; // available globally in buffers.h
    height = mode -> height;
    GLFWwindow* window = glfwCreateWindow(width, height, "Engine", monitor, NULL);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // means we get precise input data

    frame_buffer = std::vector<uint32_t>(width*height); // see buffers.h
    depth_buffer = std::vector<uint32_t>(width*height);

    model = Model(pathToModel); // see model.h

    mainLoop(window);
}

int main(int argc, char *argv[]) {
    std::string pathToModel = "";
    if (argc != 1) {
        std::cout << "Must enter a single argument: the path to the file you want to display" << std::endl;
        return -1;
    }
    else {
        pathToModel = argv[0];
    }

    init(pathToModel);
}

// main is responsible for initializing glfw and starting a mainloop
// all glfw and opengl code is contained purely inside here.
