//
// Created by Jens Kromdijk on 05/04/25.
//

#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "shader.h"

class Terrain
{
public:
    Terrain() = default;

    ~Terrain();

    void load(const std::string &path);

    void loadVertices(const std::string &path);

    void loadIndices();

    void loadBuffers();

    void render(const Shader &shader, const glm::mat4 &projection, const glm::mat4 &view, const glm::mat4& model) const;

    void free();

private:
    int _width{0};
    int _height{0};
    int _channels{0};

    unsigned int VAO{0};
    unsigned int VBO{0};
    unsigned int EBO{0};

    std::vector<float> _vertices{}; // stores vertices separately
    std::vector<unsigned int> _indices{}; // stores indices for EBO

    // for rendering
    unsigned int _num_strips{0};
    unsigned int _num_vertices_per_strip{0};
};


#endif //TERRAIN_H
