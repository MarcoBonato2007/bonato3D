#include <glad/glad.h>
#include "lib_files/glad.c"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "lib_files/stb_image.h"

#include "headers/shader.h"
#include "headers/constants.h"
#include "headers/player.h"

GLfloat vertices[] = { // 3 vals for position, then 4 vals for color, then 2 vals for tex coords
    -0.5f, 0.5f, 5.0f,   1.0f, 0.0f, 0.0f, 1.0f,    0.0f, 4.0f,
    0.5f, 0.5f, 5.0f,   0.0f, 1.0f, 0.0f, 1.0f,     4.0f, 4.0f,
    0.5f, -0.5f, 5.0f,   0.0f, 0.0f, 1.0f, 1.0f,   4.0f, 0.0f,
    -0.5f, -0.5f, 5.0f,  0.9f, 0.3f, 0.7f, 1.0f,   0.0f, 0.0f,

    -0.5f, 0.5f, 6.0f,   0.46f, 0.2f, 0.8f, 1.0f,  0.0f, 4.0f,
    0.5f, 0.5f, 6.0f,   0.67f, 0.69f, 0.4f, 1.0f,  4.0f, 4.0f,
    0.5f, -0.5f, 6.0f,   0.84f, 0.20f, 0.14f, 1.0f, 4.0f, 0.0f,
    -0.5f, -0.5f, 6.0f,  0.48f, 0.27f, 0.84f, 1.0f, 0.0f, 0.0f
};

GLuint indices[] = { // Specifies the triangles to draw using indices from the vertices given
    2, 1, 0,
    2, 0, 3,
    6, 4, 5,
    6, 7, 4,
    3, 0, 4,
    3, 4, 7,
    6, 5, 1,
    6, 1, 2,
    1, 5, 4, 
    1, 4, 0
};  

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

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); // counterclockwise winding faces are visible

    Shader shader("shaders/vert_shader.vert", "shaders/frag_shader.frag");
    Player player = Player(
        glm::vec4 {0, 0, 0, 1},
        glm::vec4 {0, 0, 0, 1},
        glm::vec4 {0, 0, 0, 1},
        70, 
        PI/2,
        (float) (mode->width)/(mode->height),
        0.1, 
        1000, 
        4, 
        40,
        1
    );

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*) 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*) (3*sizeof(GLfloat)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9*sizeof(GLfloat), (void*) (7*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    GLuint look_at_location = glGetUniformLocation(shader.ID, "look_at");
    GLuint projection_location = glGetUniformLocation(shader.ID, "projection");

    float prev_time = 0;
    float cur_time = 0;
    float delta_time = 0;
    
    double cursor_x, cursor_y;
    double prev_cursor_x, prev_cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);

    glActiveTexture(GL_TEXTURE0);
    GLuint container_texture;
    glGenTextures(1, &container_texture);
    glBindTexture(GL_TEXTURE_2D, container_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int img_width, img_height, num_channels;
    unsigned char *image_data = stbi_load("textures/container.jpg", &img_width, &img_height, &num_channels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    glActiveTexture(GL_TEXTURE1);
    GLuint smiley_texture;
    glGenTextures(1, &smiley_texture);
    glBindTexture(GL_TEXTURE_2D, smiley_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_set_flip_vertically_on_load(true);
    image_data = stbi_load("textures/awesomeface.png", &img_width, &img_height, &num_channels, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image_data);

    shader.execute();
    glUniform1i(glGetUniformLocation(shader.ID, "input_texture"), 0);
    glUniform1i(glGetUniformLocation(shader.ID, "input_texture_2"), 1);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 look_at, normal_look_at;
        std::tie(look_at, normal_look_at) = player.camera.get_look_at();

        glUniformMatrix4fv(look_at_location, 1, GL_FALSE, &look_at[0][0]);
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, &player.camera.projection[0][0]);

        shader.execute();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, 0);

        cur_time = glfwGetTime();
        delta_time = cur_time - prev_time;

        player.collision_handler();
        player.step(window, delta_time);

        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        player.mouse_handler(width, height, cursor_x-prev_cursor_x, cursor_y-prev_cursor_y, delta_time);
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;

        prev_time = cur_time;

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

// Containerize code into individual mesh (each mesh has a single texture)
// Import external models (see learnopengl tutorial, remember that multiple meshes can make a single object)
// Have each object store a model matrix (containing translation and rotation, to get to world space)
// Try to group all static objects together

// Add lots of stuff around you in a circle until the scene is too slow
// Bounding volumes for objects (do different ones for different objects, e.g. a spherical one for an asteroid model)
// Remember to frustum cull before even processing any part of an object (but u can project the bounding box for example)
// Frustum cull on each object (see opengl tutorial, don't do this in clip space or bounding spheres won't work)
// Add even more stuff evenly around you to make it slower
// Optimize frustum culling w/ acceleration structured like BVH, BSP tree and octree (so that you don't check every object)
// Consider possibly using a quadtree if frustum culling on the y axis isn't that important
// Precompute the acceleration structure for a static scene

// Implement a player model
// Collision detection (bounding volumes?, quadtrees, BSP trees, octtrees, BVH, kd-trees)
// Test collision detection with a floor and other objects

// Make sure deltaTime is working correctly, consider what happens on lag spikes
// Add inter-frame collision detection (required for fast moving objects)

// More features to add:
    // Lighting (see learnopengl tutorial, do a simple trickery like phong lighting)
    // Friction (how to detect if someone is sliding? (no movement accel when touching)

// List of optimizations
    // MAKE THE SCENE SLOWER BEFORE! Or check the time in ms of render calls
    // Occlusion culling (can precompute for a certain scene)
    // See what else you can optimize with space partitioning
    // Give GPU commands all at once
    // Use instancing and batching
    // Optimize triangle storage (like storing color by bitmasking uints or storing normals with only 2 values)
    // Texture atlas
    // Levels of detail
    // Mipmapping
    // Stencil buffer
    // Scissor test
    // Optimize models and imported files, what if u make ur own format in binary?
    // Caching (save and reuse stuff instead of recalculating), or try to check code for cache miss stuff
    // Try to make branchless code for shaders
    // Compute shader
    // Try SIMD/vectorization or implementing algorithms on GPU with extra compute shaders instead of CPU (e.g. frustum culling on GPU)
    // Temporal reprojection (use info from previous frame to inform current frame)
    // Antialiasing (research the various methods)
    // Hierarchical z-buffer?
    // Depth prepass

// For rigid body physics (if used), see YT video by blackedout01
// Go back through code, clean up, make more readable, optimize
// Optimize data stuff, like using short ints for indices for models
