#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;

    Vertex(glm::vec3 pos, glm::vec3 normal) {
        this->pos = pos;
        this->normal = normal;
    }
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {
        this->vertices = vertices;
        this->indices = indices;
    };
};

#endif
