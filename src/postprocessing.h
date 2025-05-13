#ifndef POSTPROCESSING_H
#define POSTPROCESSING_H

#include "objectShapes3D.h"

class PostProcessor
{
public:
    PostProcessor() = default;
    PostProcessor(const int width, const int height)
    {
        init(width, height);
    }

    ~PostProcessor()
    {
        free();
    };

    void init(const int width, const int height)
    {
        // framebuffer initialization
        _width = width;
        _height = height;

        initGenerateFramebuffer();
        initGenerateFramebufferTexture();
        initGenerateRenderbuffer();

        check();

        // generate quad for rendering
        initGenerateQuad();
    }

    void initGenerateFramebuffer()
    {
        unsigned int framebuffer;
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        FBO = framebuffer;
    }

    void initGenerateFramebufferTexture()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // generate texture
        unsigned int textureColorBuffer;
        glGenTextures(1, &textureColorBuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // attach to framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

        TEX = textureColorBuffer;
    }

    void initGenerateRenderbuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        // generate render buffer object
        unsigned int rbo;
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // attach render buffer object
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        RBO = rbo;
    }

    void initGenerateQuad()
    {
        unsigned int quadVAO, quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);

        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Shapes3D::quadVerticesTexCoords), Shapes3D::quadVerticesTexCoords, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));

        VAO = quadVAO;
        VBO = quadVBO;

        glBindVertexArray(0);
    }

    void check() const
    {
        // check
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cout << "ERROR::FRAMEBUFFER Framebuffer is not complete!" << std::endl;
        } else
        {
            std::cout << "Successfully initialized postprocessor!" << std::endl;
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void free() const
    {
        glDeleteTextures(1, &TEX);
        glDeleteRenderbuffers(1, &RBO);
        glDeleteFramebuffers(1, &FBO);
    }

    void render(const Shader& shader) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        // clear buffers
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        shader.setInt("screenTexture", 0);

        glBindVertexArray(VAO);
        glBindTexture(GL_TEXTURE_2D, TEX);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
    }

    // for framebuffer_size_callback()
    void generate(const int width, const int height)
    {
        free();
        init(width, height);
    }


    void enable() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    }

    void disable() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    [[nodiscard]] unsigned int getFramebuffer() const
    {
        return FBO;
    }

    [[nodiscard]] unsigned int getRenderbuffer() const
    {
        return RBO;
    }

    [[nodiscard]] unsigned int getTexture() const
    {
        return TEX;
    }

    [[nodiscard]] unsigned int getVAO() const
    {
        return VAO;
    }

    [[nodiscard]] unsigned int getVBO() const
    {
        return VBO;
    }

private:
    // framebuffer properties
    int _width{0};
    int _height{0};

    unsigned int FBO{0};
    unsigned int RBO{0};
    unsigned int TEX{0};

    // simple quad
    unsigned int VAO{0};
    unsigned int VBO{0};
};

#endif