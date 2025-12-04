#ifndef DRAWING_H
#define DRAWING_H

#include <iostream>

#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/mat2x2.hpp>

#include "model.h"

uint32_t width, height;
std::vector<uint32_t> frame_buffer; // 32-bit RGBA
std::vector<uint32_t> depth_buffer;

void setPixel(uint32_t x, uint32_t y, uint32_t color) {
    frame_buffer[x + y*width] = color;
}

int getDepth(uint32_t x, uint32_t y) {
    return depth_buffer[x + y*width];
}

// checks if the point c is to the left of line from a to b
// takes into account the top left rule
bool isLeft(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
    glm::vec2 ab = b-a;
    glm::vec2 ac = c-a;

    // res is > 0 if (a, b) is left, = 0 if on, and < 0 if right of the line
    int res = - glm::determinant(glm::mat2(ab, ac)); // minus sign due to flipped y axis

    // If this is a left or top edge we want to exclude when res = 0 (top left rule)
    if ((ab[1] == 0 && ab[0] < 0) || (ab[1] > 0)) {
        return (res > 0);
    }
    else {
        return (res >= 0);
    }
}

// Triangle vertices must be counterclockwise
void drawTriangle(uint32_t color, glm::vec2 v1, glm::vec2 v2, glm::vec2 v3) {
    // bottom-left and top-right coordinates of triangle bounding box
    glm::vec2 bl = glm::min(v1, glm::min(v2, v3));
    glm::vec2 tr = glm::max(v1, glm::max(v2, v3));

    for (int x=bl[0]; x<=tr[0]; x++) {
        for (int y=bl[1]; y<=tr[1]; y++) {
            glm::vec2 pos = {x, y};
            
            if (isLeft(v1, v2, pos) && isLeft(v2, v3, pos) && isLeft(v3, v1, pos)) {
                setPixel(x, y, 0xFFFFFF);
            }
        }
    }

    // TODO: add implementation with z-buffer
        // How to get z value for each pixel??

    // TODO: think about how to draw overlapping triangles
}

// Sets the framebuffer. This is the core of the program.
void draw() {
    drawTriangle(0xFFFFFF, {100, height-100}, {width-200, 200}, {100, 100});
}

#endif
