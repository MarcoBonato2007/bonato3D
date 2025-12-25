#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>

#include <iostream>

#include "mesh.h"

struct Model {
    std::vector<Mesh> meshes;

    Model() { // standard cube model
        meshes = {
            Mesh(
                {
                    Vertex({-0.5f, 0.5f, -2.0f}),
                    Vertex({0.5f, 0.5f, -2.0f}),
                    Vertex({0.5f, -0.5f, -2.0f}),
                    Vertex({-0.5f, -0.5f, -2.0f}),
                    Vertex({-0.5f, 0.5f, -3.0f}),
                    Vertex({0.5f, 0.5f, -3.0f}),
                    Vertex({0.5f, -0.5f, -3.0f}),
                    Vertex({-0.5f, -0.5f, -3.0f})
                }, 
                {
                    2, 1, 0,
                    2, 0, 3,
                    6, 4, 5,
                    6, 7, 4,
                    3, 0, 4,
                    3, 4, 7,
                    6, 5, 1,
                    6, 1, 2,
                    1, 5, 4, 
                    1, 4, 0,
                    2, 3, 7,
                    2, 7, 6
                }
            ),
            Mesh(
                {
                    Vertex({-0.5f, 0.5f, -4.0f}),
                    Vertex({0.5f, 0.5f, -4.0f}),
                    Vertex({0.5f, -0.5f, -4.0f}),
                    Vertex({-0.5f, -0.5f, -4.0f}),
                    Vertex({-0.5f, 0.5f, -5.0f}),
                    Vertex({0.5f, 0.5f, -5.0f}),
                    Vertex({0.5f, -0.5f, -5.0f}),
                    Vertex({-0.5f, -0.5f, -5.0f})
                }, 
                {
                    2, 1, 0,
                    2, 0, 3,
                    6, 4, 5,
                    6, 7, 4,
                    3, 0, 4,
                    3, 4, 7,
                    6, 5, 1,
                    6, 1, 2,
                    1, 5, 4, 
                    1, 4, 0,
                    2, 3, 7,
                    2, 7, 6
                }
            ),
        };
    }
    
    Model(std::string pathToModel) {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(pathToModel, aiProcess_Triangulate);

        meshes = {};

        if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
        {
            std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
            return;
        }
        else {
            processNode(scene->mRootNode, scene);
        }        
    }

    void processNode(aiNode *node, const aiScene *scene) {
        for (int i=0; i<node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            meshes.push_back(processMesh(mesh, scene));			
        }
        for (int i=0; i<node->mNumChildren; i++){
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh *mesh, const aiScene *scene) {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        if (mesh->mFaces->mNumIndices != 3) {
            throw std::logic_error("Error triangulating faces");
        }

        for (int i=0; i<mesh->mNumVertices; i++) {
            glm::vec3 pos = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
            // glm::vec3 normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

            vertices.push_back(Vertex(pos)); // normal
        }

        for (int i=0; i<mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            indices.push_back(face.mIndices[0]);
            indices.push_back(face.mIndices[1]);
            indices.push_back(face.mIndices[2]);
        }

        return Mesh(vertices, indices);
    }

    void draw() {
        for (Mesh m: meshes) {
            m.draw();
        }
    }
};

Model model; // the main model shared by the whole program

#endif