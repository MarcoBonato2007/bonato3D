#ifndef DRAWING_H
#define DRAWING_H

#include <iostream>
#include <glm/glm.hpp>

uint32_t width, height;
std::vector<uint32_t> frame_buffer; // 32-bit RGBA
std::vector<float> depth_buffer;

// checks how much point c is to the left of line from a to b
inline float leftness(float ax, float ay, float bx, float by, float cx, float cy) {
    // is > 0 if (a, b) is left, = 0 if on, and < 0 if right of the line
    return (bx-ax)*(cy-ay) - (by-ay)*(cx-ax);
}

// Triangle vertices must be counterclockwise
// Each vector is x and y screen (pixel) coordinates along with a normalized [-1, 1] depth (z-value)
// Note: do NOT floor v1, v2, or v3
void drawTriangle(uint32_t color, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
    // bottom-left and top-right coordinates of triangle bounding box
    glm::vec2 bl = glm::floor(glm::min(v1, glm::min(v2, v3)));
    glm::vec2 tr = glm::ceil(glm::max(v1, glm::max(v2, v3)));
    int x_min = std::max(0,(int)bl.x);
    int x_max = std::min((unsigned int)tr.x, width-1);
    int y_min = std::max(0,(int)bl.y);
    int y_max = std::min((unsigned int)tr.y, height-1);

    glm::vec3 e1 = v2-v1;
    glm::vec3 e2 = v3-v2;
    glm::vec3 e3 = v1-v3;
    float xf = x_min+0.5;
    float yf = y_min+0.5;

    glm::vec3 normal = glm::cross(e1, e2);
    float x_increm = -normal.x/normal.z;
    float y_increm = -normal.y/normal.z;
    float start_row_depth = v1.z + (xf-v1.x)*x_increm + (yf-v1.y)*y_increm;

    float leftness1_start = e1.x*(yf-v1.y) - e1.y*(xf-v1.x);
    bool notleftortop1 = !((e1.y == 0 && e1.x < 0) || (e1.y > 0));
    float leftness2_start = e2.x*(yf-v2.y) - e2.y*(xf-v2.x);
    bool notleftortop2 = !((e2.y == 0 && e2.x < 0) || (e2.y > 0));
    float leftness3_start = e3.x*(yf-v3.y) - e3.y*(xf-v3.x);
    bool notleftortop3 = !((e3.y == 0 && e3.x < 0) || (e3.y > 0));

    int i_1 = x_min + y_min*width;

    for (int y=y_min; y<=y_max; y++) {
        float cur_depth = start_row_depth;
        float leftness1 = leftness1_start;
        float leftness2 = leftness2_start;
        float leftness3 = leftness3_start;
        int i = i_1;

        for (int x=x_min; x<=x_max; x++) {                
            if (cur_depth >= depth_buffer[i]
                && cur_depth >= -1
                && cur_depth <= 1
                && (leftness1 > 0 || (leftness1 == 0 && notleftortop1))
                && (leftness2 > 0 || (leftness2 == 0 && notleftortop2))
                && (leftness3 > 0 || (leftness3 == 0 && notleftortop3))
            ) {
                frame_buffer[i] = color;
                depth_buffer[i] = cur_depth;
            }

            cur_depth += x_increm;
            leftness1 -= e1.y;
            leftness2 -= e2.y;
            leftness3 -= e3.y;
            i += 1;
        }

        start_row_depth += y_increm;
        leftness1_start += e1.x;
        leftness2_start += e2.x;
        leftness3_start += e3.x;
        i_1 += width;
    }

}

#endif
