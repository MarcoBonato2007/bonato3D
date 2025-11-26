#ifndef MESH_H
#define MESH_H

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

#endif
