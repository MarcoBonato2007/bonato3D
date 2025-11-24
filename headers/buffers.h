#ifndef BUFFERS_H
#define BUFFERS_H

#include <vector>

// contains basic drawing variables and functions

std::vector<uint32_t> framebuffer; // 32-bit RGBA, initialized in main.cpp
uint32_t width, height;
// will need a depth buffer eventually (probably)

void set_pixel(uint32_t x, uint32_t y, uint32_t color) {
    framebuffer[x + y*width] = color;
}

#endif

