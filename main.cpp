#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include "triangle.h"
#include "player.h"
#include "constants.h"

void draw(Camera camera, std::vector<Triangle> triangles) {
    glClear(GL_COLOR_BUFFER_BIT); // Clear all colors on the screen

    std::vector<Triangle> new_triangles = {};

    for (auto triangle: triangles) {
        for (auto processed_triangle: camera.process_triangle(triangle)) {
            Triangle new_triangle = Triangle(
                std::get<0>(processed_triangle),
                std::get<1>(processed_triangle),
                std::get<2>(processed_triangle),
                triangle.normal,
                triangle.red,
                triangle.green,
                triangle.blue
            );
            new_triangles.push_back(new_triangle);
        }
    }

    for (auto triangle: new_triangles) {
        triangle.vert1 = camera.projection*triangle.vert1;
        triangle.vert2 = camera.projection*triangle.vert2;
        triangle.vert3 = camera.projection*triangle.vert3;

        glColor3ub(triangle.red, triangle.green, triangle.blue);
        glBegin(GL_TRIANGLES);
            glVertex4f(triangle.vert1.x, triangle.vert1.y, triangle.vert1.z, triangle.vert1.w);
            glVertex4f(triangle.vert2.x, triangle.vert2.y, triangle.vert2.z, triangle.vert2.w);
            glVertex4f(triangle.vert3.x, triangle.vert3.y, triangle.vert3.z, triangle.vert3.w);
        glEnd();
    }

    glFlush();
}

int main() {
    glfwInit();
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Engine", monitor, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    Player player = Player(
        glm::vec4 {0, 0, 0, 1},
        glm::vec4 {0, 0, 0, 1},
        glm::vec4 {0, 0, 0, 1},
        70, 
        PI/2,
        (float) width/height,
        0.1, 
        1000, 
        1, 
        1
    );

    float prev_time = 0;
    float cur_time = 0;
    float delta_time = 0;
    
    double cursor_x, cursor_y;
    double prev_cursor_x, prev_cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);

    std::vector<Triangle> triangles = {
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {0.5, 1, 5, 1},
            glm::vec4 {0.5, 0, 5, 1},
            glm::vec4 {0, 0, -1, 1},
            167, 
            255, 
            200
        ), 
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {0.5, 0, 5, 1},
            glm::vec4 {-0.5, 0, 5, 1},
            glm::vec4 {0, 0, -1, 1},
            0, 
            0, 
            3    
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {0, 0, 1, 1},
            4, 
            192, 
            78
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {-0.5, 0, 6, 1},      
            glm::vec4 {0, 0, 1, 1},
            255, 
            0, 
            255
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {0.5, 1, 5, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0, 1, 0, 1},
            49, 
            82, 
            6
        ),
        Triangle (
            glm::vec4 {0.5, 1, 5, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {0, 1, 0, 1},
            212, 
            111, 
            69
        ),
        Triangle (
            glm::vec4 {-0.5, 0, 5, 1}, 
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {-0.5, 0, 6, 1}, 
            glm::vec4 {0, -1, 0, 1},
            69, 
            69, 
            69
        ),
        Triangle (
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {-0.5, 0, 6, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {0, -1, 0, 1},
            5, 
            10, 
            15
        ),
        Triangle (
            glm::vec4 {0.5, 1, 5, 1}, 
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {1, 0, 0, 1},
            74, 
            34, 
            80
        ),
        Triangle (
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {1, 0, 0, 1},
            255, 
            242, 
            56
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {-0.5, 0, 5, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {-1, 0, 0, 1},
            12, 
            98, 
            255
        ),
        Triangle (
            glm::vec4 {-0.5, 0, 6, 1}, 
            glm::vec4 {-0.5, 0, 5, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {-1, 0, 0, 1},
            11, 
            16, 
            54
        )
    };  

    while (!glfwWindowShouldClose(window)) {
        draw(player.camera, triangles);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // Now we handle things like physics and moving around
        cur_time = glfwGetTime();
        delta_time = cur_time - prev_time;

        player.step(delta_time, glm::vec4 {0, 0, -9.80665, 0});
        player.collision_handler();
        player.keyboard_handler(window, delta_time);

        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        player.mouse_handler(width, height, cursor_x-prev_cursor_x, cursor_y-prev_cursor_y, delta_time);
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;

        prev_time = cur_time;
    }

    return 0;
}

// Split the code up into multiple files, make it nice and modular
// Test frustum cullingaaw
// Fix the triangle clipping (atm not working for large triangles since checking for line visibility isnt perfect)
// Depth buffering, make sure multiple objects are drawn correctly
// Import external models (like obj files)
// Occlusion culling
// Shaders
// Lighting
// Implement the jumping mechanic
// Make sure deltaTime is working correctly, consider what happens on lag spikes
// Implement a player model
// Collision detection (bounding volumes?, quadtrees, BSP trees, octtrees, BVH, kd-trees), test using a floor
// Realize that things like BSP trees can be precomputed for a reused map
// Add inter-frame detection (required for fast moving objects)
// Levels of detail
// Mipmapping
// Stencil buffer
// Scissor test
// Triangle strips (optimize drawing)
// Bounding volumes for objects
// Frustum cull objects instead of triangles?
// Optimize triangle storage (like storing color by bitmasking uints or storing normals with only 2 values)
// For frustum culling, iterate over bounding meshes vs iterate over triangles?
// Give GPU commands all at once
// Use instancing and batching
// Think of the best differential equation solver to use (verlet, leapfrog, runge-kutta, satisfying contraint functions, etc.)
// Implement friction and drag (think if you really need these, consider things like bullets, jumping off cliffs, sliding down stuff)
// Optimize models and imported files, what if u make ur own format in binary?
// Caching (save and reuse stuff instead of recalculating)
// Try to make branchless code (avoid if/else statements)
// Compute shader
// Temporal reprojection (use info from previous frame to inform current frame)
// Antialiasing (research the various methods)
// Hierarchical z-buffer
// Floating point precision is higher near 0, so check projection for outputted z values
// Depth prepass
// For rigid body physics (if used), see YT video by blackedout01
// Go back through code, clean up, make more readable, optimize
