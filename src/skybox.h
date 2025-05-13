#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include "cubemap.h"
#include "objectShapes3D.h"
#include "shader.h"

class Skybox
{
public:
    Skybox() = default;

    explicit Skybox(const std::string &dir)
    {
        load(dir);
    }

    void load(const std::string &dir)
    {
        // load skybox texture
        std::vector<std::string> files;
        files.push_back(dir + "/right.jpg");
        files.push_back(dir + "/left.jpg");
        files.push_back(dir + "/top.jpg");
        files.push_back(dir + "/bottom.jpg");
        files.push_back(dir + "/front.jpg");
        files.push_back(dir + "/back.jpg");

        _tex = CubeMap_N::loadCubeMap(files);
        _loaded = true;

        // load vertex data
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes3D::skyboxVertices), Shapes3D::skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void *>(0));

        glBindVertexArray(0);
    }

    void render(const Shader &shader, glm::mat4 view, const glm::mat4 &proj) const
    {
        // truncate translation so skybox never moves
        view = glm::mat4(glm::mat3(view));

        glDepthFunc(GL_LEQUAL);
        shader.use();

        shader.setMat4("view", view);
        shader.setMat4("projection", proj);
        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);
    }

    [[nodiscard]] GLuint getVAO() const
    {
        return VAO;
    }

    [[nodiscard]] GLuint getVBO() const
    {
        return VBO;
    }

    [[nodiscard]] GLuint getTex() const
    {
        return _tex;
    }

private:
    unsigned int _tex{0};
    bool _loaded{false};

    unsigned int VAO{}, VBO{};
};

#endif
