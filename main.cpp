#include "src/app.h"

#include "src/model.h"
#include "src/util.h"

int main() {
    // initialization
    App app{640, 640, "OpenGL window"};
    app.enableDepthTesting();
    app.setCameraEnabled(true);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    float planeVertices[] = {
        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
       -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
       -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

        5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
       -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
        5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);

    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    Texture* floorTex {app.loadTexture("data/images/floor.png")};

    Texture* tomato {app.loadTexture("data/images/tomato.png")};
    tomato->activate(0);

    Objects::Cube cube1 {glm::vec3{-1.0f, 0.0f, -1.0f}, {1.0f, 1.0f, 1.0f}};
    Objects::Cube cube2 {glm::vec3{2.0f, 1.0f, 0.0f}, glm::vec3{1.0f, 1.0f, 1.0f}};
    Objects::Cube cube3 {glm::vec3{1.0f, 2.0f, 1.0f}, {0.9f, 0.9f, 0.9f}};

    Shader cubeShader{"shaders/builtin/texCube.vert", "shaders/builtin/texCube.frag"};

    // configure framebuffers
    unsigned int depthFBO;
    glGenFramebuffers(1, &depthFBO);

    const unsigned int SHADOW_WIDTH {1024}, SHADOW_HEIGHT {1024};
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // main loop
    while (!app.shouldClose()) {
        app.handleInput();
        app.clear();

        floorTex->activate(0);
        cubeShader.use();
        cubeShader.setInt("tex", 0);

        cubeShader.setMat4("projection", app.getPerspectiveMatrix());
        cubeShader.setMat4("view", app.getViewMatrix());
        cubeShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        tomato->activate(0);
        cubeShader.use();
        cubeShader.setInt("tex", 0);

        app.drawCube(cube1, cubeShader, CUBE_TEXCOORDS);
        app.drawCube(cube2, cubeShader, CUBE_TEXCOORDS);
        app.drawCube(cube3, cubeShader, CUBE_TEXCOORDS);

        app.tick();
    }

    // clean up
    app.freeTexture(tomato);
    app.close();

    return 0;
}
