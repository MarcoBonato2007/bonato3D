#ifndef DRAWING_H
#define DRAWING_H

#include <iostream>

#include <glm/glm.hpp>

uint32_t width, height;
std::vector<uint32_t> frame_buffer; // 32-bit RGBA
std::vector<float> depth_buffer;

void setPixel(uint32_t x, uint32_t y, uint32_t color) {
    frame_buffer[x + y*width] = color;
}

int getDepth(uint32_t x, uint32_t y) {
    return depth_buffer[x + y*width];
}

void setDepth(uint32_t x, uint32_t y, float new_depth) {
    depth_buffer[x + y*width] = new_depth;
}

// checks if the point c is to the left of line from a to b
// takes into account the top left rule
bool isLeft(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    glm::vec2 ab = b-a;
    glm::vec2 ac = c-a;

    // res is > 0 if (a, b) is left, = 0 if on, and < 0 if right of the line
    float res = glm::determinant(glm::mat2(ab, ac)); // minus sign due to flipped y axis

    // If this is a left or top edge we want to exclude when res = 0 (top left rule)
    if ((ab[1] == 0 && ab[0] < 0) || (ab[1] > 0)) {
        return (res > 0);
    }
    else {
        return (res >= 0);
    }
}

// Triangle vertices must be counterclockwise
// Each vector is x and y screen (pixel) coordinates along with a normalized [-1, 1] depth (z-value)
// Note: do NOT floor v1, v2, or v3
void drawTriangle(uint32_t color, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    // bottom-left and top-right coordinates of triangle bounding box
    glm::vec2 bl = glm::floor(glm::min(v1, glm::min(v2, v3)));
    glm::vec2 tr = glm::ceil(glm::max(v1, glm::max(v2, v3)));

    glm::vec3 normal = glm::cross(v2-v1, v3-v1);

    glm::vec3 bottom; // bottom most vertex
    if (v1.y == bl.y) {
        bottom = v1;
    }
    else if (v2.y == bl.y) {
        bottom = v2;
    }
    else {
        bottom = v3;
    }

    float x_increm = -normal.x/normal.z;
    float y_increm = -normal.y/normal.z;
    // start with depth of top left pixel of bounding box
    float cur_depth = bottom.z + (bl.x+0.5-bottom.x)*x_increm + (bl.y+0.5-bottom.y)*y_increm;

    for (int x=bl.x; x<=tr.x; x++) {
        if (x < 0 || x >= width) {continue;}

        float start_row_depth = cur_depth;

        for (int y=bl.y; y<=tr.y; y++) {
            if (y < 0 || y >= height) {continue;}
                
            glm::vec2 pos = {x+0.5f, y+0.5f};
            if (isLeft(v1, v2, pos) 
                && isLeft(v2, v3, pos) 
                && isLeft(v3, v1, pos) 
                && cur_depth >= depth_buffer[x + y*width]
                && cur_depth >= -1
                && cur_depth <= 1
            ) {
                setPixel(x, y, color);
                setDepth(x, y, cur_depth);
            }
            cur_depth += y_increm;
        }

        start_row_depth += x_increm;
        cur_depth = start_row_depth;
    }

}

#endif
