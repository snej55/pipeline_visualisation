//
// Created by Jens Kromdijk on 19/02/2025.
//

#include "mesh.h"

Mesh::Mesh(const std::vector<MeshN::Vertex> &verts, const std::vector<unsigned int> &indexes,
           const std::vector<MeshN::Tex> &texes)
    : vertices{verts}, indices{indexes}, textures{texes}
{
    setupMesh();
}

void Mesh::draw(const Shader &shader) const
{
    unsigned int diffuseNr{1};
    unsigned int specularNr{1};
    unsigned int normalNr{1};

    for (unsigned int i{0}; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number;
        std::string name{textures[i].type};
        if (name == "diffuse")
        {
            // NOTE: n++ returns n then increments, ++n increments n then returns it
            if (diffuseNr > 1)
                number = std::to_string(diffuseNr++);
            else
                diffuseNr++;
        } else if (name == "specular")
        {
            if (specularNr > 1)
                number = std::to_string(specularNr++);
            else
                specularNr++;
        } else if (name == "normal")
        {
            if (normalNr > 1)
                number = std::to_string(normalNr++);
            else
                normalNr++;
        }
        shader.setInt(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, nullptr);

    // reset
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    unsigned int meshVAO, meshVBO, meshEBO;
    glGenVertexArrays(1, &meshVAO);
    glGenBuffers(1, &meshVBO);
    glGenBuffers(1, &meshEBO);

    glBindVertexArray(meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(MeshN::Vertex)), vertices.data(),
                 GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)),
                 indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshN::Vertex), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshN::Vertex),
                          reinterpret_cast<void *>(offsetof(MeshN::Vertex, normal)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshN::Vertex),
                          reinterpret_cast<void *>(offsetof(MeshN::Vertex, texCoords)));
    glEnableVertexAttribArray(2);

    // not really necessary but just in case
    glBindVertexArray(0);

    // set actual VAO, VBO & EBO values
    VAO = meshVAO;
    VBO = meshVBO;
    EBO = meshEBO;
}
