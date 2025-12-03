#ifndef DRAWING_H
#define DRAWING_H

#include <iostream>
#include "model.h"

uint32_t width, height;
std::vector<uint32_t> frame_buffer; // 32-bit RGBA
std::vector<uint32_t> depth_buffer;

void setPixel(uint32_t x, uint32_t y, uint32_t color) {
    frame_buffer[x + y*width] = color;
}

// checks if (a, b) is to the left of line from (x1, y1) to (x2, y2)
// takes into account the top left rule
bool isLeft(int x1, int y1, int x2, int y2, int a, int b) {
    // res is > 0 if (a, b) is left, = 0 if on, and < 0 if right of the line
    // derived via a determinant (factor of -1 due to flipped y axis)
    int dx = x2-x1;
    int dy = y2-y1;
    int res = (a-x1)*dy - (b-y1)*dx;

    // If this is a left or top edge we want to exclude when res = 0 (top left rule)
    if ((dy == 0 && dx < 0) || (dy > 0)) {
        return (res > 0);
    }
    else {
        return (res >= 0);
    }
}

// Triangle vertices must be counterclockwise
void triangle(uint32_t color, int x1, int y1, int x2, int y2, int x3, int y3) {
    int x_min = std::min(x3, std::min(x1, x2));
    int x_max = std::max(x3, std::max(x1, x2));
    int y_min = std::min(y3, std::min(y1, y2));
    int y_max = std::max(y3, std::max(y1, y2));

    for (int x=x_min; x<=x_max; x++) {
        for (int y=y_min; y<=y_max; y++) {
            bool a = isLeft(x1, y1, x2, y2, x, y);
            bool b = isLeft(x2, y2, x3, y3, x, y);
            bool c = isLeft(x3, y3, x1, y1, x, y);

            if (a && b && c) {
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
    triangle(0xFFFFFF, 100, height-100, width-200, 200, 100, 100);
}

#endif
