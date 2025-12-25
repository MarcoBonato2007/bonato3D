#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <random>

#include "matrices.h"
#include "player.h"

struct Vertex {
    glm::vec3 pos;
    // glm::vec3 normal;

    Vertex(glm::vec3 pos) { // glm::vec3 normal
        this->pos = pos;
        // this->normal = normal;
    }
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<uint32_t> colors;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
        this->vertices = vertices;
        this->indices = indices;
        
        colors = {};
        for (int i=0; i<indices.size()/3; i++) {
            uint32_t rand_color = ((rand() & 0x7fffu)<<17 | (rand() & 0x7fffu)<<2 ) | (rand() & 0x7fffu)>>13;
            rand_color = ((rand_color << 8) >> 8) + 0xFF000000;
            colors.push_back(((rand() & 0x7fffu)<<17 | (rand() & 0x7fffu)<<2 ) | (rand() & 0x7fffu)>>13);
        }
    };

    void draw() {
        std::vector<glm::vec3> screen_verts = {};
        for (Vertex v: vertices) {
            glm::vec4 v_proj = mvp*glm::vec4(v.pos, 1.0);
            glm::vec3 v_perspdiv = glm::vec3(v_proj/v_proj.w);
            // Should you do the conversion to screen coords directly inside the projection?
            glm::vec3 v_screen = glm::vec3(width*(v_perspdiv.x+1)/2, height*(v_perspdiv.y+1)/2, v_perspdiv.z);
            screen_verts.push_back(v_screen);
        }
        for (int i=0; i<indices.size(); i+=3) {
            // Do clipping into more triangles if vertices are off screen?
            drawTriangle(
                colors[i/3], 
                screen_verts[indices[i]], 
                screen_verts[indices[i+1]], 
                screen_verts[indices[i+2]]
            );
        }

    }
};

#endif
