#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <glm/glm.hpp>

struct Triangle {
    glm::vec4 vert1;
    glm::vec4 vert2;
    glm::vec4 vert3;
    glm::vec4 normal;
    unsigned char red;
    unsigned char green;
    unsigned char blue;

    Triangle(
        glm::vec4 vert1_, 
        glm::vec4 vert2_, 
        glm::vec4 vert3_, 
        glm::vec4 normal_, 
        unsigned char red_,
        unsigned char green_,
        unsigned char blue_
    ) {
        vert1 = vert1_;
        vert2 = vert2_;
        vert3 = vert3_;
        normal = normal_;
        red = red_;
        green = green_;
        blue = blue_;
    }

};

#endif
