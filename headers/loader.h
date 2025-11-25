#ifndef LOADER_H
#define LOADER_H

#include <string>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Contains functions to perform model loading
// Done using assimp

struct Mesh {
    int numVertices, numFaces, numIndices;
    aiVector3D *vertices;
    aiFace *faces;

    Mesh(unsigned int pNumVertices, unsigned int pNumFaces, unsigned int pNumIndices, aiVector3D *pVertices, aiFace *pFaces) {
        numVertices = pNumVertices;
        numFaces = pNumFaces;
        numIndices = pNumIndices;
        vertices = pVertices;
        faces = pFaces;
    };
};

void load_model(std::string path) {
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate);

    std::vector<Mesh> meshes = {};

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    else {
        for (int i=0; i<scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];
            meshes.push_back(
                Mesh(
                    mesh->mNumVertices, 
                    mesh->mNumFaces, 
                    mesh->mFaces->mNumIndices, 
                    mesh->mVertices, 
                    mesh->mFaces
                )
            );
        }
    }

    

    // TODO: model loading
}

#endif

