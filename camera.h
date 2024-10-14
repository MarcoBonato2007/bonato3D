#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "constants.h"
#include "triangle.h"

struct Camera {
    float tan_half_x_fov;
    float tan_half_y_fov;
    float near;
    float far;
    glm::mat4 projection;
    float pitch = 0;
    float yaw = 0;
    glm::vec4 pos = {0, 0, 0, 1};

    Camera() {}

    Camera(float x_fov, float aspect_ratio, float near_, float far_) {
        tan_half_x_fov = tan(x_fov/2);
        tan_half_y_fov = tan_half_x_fov/aspect_ratio;
        near = near_;
        far = far_;
        recalculate_projection();
    }

    void recalculate_projection() {
        projection = {
            {1/tan_half_x_fov, 0, 0, 0}, 
            {0, 1/tan_half_y_fov, 0, 0}, 
            {0, 0, (far+near)/(far-near), 1}, 
            {0, 0, -2*far*near/(far-near), 0}
        };
    }

    void set_x_fov(float new_x_fov, float aspect_ratio) {
        tan_half_x_fov = tan(new_x_fov/2);
        set_aspect_ratio(aspect_ratio);
    }

    void set_far(float new_far) {
        far = new_far;
        recalculate_projection();
    }

    void set_near(float new_near) {
        near = new_near;
        recalculate_projection();
    }

    void set_aspect_ratio(float new_aspect_ratio) {
        tan_half_y_fov = tan_half_x_fov/new_aspect_ratio;
        recalculate_projection();
    }

    std::pair<glm::mat4, glm::mat4> get_look_at() {
        glm::mat4 cam_translate = glm::translate(I, glm::vec3(-pos));
        glm::mat4 cam_rotate = glm::rotate(I, pitch, X) * glm::rotate(I, yaw, Y);
        glm::mat4 look_at = cam_rotate*cam_translate;
        glm::mat4 normal_look_at = glm::transpose(glm::inverse(look_at));
        
        return std::make_pair(look_at, normal_look_at);
    }

    bool is_line_visible(glm::vec4 start, glm::vec4 end) {
        glm::vec4 line_seg = end-start;

        std::pair<float, float> lower_fractions[3] = {
            {-start.z*tan_half_x_fov-start.x, line_seg.x+line_seg.z*tan_half_x_fov},
            {-start.z*tan_half_y_fov-start.y, line_seg.y+line_seg.z*tan_half_y_fov}, 
            {near-start.z, line_seg.z}
        };

        std::pair<float, float> upper_fractions[3] = {
            {start.z*tan_half_x_fov-start.x, line_seg.x-line_seg.z*tan_half_x_fov},
            {start.z*tan_half_y_fov-start.y, line_seg.y-line_seg.z*tan_half_y_fov},
            {far-start.z, line_seg.z}
        };

        float lower_bound = -INFINITY;
        float upper_bound = INFINITY;

        for (std::pair<float, float> fraction: lower_fractions) {
            if (fraction.second > 0) {
                lower_bound = std::max(lower_bound, fraction.first/fraction.second);
            }
            else if (fraction.second < 0) {
                upper_bound = std::min(upper_bound, fraction.first/fraction.second);
            }
            else if (fraction.first > 0) {return false;}
        }

        for (std::pair<float, float> fraction: upper_fractions) {
            if (fraction.second > 0) {
                upper_bound = std::min(upper_bound, fraction.first/fraction.second);
            }
            else if (fraction.second < 0) {
                lower_bound = std::max(lower_bound, fraction.first/fraction.second);
            }
            else if (fraction.first < 0) {return false;}
        }
        
        return (lower_bound <= 1 and upper_bound >= 0);
    }

    bool is_triangle_in_frustum(glm::vec4 vert1, glm::vec4 vert2, glm::vec4 vert3) {
        std::pair<glm::vec4, glm::vec4> lines[3] = {{vert1, vert2}, {vert1, vert3}, {vert2, vert3}};

        bool flag = false;
        for (auto points: lines) {
            if (is_line_visible(points.first, points.second)) {
                flag = true;
            }
        }

        return flag;
    }

    std::vector<std::tuple<glm::vec4, glm::vec4, glm::vec4>> near_clip_triangle(
        glm::vec4 vert1,
        glm::vec4 vert2,
        glm::vec4 vert3
    ) {
        // Clips this triangle agains the plane z = near
        // Returns a vector of resulting clipped triangles (only 1 or 2 triangles)

        std::vector<glm::vec4> verts_behind_near = {};
        std::vector<glm::vec4> verts_in_front_of_near = {};
        for (glm::vec4 vert: {vert1, vert2, vert3}) {
            if (vert.z < near) {
                verts_behind_near.push_back(vert);
            }
            else {
                verts_in_front_of_near.push_back(vert);
            }
        }

        glm::vec4 vert_1, vert_2, vert_3;

        if (verts_behind_near.size() == 0) {
            return {{vert1, vert2, vert3}};
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

        glm::vec4 intersect_1 = vert_1 + (near-vert_1.z)/(vert_2.z-vert_1.z) * (vert_2-vert_1);
        glm::vec4 intersect_2 = vert_1 + (near-vert_1.z)/(vert_3.z-vert_1.z) * (vert_3-vert_1);

        if (verts_behind_near.size() == 1) {
            return {{intersect_1, intersect_2, vert_2}, {vert_2, vert_3, intersect_2}};
        }
        else {
            return {{intersect_1, intersect_2, vert_1}};
        }
    }

    std::vector<std::tuple<glm::vec4, glm::vec4, glm::vec4>> process_triangle(Triangle triangle) {
        glm::mat4 look_at, normal_look_at;
        std::tie(look_at, normal_look_at) = get_look_at();

        triangle.vert1 = look_at*triangle.vert1;
        triangle.vert2 = look_at*triangle.vert2;
        triangle.vert3 = look_at*triangle.vert3;
        triangle.normal = normal_look_at*triangle.normal;

        if (is_triangle_in_frustum(triangle.vert1, triangle.vert2, triangle.vert3) and glm::dot(triangle.vert1, triangle.normal) < triangle.vert1.w*triangle.normal.w) {
            return near_clip_triangle(triangle.vert1, triangle.vert2, triangle.vert3);
        }
        else {
            return {};
        }
    }

};

#endif
