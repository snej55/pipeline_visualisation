//
// Created by Jens Kromdijk on 20/02/2025.
//

#include "model.h"

#include <iostream>
#include <ostream>


Model::Model(const std::string &path)
{
    std::cout << "loading model '" << path << "'..." << std::endl;
    loadModel(path);
    std::cout << "loaded model '" << path << "'!" << std::endl;
}

void Model::draw(const Shader &shader) const
{
    for (unsigned int i{0}; i < meshes.size(); ++i)
    {
        meshes[i].draw(shader);
    }
}

void Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene *scene{
        importer.ReadFile(
            path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace)
    };
    // error handling
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        // if it isn't zero
        std::cout << "ERROR::ASSIMP:" << importer.GetErrorString() << std::endl;
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
}

void Model::processNode(const aiNode *node, const aiScene *scene)
{
    for (unsigned int i{0}; i < node->mNumMeshes; ++i)
    {
        // node->mMeshes is a list of indices for scene->mMeshes
        aiMesh *mesh{scene->mMeshes[node->mMeshes[i]]};

        meshes.push_back(processMesh(mesh, scene));
    }
    // repeat recursively for all children
    for (unsigned int i{0}; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<MeshN::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshN::Tex> textures;

    for (unsigned int i{0}; i < mesh->mNumVertices; ++i)
    {
        MeshN::Vertex vertex;
        // process vertex
        // retrieve vertex positions
        glm::vec3 vPos;
        vPos.x = mesh->mVertices[i].x;
        vPos.y = mesh->mVertices[i].y;
        vPos.z = mesh->mVertices[i].z;
        vertex.position = vPos;
        // same for normals
        glm::vec3 vNormal;
        vNormal.x = mesh->mNormals[i].x;
        vNormal.y = mesh->mNormals[i].y;
        vNormal.z = mesh->mNormals[i].z;
        vertex.normal = vNormal;
        // texture coordinates if mesh has them
        // first check if it actually has texture coordinates
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vTexCoords;
            // assimp allows up to 8 tex coords, but we only care about the first set
            vTexCoords.x = mesh->mTextureCoords[0][i].x;
            vTexCoords.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vTexCoords;
        } else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }

    // indices
    for (unsigned int i{0}; i < mesh->mNumFaces; ++i)
    {
        aiFace face{mesh->mFaces[i]};
        // each face usually has like 3 indices or something
        for (unsigned int j{0}; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // materials
    aiMaterial *material{scene->mMaterials[mesh->mMaterialIndex]};

    // in the shaders we'll call the diffuse and specular textures "diffuse" and "specular" respectively
    std::vector<MeshN::Tex> diffuseMaps{loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse")};
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    std::vector<MeshN::Tex> specularMaps{loadMaterialTextures(material, aiTextureType_SPECULAR, "specular")};
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    std::vector<MeshN::Tex> normalMaps{loadMaterialTextures(material, aiTextureType_HEIGHT, "normal")};
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    return Mesh{vertices, indices, textures};
}

std::vector<MeshN::Tex> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<MeshN::Tex> textures;
    for (unsigned int i{0}; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip{false};
        // check if we haven't already loaded this texture
        for (unsigned int j{0}; j < loadedTextures.size(); ++j)
        {
            // compare
            if (std::strcmp(loadedTextures[j].path.data(), str.C_Str()) == 0)
            {
                // we found something
                // push back THAT texture instead
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }

        // if we didn't find any matches
        if (!skip)
        {
            // some string manip witchcraft
            std::string filename{str.C_Str()};
            filename += directory + '/' + filename;
            // temporary *other* texture to loadFromFile (called in constructor)
            const Texture temp{filename.c_str()};
            // convert back to Mesh namespace Tex struct
            MeshN::Tex texture;
            texture.id = temp.TEX;
            texture.type = typeName;
            texture.path = str.C_Str();
            loadedTextures.push_back(texture);
            textures.push_back(texture);
        }
    }
    return textures;
}
