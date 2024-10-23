#include <glad/glad.h>
#include "glad.c"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>

#include "shader.h"
#include "constants.h"
#include "player.h"

GLfloat vertices[] = { // 3 vals for position, then 4 vals for color
    -0.5f, 0.5f, 5.0f,   0.2f, 0.3f, 0.1f, 1.0f,
    0.5f, 0.5f, 5.0f,   0.1f, 0.3f, 0.6f, 1.0f,
    0.5f, -0.5f, 5.0f,   0.6f, 0.1f, 0.90f, 1.0f,
    -0.5f, -0.5f, 5.0f,  0.9f, 0.3f, 0.7f, 1.0f,

    -0.5f, 0.5f, 6.0f,   0.46f, 0.2f, 0.8f, 1.0f,
    0.5f, 0.5f, 6.0f,   0.67f, 0.69f, 0.4f, 1.0f,
    0.5f, -0.5f, 6.0f,   0.84f, 0.20f, 0.14f, 1.0f,
    -0.5f, -0.5f, 6.0f,  0.48f, 0.27f, 0.84f, 1.0f,
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

    Shader shader("vert_shader.vert", "frag_shader.frag");
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*) 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    GLuint look_at_location = glGetUniformLocation(shader.ID, "look_at");
    GLuint projection_location = glGetUniformLocation(shader.ID, "projection");

    float prev_time = 0;
    float cur_time = 0;
    float delta_time = 0;
    
    double cursor_x, cursor_y;
    double prev_cursor_x, prev_cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);

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

// Swap to using triangle strips
