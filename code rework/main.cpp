// Import order is important.

#include <glad/glad.h> // loads in opengl pointers, saving a bunch of time
#include "lib_files/glad.c"
#include <GLFW/glfw3.h> // used to open windows, take input, etc.
#include <glm/glm.hpp> // OpenGL Math library
#include <iostream> // for debugging by printing to terminal

#define STB_IMAGE_IMPLEMENTATION
#include "lib_files/stb_image.h" // for handling images

int main() {
    // Initialization of the GLFW window
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Engine", NULL, NULL);    
    // GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    // const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    // GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Engine", monitor, NULL);
    glfwMakeContextCurrent(window);

    // Input modifications to GLFW window
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // needed for nice camera movement

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // loads in opengl pointers, saves time

    // Mofidifications to OpenGL behavior
    glEnable(GL_DEPTH_TEST); // New elements only added to screen if they have a lower depth
    glEnable(GL_CULL_FACE); // counterclockwise winding faces only are visible, culls backfaces

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Image drawing, physics, input, etc. go here
        
        // Hello triangle
        glColor3f(0.1, 0.2, 0.3);
        glBegin(GL_TRIANGLES);
            glVertex3f(0, 0, -0.5);
            glVertex3f(1, 0, -5);
            glVertex3f(0, 1, -0.5);
        glEnd();

        // glFlush();   


        glfwPollEvents(); // Processes events like drawing objects to screen
        glfwSwapBuffers(window); // front buffer contains current image, back buffer next frame.
    }

    glfwTerminate();
    return 0;
}

// Make a cube and figure out projection.
