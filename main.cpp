#include "src/extern/glad.c"

#include "src/opengl/app.h"
#include "src/opengl/fonts.h"

#include "src/paper_loader.h"
#include "src/clusters.h"

#include <string>
#include <sstream>
#include <iomanip>

constexpr unsigned int FONT_SIZE {12};
constexpr bool DEBUG_INFO_ENABLED {true};
// animation tweaks
constexpr float ANIMATION_SPEED {700.f};
// cluster depth for rendering
constexpr int CLUSTER_DEPTH {6};
constexpr float SCALE {5.0};

int main()
{
    // load papers
    PaperLoader paperLoader{};
    paperLoader.loadFromFile("data/papers_with_labels.csv");
    paperLoader.generateClusters(); // group papers into clusters

    std::cout << "Loaded papers!\n";

    // ---- OpenGL ---- //
    // initialization
    App app{640, 640, "OpenGL window"};
    app.enableDepthTesting();
    app.setCameraEnabled(true);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glLineWidth(5.0f);
    // glEnable(GL_CULL_FACE);

    // load coordinates from papers
    std::vector<float> paperData;
    paperLoader.getVertices(paperData, SCALE);

    // generate convex hull models from clusters (saved at data/cluster_models/)
    Clusters::ClusterRenderer clusterRenderer{};
    // // generates .obj file` of convex hull for each cluster
    clusterRenderer.generateClusters(paperLoader.getClustersFull(), SCALE);
    clusterRenderer.loadClusters(paperLoader.getClustersFull());

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

    // load papers shader
    const Shader pointShader{"shaders/pointsLighting.vert", "shaders/pointsLighting.frag"};
    // shader.addGeometryShader("shaders/points.geom");

    // post processing shader
    const Shader screenShader{"shaders/builtin/screenShader.vert", "shaders/builtin/screenShader.frag"};
    app.initPostProcessing();

    // initialize font manager
    FontManager fontManager{};
    fontManager.init("data/fonts/opensans/OpenSans-Light.ttf", FONT_SIZE);
    // load fonts shader
    const Shader fontShader{"shaders/builtin/fonts.vert", "shaders/builtin/fonts.frag"};

    // cluster shader
    const Shader clusterShader{"shaders/cluster.vert", "shaders/cluster.frag"};
    Clusters::ClusterModel hull {"data/cluster_models/cluster_2_0.obj"};

    // main loop
    while (!app.shouldClose()) {
        app.handleInput();
        app.enablePostProcessing();
        // ---- do rendering ---- //
        app.clear();

        pointShader.use();
        pointShader.setMat4("projection", app.getPerspectiveMatrix());
        pointShader.setMat4("view", app.getViewMatrix());
        pointShader.setMat4("model", glm::mat4(1.0f));
        pointShader.setVec3("camerapos", app.getCameraPosition());
        pointShader.setFloat("time", static_cast<float>(glfwGetTime() * ANIMATION_SPEED));
        pointShader.setInt("lastIndex", static_cast<int>(paperLoader.getLastIndex()));
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<int>(paperData.size()));

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // clusterShader.use();
        // clusterShader.setMat4("projection", app.getPerspectiveMatrix());
        // clusterShader.setMat4("view", app.getViewMatrix());
        // clusterShader.setMat4("model", glm::mat4(1.0));
        // clusterShader.setVec3("color", glm::vec3{1.0f, 0.0f, 0.0f});
        // hull.render(clusterShader);

        clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(1.0f, 0.5f, 0.0f), 2, 0);
        clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(0.0f, 1.0f, 0.0f), 2, 1);
        clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(1.0f, 0.0f, 0.0f), 2, 2);
        clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(0.0f, 0.0f, 1.0f), 2, 3);
        // clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(1.0f, 0.5f, 0.0f), 6, 4);
        // clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(0.0f, 1.0f, 0.0f), 6, 5);
        // clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(1.0f, 0.0f, 0.0f), 6, 6);
        // clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(), glm::vec3(0.0f, 0.0f, 1.0f), 6, 7);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // ---- debug info and post-processing ---- //

        fontManager.updateProjection(static_cast<float>(app.getWidth()), static_cast<float>(app.getHeight()));

        if (DEBUG_INFO_ENABLED)
        {
            std::stringstream text;
            // average frame time
            float avgTime {static_cast<int>(app.getAvgFrameTime() * 1000) / 1000.0f};
            text << "Avg. frame time: " << avgTime * 1000.0f<< " ms";
            fontManager.renderText(fontShader, text.str(), 10.0f, static_cast<float>(app.getHeight()) - 20.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str(""); // clear string stream
            // framebuffer size
            text << "Framebuffer size: " << app.getWidth() << " * " << static_cast<float>(app.getHeight());
            fontManager.renderText(fontShader, text.str(), 10.0f, static_cast<float>(app.getHeight()) - 35.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str(""); // clear string stream
            // progress
            int progress {std::min(static_cast<int>(paperLoader.getNumPapers()), static_cast<int>(glfwGetTime() * ANIMATION_SPEED))};
            float percentage {static_cast<float>(glfwGetTime()) * ANIMATION_SPEED / static_cast<float>(paperLoader.getNumPapers())}; // progress as percentage
            percentage = std::min(100.0f, static_cast<float>(static_cast<int>(percentage * 1000.f)) / 10.f); // (n / 10.f = n / 1000.f * 100.f)
            text << "Progress: " << progress << "/" << paperLoader.getNumPapers() << " (" << percentage << "%)";
            fontManager.renderText(fontShader, text.str(), 10.0f, static_cast<float>(app.getHeight()) - 50.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str("");
            // animation speed
            text << "Animation speed: " << ANIMATION_SPEED;
            fontManager.renderText(fontShader, text.str(), 10.0f, static_cast<float>(app.getHeight()) - 65.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str("");
            // papers size & vertices size
            text << "Paper data size (MB): " << static_cast<int>(paperLoader.getPapersSize()) / 1000000;
            fontManager.renderText(fontShader, text.str(), 10.0f, static_cast<float>(app.getHeight()) - 80.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str("");
            text << "Vertex data size (KB): " << static_cast<int>(paperLoader.getVerticesSize() + sizeof(Shapes3D::cubeVerticesNormals)) / 1000;
            fontManager.renderText(fontShader, text.str(), 10.0f, static_cast<float>(app.getHeight()) - 95.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str("");
        }

        app.disablePostProcessing();

        app.getPostProcessor()->render(screenShader);

        app.tick();
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
    app.close();

    return 0;
}
