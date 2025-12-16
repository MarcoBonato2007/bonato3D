#ifndef DRAWING_H
#define DRAWING_H

#include <iostream>

#include <glm/glm.hpp>

uint32_t width, height;
std::vector<uint32_t> frame_buffer; // 32-bit RGBA
std::vector<float> depth_buffer;

inline void setPixel(uint32_t x, uint32_t y, uint32_t color) {
    frame_buffer[x + y*width] = color;
}

inline int getDepth(uint32_t x, uint32_t y) {
    return depth_buffer[x + y*width];
}

inline void setDepth(uint32_t x, uint32_t y, float new_depth) {
    depth_buffer[x + y*width] = new_depth;
}

// checks how much point c is to the left of line from a to b
inline float leftness(float ax, float ay, float bx, float by, float cx, float cy) {
    // is > 0 if (a, b) is left, = 0 if on, and < 0 if right of the line
    return (bx-ax)*(cy-ay) - (by-ay)*(cx-ax);
}

// checks if the point c is to the left of line from a to b
// takes into account the top left rule
inline bool isLeft(float ax, float ay, float bx, float by, float cx, float cy) {
    // res is > 0 if (a, b) is left, = 0 if on, and < 0 if right of the line
    float res = (bx-ax)*(cy-ay) - (by-ay)*(cx-ax); // minus sign due to flipped y axis

    // If this is a left or top edge we want to exclude when res = 0 (top left rule)
    if ((by-ay == 0 && bx-ax < 0) || (by-ay > 0)) {
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

    float x_increm = -normal.x/normal.z;
    float y_increm = -normal.y/normal.z;
    // start with depth of top left pixel of bounding box
    float cur_depth = v1.z + (bl.x+0.5-v1.x)*x_increm + (bl.y+0.5-v1.y)*y_increm;

    int x_min = std::max(0,(int)bl.x);
    int x_max = std::min((unsigned int)tr.x, width-1);
    int y_min = std::max(0,(int)bl.y);
    int y_max = std::min((unsigned int)tr.y, height-1);

    float v1x = v1.x;
    float v1y = v1.y;
    float v2x = v2.x;
    float v2y = v2.y;
    float v3x = v3.x;
    float v3y = v3.y;

    float leftness1 = leftness(v1x, v1y, v2x, v2y, x_min, y_min);
    bool leftortop1 = ((v2y-v1y == 0 && v2x-v1x < 0) || (v2y-v1y > 0));
    float leftness2 = leftness(v2x, v2y, v3x, v3y, x_min, y_min);
    bool leftortop2 = ((v3y-v2y == 0 && v3x-v2x < 0) || (v3y-v2y > 0));
    float leftness3 = leftness(v3x, v3y, v1x, v1y, x_min, y_min);
    bool leftortop3 = ((v1y-v3y == 0 && v1x-v3x < 0) || (v1y-v3y > 0));

    for (int x=x_min; x<=x_max; x++) {
        float start_row_depth = cur_depth;
        float start_leftness1 = leftness1;
        float start_leftness2 = leftness2;
        float start_leftness3 = leftness3;

        for (int y=y_min; y<=y_max; y++) {                
            float x_ac = x+0.5f;
            float y_ac = y+0.5f;

            if (
                (leftness1 > 0 || (leftness1 == 0 && !leftortop1))
                && (leftness2 > 0 || (leftness2 == 0 && !leftortop2))
                && (leftness3 > 0 || (leftness3 == 0 && !leftortop3))
                && cur_depth >= getDepth(x,y)
                && cur_depth >= -1 // TODO: add proper frustum culling and near clipping
                && cur_depth <= 1
            ) {
                setPixel(x, y, color);
                setDepth(x, y, cur_depth);
            }

            cur_depth += y_increm;
            leftness1 += (v2x - v1x);
            leftness2 += (v3x - v2x);
            leftness3 += (v1x - v3x);
        }

        start_row_depth += x_increm;
        cur_depth = start_row_depth;

        start_leftness1 -= (v2y - v1y);
        start_leftness2 -= (v3y - v2y);
        start_leftness3 -= (v1y - v3y);
        leftness1 = start_leftness1;
        leftness2 = start_leftness2;
        leftness3 = start_leftness3;
    }

}

#endif