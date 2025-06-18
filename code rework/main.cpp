// Forgive the insane amount of comments. It's difficult to remember how all this works.

// Import order is important.
#include <glad/glad.h> // loads in opengl pointers, saving a bunch of time
#include "lib_files/glad.c"
#include <GLFW/glfw3.h> // used to open windows, take input, etc.
#include <glm/glm.hpp> // OpenGL Math library
#include <iostream> // for debugging by printing to terminal

// #define STB_IMAGE_IMPLEMENTATION
// #include "lib_files/stb_image.h" // for handling images

#include "headers/camera.h"
#include "headers/shader.h"

// Pipeline (excl. tesselation, geometry shader):
    // 1. Specifying vertices (e.g. putting vertex data in buffers)
    // 2. Vertex shader (transforms position, color, texture, etc. of vertices), contains:
        // Projection transform, lookat transform, etc.
    // 3. Post processing, contains:
        // Primitive assembly (composing vertices into triangles)
        // Clipping
        // Face culling
    // 4. Rasterization, i.e. drawing shapes onto pixels.
        // A fragment describes the result of rasterizing a primitive.
    // 5. Fragment shader, processes each fragment, sets things like vertex color

Camera camera;
Shader shader;

// vertices and indices for a cube
GLfloat vertices[] = { // 3 vals for position, then 4 vals (RGBA) for color
    -0.5f, 0.5f, -5.0f,   1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -5.0f,   0.0f, 1.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -5.0f,   0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -5.0f,  0.9f, 0.3f, 0.7f, 1.0f,
    -0.5f, 0.5f, -6.0f,   0.46f, 0.2f, 0.8f, 1.0f,
    0.5f, 0.5f, -6.0f,   0.67f, 0.69f, 0.4f, 1.0f,
    0.5f, -0.5f, -6.0f,   0.84f, 0.20f, 0.14f, 1.0f,
    -0.5f, -0.5f, -6.0f,  0.48f, 0.27f, 0.84f, 1.0f,
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
    1, 4, 0,
    2, 3, 7,
    2, 7, 6
};  

GLuint specify_vertices() {
    // A buffer holds a piece of GPU memory.
    // A VBO (Vertex Buffer Object) holds vertex info (e.g. position, color, tex coords, etc.)
    // An EBO (Element Buffer Object) holds indices of vertices to draw the triangles with
    // A VAO (Vertex Array Object) holds info about the VBO (e.g. stride, size of coords, etc.)
    GLuint VBO, VAO, EBO; // buffers are idenfitied using a "name" which is an unsigned int
    glGenVertexArrays(1, &VAO); // first argument is how many arrays/buffers to create.
    glGenBuffers(1, &VBO); // reference is passed so the name can be updated
    glGenBuffers(1, &EBO);

    // these calls "bind" the arrays/buffers we created to targets (e.g. the current array buffer).
    // in this way you can have multiple VAO's/VBO's/EBO's, to swap you just have to glBind(name)
    glBindVertexArray(VAO); // sets current Vertex Array to this VAO.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Now we "tell" OpenGL a bunch of information about the layour of our vertex array.
    // glVertexAttribPointer specifies a vertex attribute.
        // Parameters are:
            // index (also the index used in shaders)
            // number of components
            // component size in bytes 
            // is normalized?
            // stride (i.e. vertex data repeats every ... bytes)
            // pointer to first piece of data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*) 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7*sizeof(GLfloat), (void*) (3*sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // Now all the initialization for our buffers and arrays is done!

    return VAO;
}

int main() { // contains some initializations and main loop
    // Initialization of the GLFW window, plus some input modifications
    glfwInit();
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Engine", NULL, NULL);    
    // GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Engine", monitor, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // hides cursor
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE); // needed for nice camera movement

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress); // loads in opengl pointers, saves time

    glEnable(GL_DEPTH_TEST); // New elements only added to screen if they have a lower depth
    glEnable(GL_CULL_FACE); // counterclockwise winding faces only are visible, culls backfaces

    camera = Camera(PI/2, (float)(1920)/(1080), 0.1, 100, 5);
    shader = Shader("shaders/vert_shader.vert", "shaders/frag_shader.frag");
    GLuint VAO = specify_vertices();

    double cursor_x, cursor_y;
    double prev_cursor_x, prev_cursor_y;
    glfwGetCursorPos(window, &cursor_x, &cursor_y);
    glfwGetCursorPos(window, &prev_cursor_x, &prev_cursor_y);

    float cur_time = 0;
    float prev_time = 0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(shader.look_at_location, 1, GL_FALSE, &camera.get_look_at()[0][0]);
        glUniformMatrix4fv(shader.projection_location, 1, GL_FALSE, &camera.projection[0][0]);
        shader.execute();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, std::size(indices), GL_UNSIGNED_INT, 0);

        glfwPollEvents(); // Processes events like drawing objects to screen
        glfwSwapBuffers(window); // front buffer contains current image, back buffer next frame.

        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        camera.mouse_rotate(cursor_x-prev_cursor_x, cursor_y-prev_cursor_y, 1920, 1080);
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;

        cur_time = glfwGetTime();
        camera.keyboard_handler(window, cur_time-prev_time);
        prev_time = cur_time;
    }

    glfwTerminate();
    return 0;

    // try using opengl builtin mouse handler function
}
