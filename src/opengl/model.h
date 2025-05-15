//
// Created by Jens Kromdijk on 20/02/2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
// for the loadFromFile function
#include "texture.h"

#include <string>
#include <vector>
#include <iostream>

class Model
{
public:
    explicit Model(const std::string &path);

    void draw(const Shader &shader) const;

private:
    std::vector<Mesh> meshes;
    std::string directory;

    std::vector<MeshN::Tex> loadedTextures; // prevent from loading the same texture twice

    void loadModel(const std::string &path);

    void processNode(const aiNode *node, const aiScene *scene);

    Mesh processMesh(aiMesh *mesh, const aiScene *scene);

    std::vector<MeshN::Tex> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};


#endif //MODEL_H
