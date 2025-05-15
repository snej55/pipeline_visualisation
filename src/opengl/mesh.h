//
// Created by Jens Kromdijk on 19/02/2025.
//

#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>

#include "shader.h"

namespace MeshN
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    // different to Texture::Texture (kinda)
    struct Tex
    {
        unsigned int id;
        std::string type;
        std::string path;
    };
}

class Mesh
{
public:
    std::vector<MeshN::Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<MeshN::Tex> textures;

    Mesh(const std::vector<MeshN::Vertex> &verts, const std::vector<unsigned int> &indexes,
         const std::vector<MeshN::Tex> &texes);

    void draw(const Shader &shader) const;

private:
    unsigned int VAO{}, VBO{}, EBO{};

    void setupMesh();
};


#endif //MESH_H
