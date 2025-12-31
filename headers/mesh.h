#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <random>

#include "globals.h"
#include "drawing.h"

// shifts a,b,c into b,c,a
inline void lshift(glm::vec4 &a, glm::vec4 &b, glm::vec4 &c) {
    std::swap(a,c);
    std::swap(a,b);
}

// returns the intersection of the near plane (z+w = 0) between points a and b
inline glm::vec4 homogNearIntersect(glm::vec4 a, glm::vec4 b) {
    return a + -(a.w+a.z)/(b.w-a.w+b.z-a.z) * (b-a);

    // a + t(b-a) has z + w = 0
    // so a.w + t(b.w-a.w) + a.z + t(b.z-a.z) = 0
    // so t = -(a.w+a.z)/(b.w-a.w+b.z-a.z)
}

inline glm::vec3 homogToScreen(glm::vec4 homogCoord) {
    glm::vec3 perspdiv = glm::vec3(homogCoord/homogCoord.w);
    glm::vec3 screen_coord = glm::vec3(
        width*(perspdiv.x+1)/2, 
        height*(perspdiv.y+1)/2, 
        perspdiv.z
    );
    return screen_coord;
}

struct Triangle3 {
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;
    uint32_t color;
    
    Triangle3(uint32_t color, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
        this->color = color;
    }
};

struct Triangle4 {
    glm::vec4 v1;
    glm::vec4 v2;
    glm::vec4 v3;
    uint32_t color;
    Triangle4(uint32_t color, glm::vec4 v1, glm::vec4 v2, glm::vec4 v3) {
        this->v1 = v1;
        this->v2 = v2;
        this->v3 = v3;
        this->color = color;
    }
};

struct Mesh {
    std::vector<Triangle3> triangles;

    Mesh(std::vector<glm::vec3> vertices, std::vector<unsigned int> indices) {
        for (int i=0; i<indices.size(); i+=3) {
            uint32_t rand_color = ((rand() & 0x7fffu)<<17 | (rand() & 0x7fffu)<<2 ) | (rand() & 0x7fffu)>>13;
            triangles.push_back(Triangle3(rand_color, vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]));
        }
    };

    void draw(glm::mat4 &mvp) {
        std::vector<Triangle4> processed_triangles = {};
        
        for (Triangle3 t: triangles) {
            glm::vec4 v1 = mvp*glm::vec4(t.v1, 1.0);
            glm::vec4 v2 = mvp*glm::vec4(t.v2, 1.0);
            glm::vec4 v3 = mvp*glm::vec4(t.v3, 1.0);

            // continue if all 3 vertices outside frustum
            if (
                (abs(v1.x) > v1.w || abs(v1.y) > v1.w || abs(v1.z) > v1.w)
                && (abs(v2.x) > v2.w || abs(v2.y) > v2.w || abs(v2.z) > v2.w)
                && (abs(v3.x) > v3.w || abs(v3.y) > v3.w || abs(v3.z) > v3.w)
            ) {
                continue;
            }

            // near clipping
            // lshift is used to reorder vertices while keeping their anticlockwise-ness
            int num_behind = (v1.z < -v1.w) + (v2.z < -v2.w) + (v3.z < -v3.w);
            if (num_behind == 0) {
                processed_triangles.push_back(Triangle4(t.color, v1, v2, v3));
            }
            else if (num_behind == 1) {
                while (v1.z >= -v1.w) { // have v1 be the vertex behind the near plane
                    lshift(v1, v2, v3);
                }
                glm::vec4 intersect1 = homogNearIntersect(v1, v2);
                glm::vec4 intersect2 = homogNearIntersect(v1, v3);

                processed_triangles.push_back(Triangle4(t.color, intersect1, v2, intersect2));
                processed_triangles.push_back(Triangle4(t.color, intersect2, v2, v3));
            }
            else if (num_behind == 2) {
                while (v3.z < -v3.w) { // have v3 be the vertex in front of the near plane
                    lshift(v1, v2, v3);
                }
                glm::vec4 intersect1 = homogNearIntersect(v1, v3);
                glm::vec4 intersect2 = homogNearIntersect(v2, v3);
               
                processed_triangles.push_back(Triangle4(t.color, intersect1, intersect2, v3));
            }
        }
                        
        for (Triangle4 t: processed_triangles) {
            // perform perspective division and ndc to screen coords conversion
            // and call drawTriangle

            drawTriangle(
                t.color, 
                homogToScreen(t.v1), 
                homogToScreen(t.v2), 
                homogToScreen(t.v3)
            );  
        }

    }
};

#endif
