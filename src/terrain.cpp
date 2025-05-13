//
// Created by Jens Kromdijk on 05/04/25.
//

#include <glad/glad.h>
#include "terrain.h"

#include <glm/glm.hpp>

// #define STB_IMAGE_IMPLEMENTATION
#include <STB/stb_image.h>

#include <iostream>

Terrain::~Terrain()
{
    free();
}


void Terrain::load(const std::string& path)
{
    free();
    loadVertices(path);
    loadIndices();
    loadBuffers();
}

void Terrain::loadVertices(const std::string& path)
{
    // load height map data
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data{stbi_load(path.c_str(), &_width, &_height, &_channels, 0)};
    if (data)
    {
        std::cout << "Loaded heightmap: " << _width << " * " << _height << std::endl;
        // apply scale + shift to height data
        constexpr float yScale{64.0f / 256.0f};
        constexpr float yShift{16.0f};

        // get data
        for (unsigned int x{0}; x < _height; ++x)
        {
            for (unsigned int y{0}; y < _width; ++y)
            {
                // get texel for (x, y) tex coord
                unsigned char* texel{data + (y + _width * x) * _channels};
                // raw height from coordinate
                unsigned char height{*texel};

                // get vertex
                _vertices.push_back(-static_cast<float>(_height) / 2.0f + static_cast<float>(x)); // x
                // apply y scaling and shifting to y coord
                _vertices.push_back(static_cast<float>(static_cast<int>(height)) * yScale - yShift);
                _vertices.push_back(-static_cast<float>(_width) / 2.0f + static_cast<float>(y)); // z
            }
        }

        _num_strips = _height - 1; // each strip is one row
        _num_vertices_per_strip = _width * 2; // one for each side
    } else
    {
        std::cout << "Failed to load texture: " << path << std::endl;
    }

    stbi_image_free(data);
}

void Terrain::loadIndices()
{
    // iterate through each row
    for (unsigned int i{0}; i < _height - 1; ++i)
    {
        // each column in row
        for (unsigned int j{0}; j < _width; ++j)
        {
            // each side of strip
            for (unsigned int k{0}; k < 2; ++k)
            {
                _indices.push_back(j + _width * (i + k)); // get index
            }
        }
    }
}

void Terrain::loadBuffers()
{
    unsigned int terrainVAO, terrainVBO, terrainEBO;

    // vertex array object
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    // vertex buffer object
    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(_vertices.size() * sizeof(float)), _vertices.data(),
                 GL_STATIC_DRAW);

    // vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

    // element buffer object
    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(_indices.size() * sizeof(unsigned int)),
                 _indices.data(), GL_STATIC_DRAW);

    VAO = terrainVAO;
    VBO = terrainVBO;
    EBO = terrainEBO;
}

void Terrain::render(const Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4& model) const
{
    shader.use();
    // shader.setMat4("projection", projection);
    // shader.setMat4("view", view);
    shader.setMat4("model", model);

    // render
    glBindVertexArray(VAO);
    for (unsigned int strip{0}; strip < _num_strips; ++strip)
    {
        glDrawElements(GL_TRIANGLE_STRIP, static_cast<GLsizei>(_num_vertices_per_strip), GL_UNSIGNED_INT,
                       reinterpret_cast<void *>(sizeof(unsigned int) * _num_vertices_per_strip * strip));
    }
    glBindVertexArray(0);
}

void Terrain::free()
{
    // reset
    _width = 0;
    _height = 0;
    _channels = 0;

    _num_strips = 0;
    _num_vertices_per_strip = 0;

    // free that memory
    _vertices.clear();
    _indices.clear();

    // free buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
