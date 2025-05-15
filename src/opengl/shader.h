//
// Created by Jens Kromdijk on 31/01/2025.
//

#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

#include <string>

namespace Shaders
{
    inline const char *defaultVertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";

    inline const char *defaultFragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);\n"
            "}\n\0";
}


class Shader
{
public:
    unsigned int ID;

    Shader(const char *vertexPath, const char *fragmentPath, bool defaultShader = false);

    Shader(bool source, const char *vert_shader_source, const char *frag_shader_source);

    // optional geometry shader
    void addGeometryShader(const char* geometryPath) const;

    // activate
    void use() const;

    void close() const;

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};


#endif //SHADER_H
