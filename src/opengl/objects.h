//
// Created by Jens Kromdijk on 16/02/2025.
//

#ifndef OBJECTS_H
#define OBJECTS_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "objectShapes3D.h"

enum CubeVertexDatOption
{
    CUBE_VERTICES,
    CUBE_TEXCOORDS,
    CUBE_NORMALS,
    CUBE_FULL,
};

namespace Objects
{
    struct Cube
    {
        glm::vec3 position;
        glm::vec3 scale;
    };
};

class ObjectHandler
{
public:
    ObjectHandler() = default;

    void init()
    {
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes3D::cubeVertices), Shapes3D::cubeVertices, GL_STATIC_DRAW);

        glBindVertexArray(cubeVAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);

        // ---------------------------------------- //

        glGenVertexArrays(1, &cubeNormalVAO);
        glGenBuffers(1, &cubeNormalVBO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeNormalVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes3D::cubeVerticesNormals), Shapes3D::cubeVerticesNormals,
                     GL_STATIC_DRAW);

        glBindVertexArray(cubeNormalVAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // ---------------------------------------- //

        glGenVertexArrays(1, &cubeTexCoordsVAO);
        glGenBuffers(1, &cubeTexCoordsVBO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeTexCoordsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes3D::cubeVerticesTexCoords), Shapes3D::cubeVerticesTexCoords,
                     GL_STATIC_DRAW);

        glBindVertexArray(cubeTexCoordsVAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // ---------------------------------------- //

        glGenVertexArrays(1, &cubeFullVAO);
        glGenBuffers(1, &cubeFullVBO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeFullVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes3D::cubeVerticesExtended), Shapes3D::cubeVerticesExtended,
                     GL_STATIC_DRAW);

        glBindVertexArray(cubeFullVAO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), reinterpret_cast<void *>(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void drawCube(const Shader &shader, const Objects::Cube &cube, const glm::mat4 &projection, const glm::mat4 &view,
                  CubeVertexDatOption type, const float angle = 0.0f,
                  const glm::vec3 rotateAxis = {1.0f, 1.0f, 1.0f}) const
    {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glm::mat4 model{1.0f};
        model = glm::translate(model, cube.position);
        model = glm::rotate(model, angle, rotateAxis);
        model = glm::scale(model, cube.scale);

        shader.setMat4("model", model);

        const glm::mat3 normalMat{glm::transpose(glm::inverse(model))};
        shader.setMat3("normalMat", normalMat);

        switch (type)
        {
            case CUBE_VERTICES:
                glBindVertexArray(cubeVAO);
                break;
            case CUBE_TEXCOORDS:
                glBindVertexArray(cubeTexCoordsVAO);
                break;
            case CUBE_NORMALS:
                glBindVertexArray(cubeNormalVAO);
                break;
            case CUBE_FULL:
                glBindVertexArray(cubeFullVAO);
                break;
            default:
                glBindVertexArray(cubeVAO);
                break;
        }

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    /*
    void drawCube(const Shader& shader, const Objects::Cube& cube, const glm::mat4& projection, const glm::mat4& view, const float angle = 0.0f, const glm::vec3 rotateAxis = {1.0f, 1.0f, 1.0f}) const {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glm::mat4 model {1.0f};
        model = glm::translate(model, cube.position);
        model = glm::rotate(model, angle, rotateAxis);
        model = glm::scale(model, cube.scale);

        shader.setMat4("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void drawCubeNormals(const Shader& shader, const Objects::Cube& cube, const glm::mat4& projection, const glm::mat4& view, const float angle = 0.0f, const glm::vec3 rotateAxis = {1.0f, 1.0f, 1.0f}) const {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glm::mat4 model {1.0f};
        model = glm::translate(model, cube.position);
        model = glm::rotate(model, angle, rotateAxis);
        model = glm::scale(model, cube.scale);

        shader.setMat4("model", model);

        const glm::mat3 normalMat {glm::transpose(glm::inverse(model))};
        shader.setMat3("normalMat", normalMat);

        glBindVertexArray(cubeNormalVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    void drawCubeFull(const Shader& shader, const Objects::Cube& cube, const glm::mat4& projection, const glm::mat4& view, const float angle = 0.0f, const glm::vec3 rotateAxis = {1.0f, 1.0f, 1.0f}) const {
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        glm::mat4 model {1.0f};
        model = glm::translate(model, cube.position);
        model = glm::rotate(model, angle, rotateAxis);
        model = glm::scale(model, cube.scale);

        shader.setMat4("model", model);

        const glm::mat3 normalMat {glm::transpose(glm::inverse(model))};
        shader.setMat3("normalMat", normalMat);

        glBindVertexArray(cubeNormalVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
*/
private:
    unsigned int cubeVAO, cubeVBO;

    unsigned int cubeNormalVAO, cubeNormalVBO;

    unsigned int cubeTexCoordsVAO, cubeTexCoordsVBO;

    unsigned int cubeFullVAO, cubeFullVBO;
};

#endif //OBJECTS_H
