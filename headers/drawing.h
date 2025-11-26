#ifndef DRAWING_H
#define DRAWING_H

#include "model.h"

uint32_t width, height;
std::vector<uint32_t> frame_buffer; // 32-bit RGBA
std::vector<uint32_t> depth_buffer;

void setPixel(uint32_t x, uint32_t y, uint32_t color) {
    frame_buffer[x + y*width] = color;
}

// Sets the framebuffer. This is the core of the program.
void draw() {
    // temporary example drawing
    for (int x=0; x<width; x++) {
        for (int y=0; y<height; y++) {
            setPixel(x, y, 0xFF0000FF); // red opaque color
        }
    }
}

#endif