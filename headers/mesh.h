#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <random>

#include "matrices.h"
#include "player.h"

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

inline void showVec3(glm::vec3 a) {
    std::cout << a.x << " " << a.y << " " << a.z << std::endl;
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
            rand_color = ((rand_color << 8) >> 8) + 0xFF000000; // ensure opaque

            triangles.push_back(Triangle3(rand_color, vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]));
        }
    };

    void draw() {
        std::vector<Triangle4> processed_triangles = {};
        
        for (Triangle3 t: triangles) {
            glm::vec4 v1 = mvp*glm::vec4(t.v1, 1.0);
            glm::vec4 v2 = mvp*glm::vec4(t.v2, 1.0);
            glm::vec4 v3 = mvp*glm::vec4(t.v3, 1.0);

            // check that at least one vertex is inside the frustum
            if (
                (abs(v1.x) > v1.w || abs(v1.y) > v1.w || abs(v1.z) > v1.w)
                && (abs(v2.x) > v2.w || abs(v2.y) > v2.w || abs(v2.z) > v2.w)
                && (abs(v3.x) > v3.w || abs(v3.y) > v3.w || abs(v3.z) > v3.w)
            ) {
                continue;
            }

            // reorder to have those behind near plane first
            // BUT: keep original order (i.e. v1v2v3 or v2v3v1 or v3v1v2)
            int num_behind = (v1.z < -v1.w) + (v2.z < -v2.w) + (v3.z < -v3.w);
            if (num_behind == 0) {
                processed_triangles.push_back(Triangle4(t.color, v1, v2, v3));
            }
            else if (num_behind == 1) {
                while (v1.z >= -v1.w) {
                    lshift(v1, v2, v3);
                }
                glm::vec4 intersect1 = homogNearIntersect(v1, v2);
                glm::vec4 intersect2 = homogNearIntersect(v1, v3);

                processed_triangles.push_back(Triangle4(t.color, intersect1, v2, intersect2));
                processed_triangles.push_back(Triangle4(t.color, intersect2, v2, v3));
            }
            else if (num_behind == 2) {
                while (v3.z < -v3.w) {
                    lshift(v1, v2, v3);
                }
                glm::vec4 intersect1 = homogNearIntersect(v1, v3);
                glm::vec4 intersect2 = homogNearIntersect(v2, v3);
               
                processed_triangles.push_back(Triangle4(t.color, intersect1, intersect2, v3));
            }
        }
                        
        for (Triangle4 t: processed_triangles) {
            glm::vec3 v1 = glm::vec3(t.v1/t.v1.w);
            v1 = glm::vec3(width*(v1.x+1)/2, height*(v1.y+1)/2, v1.z);

            glm::vec3 v2 = glm::vec3(t.v2/t.v2.w);
            v2 = glm::vec3(width*(v2.x+1)/2, height*(v2.y+1)/2, v2.z);
            
            glm::vec3 v3 = glm::vec3(t.v3/t.v3.w);
            v3 = glm::vec3(width*(v3.x+1)/2, height*(v3.y+1)/2, v3.z);

            drawTriangle(t.color, v1, v2, v3);  
        }

    }
};

#endif
