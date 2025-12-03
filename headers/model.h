#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"

struct Model {
    std::vector<Mesh> meshes;

    Model() { // test model
        
    }
    
    // Model(std::string pathToModel) {
    //     Assimp::Importer importer;
    //     const aiScene *scene = importer.ReadFile(pathToModel, aiProcess_Triangulate);

    //     meshes = {};

    //     if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    //     {
    //         std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
    //         return;
    //     }
    //     else {
    //         for (int i=0; i<scene->mNumMeshes; i++) {
    //             aiMesh *mesh = scene->mMeshes[i];
    //             meshes.push_back(
    //                 Mesh(
    //                     mesh->mNumVertices, 
    //                     mesh->mNumFaces, 
    //                     mesh->mFaces->mNumIndices, 
    //                     mesh->mVertices, 
    //                     mesh->mFaces
    //                 )
    //             );
    //         }
    //     }
        
    // }

};

Model model; // the main model shared by the whole program

#endif