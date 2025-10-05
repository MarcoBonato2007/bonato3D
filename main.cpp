#include <GLFW/glfw3.h> // used to open windows, take input, etc.

#include <vector>

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(800, 600, "Pixels", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    std::vector<uint32_t> framebuffer(800*600);
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Fill framebuffer manually
        for (int y=0; y<600; ++y)
            for (int x=0; x<800; ++x)
                framebuffer[y*800 + x] = 0xFF0000FF; // Red pixels (RGBA)

        glDrawPixels(800, 600, GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
}
