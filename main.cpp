#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>

float PI = 3.141592653589793;
glm::mat4 I = glm::mat4(1);
glm::vec3 X = glm::vec3 {1, 0, 0};
glm::vec3 Y = glm::vec3 {0, 1, 0};

struct Camera {
    glm::vec4 pos = {0, 0, 0, 1};
    glm::vec3 vel = {0, 0, 0};
    glm::vec3 accel = {0, 0, 0};

    float pitch = 0;
    float yaw = 0;

    float x_fov = PI/2;
    float y_fov = 0.0;
    
    float near_dist = 0.01;
    float far_dist = 1000;
};

struct Triangle {
    glm::vec4 vert1;
    glm::vec4 vert2;
    glm::vec4 vert3;
    glm::vec4 normal;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

bool is_line_visible(glm::vec4 start, glm::vec4 end, Camera camera) {
    glm::vec4 line_seg = end-start;
    float tan_half_x_fov = tan(camera.x_fov/2);
    float tan_half_y_fov = tan(camera.y_fov/2);
    std::pair<std::pair<float, float>, std::pair<float, float>> fractions[4] = {
        {
            {
                -start.z*tan_half_x_fov-start.x, 
                line_seg.x+line_seg.z*tan_half_x_fov
            }, 
            {
                start.z*tan_half_x_fov-start.x, 
                line_seg.x-line_seg.z*tan_half_x_fov
            }
        },
        {
            {
                -start.z*tan_half_y_fov-start.y, 
                line_seg.y+line_seg.z*tan_half_y_fov
            }, 
            {
                start.z*tan_half_y_fov-start.y, 
                line_seg.y-line_seg.z*tan_half_y_fov
            }
        }, 
        {
            {
                camera.near_dist-start.z, 
                line_seg.z
            }, 
            {
                camera.far_dist-start.z, 
                line_seg.z
            }
        }
    };

    float lower_bound = -INFINITY;
    float upper_bound = INFINITY;

    for (auto bound_tests: fractions) {
        float top_1 = bound_tests.first.first;
        float bottom_1 = bound_tests.first.second;
        float top_2 = bound_tests.second.first;
        float bottom_2 = bound_tests.second.second;

        if (bottom_1 > 0) {
            lower_bound = std::max(lower_bound, top_1/bottom_1);
        }
        else if (bottom_1 < 0) {
            upper_bound = std::min(upper_bound, top_1/bottom_1);
        }
        else if (top_1 > 0) {
            return false;
        }

        if (bottom_2 > 0) {
            upper_bound = std::min(upper_bound, top_2/bottom_2);
        }
        else if (bottom_2 < 0) {
            lower_bound = std::max(lower_bound, top_2/bottom_2);
        }
        else if (top_2 < 0) {
            return false;
        }
    }

    if (upper_bound < 0 or lower_bound > 1) {
        return false;
    }
    else {
        return true;
    }
}

bool is_triangle_in_frustum(Triangle triangle, Camera camera) {
    std::pair<glm::vec4, glm::vec4> lines[3] = {
        {triangle.vert1, triangle.vert2}, 
        {triangle.vert1, triangle.vert3}, 
        {triangle.vert2, triangle.vert3}
    };

    bool flag = false;
    for (auto points: lines) {
        if (is_line_visible(points.first, points.second, camera)) {
            flag = true;
        }
    }

    return flag;
}

std::vector<Triangle> clip_triangle(Triangle triangle, Camera camera) {
    // Near plane clipping

    std::vector<glm::vec4> verts_behind_near = {};
    std::vector<glm::vec4> verts_in_front_of_near = {};
    for (auto vert: {triangle.vert1, triangle.vert2, triangle.vert3}) {
        if (vert.z < camera.near_dist) {
            verts_behind_near.push_back(vert);
        }
        else {
            verts_in_front_of_near.push_back(vert);
        }
    }

    glm::vec4 vert_1, vert_2, vert_3;

    if (verts_behind_near.size() == 0) {
        return {triangle};
    }
    else if (verts_behind_near.size() == 1) {
        vert_1 = verts_behind_near[0];
        vert_2 = verts_in_front_of_near[0];
        vert_3 = verts_in_front_of_near[1];
    }
    else {
        vert_1 = verts_in_front_of_near[0];
        vert_2 = verts_behind_near[0];
        vert_3 = verts_behind_near[1];
    }

    glm::vec4 intersect_1 = vert_1 + (camera.near_dist-vert_1.z)/(vert_2.z-vert_1.z) * (vert_2-vert_1);
    glm::vec4 intersect_2 = vert_1 + (camera.near_dist-vert_1.z)/(vert_3.z-vert_1.z) * (vert_3-vert_1);

    if (verts_behind_near.size() == 1) {
        return {
            Triangle(
                intersect_1,
                intersect_2,
                vert_2,
                triangle.normal, 
                triangle.red, 
                triangle.green, 
                triangle.blue
            ), 
            Triangle(
                vert_2, 
                vert_3,
                intersect_2,
                triangle.normal, 
                triangle.red, 
                triangle.green, 
                triangle.blue
            )
        };
    }
    else {
        return {
            Triangle(
                intersect_1, 
                intersect_2, 
                vert_1, 
                triangle.normal, 
                triangle.red, 
                triangle.green, 
                triangle.blue
            )
        };
    }
}

void draw(Camera camera, std::vector<Triangle> triangles) {
    glClear(GL_COLOR_BUFFER_BIT); // Clear all colors on the screen

    glm::mat4 proj = {
        {1/tan(camera.x_fov/2), 0, 0, 0}, 
        {0, 1/tan(camera.y_fov/2), 0, 0}, 
        {0, 0, (camera.far_dist+camera.near_dist)/(camera.far_dist-camera.near_dist), 1}, 
        {0, 0, -2*camera.far_dist*camera.near_dist/(camera.far_dist-camera.near_dist), 0}
    };

    glm::mat4 cam_translate = glm::translate(I, glm::vec3(-camera.pos.x, -camera.pos.y, -camera.pos.z));
    glm::mat4 cam_rotate = glm::rotate(I, camera.pitch, X) * glm::rotate(I, camera.yaw, Y);
    glm::mat4 look_at = cam_rotate*cam_translate;
    glm::mat4 normal_look_at = glm::transpose(glm::inverse(look_at));

    std::vector<Triangle> new_triangles = {};
    for (auto triangle: triangles) {
        triangle.vert1 = look_at*triangle.vert1;
        triangle.vert2 = look_at*triangle.vert2;
        triangle.vert3 = look_at*triangle.vert3;
        triangle.normal = normal_look_at*triangle.normal;

        if (is_triangle_in_frustum(triangle, camera) and glm::dot(triangle.vert1, triangle.normal) < triangle.vert1.w*triangle.normal.w) {
            for (auto clipped_triangle: clip_triangle(triangle, camera)) {
                new_triangles.push_back(clipped_triangle);
            }
        }
    }


    for (auto triangle: new_triangles) {
        triangle.vert1 = proj*triangle.vert1;
        triangle.vert2 = proj*triangle.vert2;
        triangle.vert3 = proj*triangle.vert3;

        // std::cout << triangle.vert1.x/triangle.vert1.w << " " << triangle.vert1.y/triangle.vert1.w << " " << triangle.vert1.z/triangle.vert1.w << std::endl;
        glColor3f(triangle.red, triangle.green, triangle.blue);
        glBegin(GL_TRIANGLES);
            glVertex4f(triangle.vert1.x, triangle.vert1.y, triangle.vert1.z, triangle.vert1.w);
            glVertex4f(triangle.vert2.x, triangle.vert2.y, triangle.vert2.z, triangle.vert2.w);
            glVertex4f(triangle.vert3.x, triangle.vert3.y, triangle.vert3.z, triangle.vert3.w);
        glEnd();
    }

    glFlush();
}

glm::vec4 keyboard_handler(GLFWwindow* window, Camera camera, float player_speed, float delta_time) {
    glm::vec4 camera_translate = glm::vec4(0);

    float x_movement = 0;
    float z_movement = 0;
    float dist = player_speed*delta_time;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        z_movement += dist;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        z_movement -= dist;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        x_movement += dist;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        x_movement -= dist;
    }

    if (x_movement and z_movement) { // Make vertical movement same speed as straight
        x_movement /= sqrt(2);
        z_movement /= sqrt(2);
    }

    camera_translate.x = x_movement;
    camera_translate.z = z_movement;

    glm::mat4 cam_rotate = glm::rotate(I, -camera.pitch, X) * glm::rotate(I, -camera.yaw, Y);
    camera_translate = cam_rotate*camera_translate;
    camera_translate.y = 0; // Don't allow to go up and down
    return camera_translate;
}

std::pair<float, float> mouse_handler(GLFWwindow* window, Camera camera, int width, int height, double dx, double dy) {
    glm::vec3 view_change = {
        dx*(2*camera.near_dist*tan(camera.x_fov/2)/width), 
        -dy*(2*camera.near_dist*tan(camera.y_fov/2)/height), 
        camera.near_dist
    };
    view_change = glm::normalize(view_change);
    float cam_yaw_change = asin(-view_change.x);
    float cam_pitch_change = asin(view_change.y/cos(cam_yaw_change));

    return {cam_pitch_change, cam_yaw_change};
}

int main() {
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "My Title", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    Camera camera;
    camera.y_fov = 2*atan(tan(camera.x_fov/2)/((float) width/(float)height));

    float prev_time = 0;
    float cur_time = 0;

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
            255, 
            255, 
            255
        ), 
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {0.5, 0, 5, 1},
            glm::vec4 {-0.5, 0, 5, 1},
            glm::vec4 {0, 0, -1, 1},
            255, 
            255, 
            255    
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {0, 0, 1, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {-0.5, 0, 6, 1},      
            glm::vec4 {0, 0, 1, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {0.5, 1, 5, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0, 1, 0, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {0.5, 1, 5, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {0, 1, 0, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {-0.5, 0, 5, 1}, 
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {-0.5, 0, 6, 1}, 
            glm::vec4 {0, -1, 0, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {-0.5, 0, 6, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {0, -1, 0, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {0.5, 1, 5, 1}, 
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {1, 0, 0, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {0.5, 0, 5, 1}, 
            glm::vec4 {0.5, 0, 6, 1}, 
            glm::vec4 {0.5, 1, 6, 1}, 
            glm::vec4 {1, 0, 0, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {-0.5, 0, 5, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {-1, 0, 0, 1},
            255, 
            255, 
            255
        ),
        Triangle (
            glm::vec4 {-0.5, 1, 5, 1}, 
            glm::vec4 {-0.5, 0, 6, 1}, 
            glm::vec4 {-0.5, 1, 6, 1}, 
            glm::vec4 {-1, 0, 0, 1},
            255, 
            255, 
            255
        )
    };  

    while (!glfwWindowShouldClose(window)) {
        draw(camera, triangles);

        cur_time = glfwGetTime();
        camera.pos += keyboard_handler(window, camera, 1, cur_time-prev_time);
        prev_time = cur_time;

        glfwGetCursorPos(window, &cursor_x, &cursor_y);
        std::pair<float, float> camera_view_change = mouse_handler(window, camera, width, height, cursor_x-prev_cursor_x, cursor_y-prev_cursor_y);
        camera.pitch += camera_view_change.first;
        camera.yaw += camera_view_change.second;
        prev_cursor_x = cursor_x;
        prev_cursor_y = cursor_y;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}

// Test using opengl builtin frustum culling and clipping and backface culling instead of own algorithms
