//
// Created by AnkenWannePC on 31/01/2025.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "./shader.h"
#include "./shapes.h"

inline float TexRectVertices[] = {
    1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom left
    0.0f, 0.0f, 0.0f, 0.0f, 1.0f // top left
};

inline unsigned int TexRectIndices[]{
    0, 1, 3, // first Triangle
    1, 2, 3 // second Triangle
};

class Texture
{
public:
    unsigned int TEX;

    Texture() = default;

    explicit Texture(const char *path);

    void loadFromFile(const char *path);

    void activate(int slot) const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    [[nodiscard]] int getNumOfChannels() const;

private:
    int _width{}, _height{};
    int _nrChannels{};
};

// similar to SHAPES_H
class TexHandler
{
public:
    TexHandler() = default;

    void init();

    void close() const;

    void drawTexture(const Texture *texture, FRect destination) const;

private:
    unsigned int VAO, VBO, EBO;

    const char *vertShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec2 aTexCoord;\n"
            "out vec2 TexCoord;\n"
            "uniform mat4 model;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "   TexCoord = aTexCoord;\n"
            "}\0";

    const char *fragShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoord;"
            "uniform sampler2D tex;"
            "void main()\n"
            "{\n"
            "   FragColor = texture(tex, TexCoord);\n"
            "}\n\0";

    Shader *texShader{nullptr};
};

#endif //TEXTURE_H
