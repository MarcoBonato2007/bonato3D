#include <GLFW/glfw3.h>
#include <vector>

#include "headers/buffers.h"

int main() {
    glfwInit();

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode -> width; // available globally in buffers.h
    height = mode -> height;
    GLFWwindow* window = glfwCreateWindow(width, height, "Engine", monitor, NULL);
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // means we get precise input data

    framebuffer = std::vector<uint32_t>(width*height); // see buffers.h
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // temporary example drawing
        for (int x=0; x<width; x++) {
            for (int y=0; y<height; y++) {
                set_pixel(x, y, 0xFF0000FF); // red opaque color
            }
        }

        glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());
        // keyboard handling to go here in future
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
}
