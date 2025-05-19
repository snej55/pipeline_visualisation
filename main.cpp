#include "src/opengl/app.h"
#include "src/opengl/fonts.h"

#include "src/paper_loader.h"

#include <string>
#include <sstream>
#include <iomanip>

constexpr unsigned int FONT_SIZE {12};
constexpr bool DEBUG_INFO_ENABLED {true};

int main()
{
    // load papers
    PaperLoader paperLoader{};
    paperLoader.loadFromFile("data/papers_with_labels.csv");

    std::cout << "Loaded papers!\n";

    // ---- OpenGL ---- //
    // initialization
    App app{640, 640, "OpenGL window"};
    app.enableDepthTesting();
    app.setCameraEnabled(true);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_CULL_FACE);

    // load coordinates from paper
    std::vector<float> paperData;
    paperLoader.getVertices(paperData, 4.0);

    // actual model vertices
    std::vector<float> vertices {0.0f, 0.0f, 0.0f};

    // generate vbo for paper instances
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(paperData.size() * sizeof(paperData[0])), paperData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vertex array and vertex buffer for vertices
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(Shapes3D::cubeVerticesNormals)), Shapes3D::cubeVerticesNormals, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // set instance data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(4 * sizeof(float)));
    glVertexAttribDivisor(2, 1); // update this index every 1th instance
    glVertexAttribDivisor(3, 1); // "" ""
    glVertexAttribDivisor(4, 1); // "" ""

    // load shader
    const Shader shader{"shaders/pointsLighting.vert", "shaders/pointsLighting.frag"};
    // shader.addGeometryShader("shaders/points.geom");

    const Shader screenShader{"shaders/builtin/screenShader.vert", "shaders/builtin/screenShader.frag"};
    app.initPostProcessing();

    // initialize font manager
    FontManager fontManager{};
    fontManager.init("data/fonts/opensans/OpenSans-Light.ttf", FONT_SIZE);
    // load fonts shader
    const Shader fontShader{"shaders/builtin/fonts.vert", "shaders/builtin/fonts.frag"};

    // main loop
    while (!app.shouldClose()) {
        app.handleInput();
        app.enablePostProcessing();
        app.clear();

        shader.use();
        shader.setMat4("projection", app.getPerspectiveMatrix());
        shader.setMat4("view", app.getViewMatrix());
        shader.setMat4("model", glm::mat4(1.0f));
        shader.setVec3("camerapos", app.getCameraPosition());
        shader.setFloat("time", static_cast<float>(glfwGetTime()));
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, paperData.size());

        fontManager.updateProjection(app.getWidth(), app.getHeight());

        if (DEBUG_INFO_ENABLED)
        {
            std::stringstream text;
            text << "Framebuffer size: " << app.getWidth() << " * " << app.getHeight();
            fontManager.renderText(fontShader, text.str(), 10.0f, app.getHeight() - 35.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str(""); // clear string stream
            float avgTime {static_cast<int>(app.getAvgFrameTime() * 1000) / 1000.0f};
            text << "Avg. frame time: " << avgTime * 1000.0f<< " ms";
            fontManager.renderText(fontShader, text.str(), 10.0f, app.getHeight() - 20.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        app.disablePostProcessing();

        app.getPostProcessor()->render(screenShader);

        app.tick();
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    app.close();

    return 0;
}
