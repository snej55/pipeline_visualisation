//
// Created by Jens Kromdijk on 07/02/2025.
//

#ifndef SHAPES_H
#define SHAPES_H

#include <glad/glad.h>
#include <glm/ext/matrix_transform.hpp>

#include "./shader.h"

// should use FRect instead for most use cases
struct IRect
{
    int x{0};
    int y{0};
    int w{0};
    int h{0};
};

struct FRect
{
    float x{0};
    float y{0};
    float w{0};
    float h{0};
};

struct Circle
{
    int x{0};
    int y{0};
    int radius{0};
};

struct Color
{
    int r{0};
    int g{0};
    int b{0};
    int a{255};
};

inline glm::vec3 color2vec(const Color color)
{
    return {
        static_cast<float>(color.r) / 255.0f, static_cast<float>(color.g) / 255.0f, static_cast<float>(color.b) / 255.0f
    };
}

inline float RectVertices[] = {
    1.0f, 0.0f, 0.0f, // top right
    1.0f, -1.0f, 0.0f, // bottom right
    0.0f, -1.0f, 0.0f, // bottom left
    0.0f, 0.0f, 0.0f // top left
};

inline unsigned int RectIndices[]{
    0, 1, 3, // first Triangle
    1, 2, 3 // second Triangle
};

class Shapes
{
public:
    Shapes() = default;

    void init()
    {
        glGenVertexArrays(1, &rectVAO);
        glGenBuffers(1, &rectVBO);
        glGenBuffers(1, &rectEBO);

        glBindVertexArray(rectVAO);

        glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(RectVertices), RectVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RectIndices), RectIndices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void *>(0));
        glEnableVertexAttribArray(0);

        // we can now safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        colorShader = new Shader{true, vertShaderSource, fragShaderSource};
    }

    void close() const
    {
        colorShader->close();
        delete colorShader;
        glDeleteVertexArrays(1, &rectVAO);
        glDeleteBuffers(1, &rectVBO);
        glDeleteBuffers(1, &rectEBO);
    }

    // if you want to draw an IRect for some reason
    void drawIRect(const IRect rect, const Color color) const
    {
        glm::mat4 model{1.0f};
        model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));
        model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f));

        colorShader->use();
        colorShader->setMat4("model", model);
        colorShader->setVec3("shapeColor", color2vec(color));
        glBindVertexArray(rectVAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glBindVertexArray(0);
    }

    void drawRect(const FRect rect, const Color color) const
    {
        glm::mat4 model{1.0f};
        model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));
        model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f));

        colorShader->use();
        colorShader->setMat4("model", model);
        colorShader->setVec3("shapeColor", color2vec(color));
        glBindVertexArray(rectVAO);
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectEBO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        // glBindVertexArray(0);
    }

private:
    const char *vertShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "uniform mat4 model;"
            "void main()\n"
            "{\n"
            "   gl_Position = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";

    const char *fragShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "uniform vec3 shapeColor;"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(shapeColor, 1.0f);\n"
            "}\n\0";

    unsigned int rectVBO{}, rectVAO{}, rectEBO{};

    Shader *colorShader{nullptr};
};

#endif //SHAPES_H
