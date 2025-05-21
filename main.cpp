#include "src/extern/glad.c"

#include "src/opengl/app.h"
#include "src/opengl/fonts.h"

#include "src/paper_loader.h"
#include "src/clusters.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

constexpr unsigned int FONT_SIZE {14};
constexpr bool DEBUG_INFO_ENABLED {true};
// animation tweaks
constexpr float ANIMATION_SPEED {10.f};
// cluster depth for rendering
constexpr int CLUSTER_DEPTH {6};
constexpr float SCALE {5.0};

void wstring2string(std::wstring ws, std::string& s);

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
    // clusterRenderer.generateClusters(paperLoader.getClustersFull());
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
    fontManager.init("data/fonts/noto_sans/NotoSans-Regular.ttf", FONT_SIZE);
    // load fonts shader
    const Shader fontShader{"shaders/builtin/fonts.vert", "shaders/builtin/fonts.frag"};

    // cluster shader
    const Shader clusterShader{"shaders/cluster.vert", "shaders/cluster.frag"};

    std::vector<int> passedClusters{};
    int lastPaperIndex{0};

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

        // render clusters
        clusterShader.use();
        clusterShader.setVec3("CameraPos", app.getCameraPosition());

        const float progress {std::min(static_cast<float>(glfwGetTime() * ANIMATION_SPEED), static_cast<float>(paperLoader.getLastIndex()))};
        const Paper& currentPaper {paperLoader.getPaper(progress)};
        const int currentCluster {paperLoader.getClusterID(currentPaper, CLUSTER_DEPTH)};
        passedClusters.push_back(currentCluster);

        for (int i{lastPaperIndex}; i <= static_cast<int>(progress); ++i)
        {
            const int paperCluster {paperLoader.getClusterID(paperLoader.getPaper(i), CLUSTER_DEPTH)};
            passedClusters.push_back(paperCluster);
        }
        lastPaperIndex = static_cast<int>(progress);

        for (int c {0}; c < std::pow(2, CLUSTER_DEPTH); ++c)
        {
            if (currentCluster == c)
            {
                glLineWidth(10.0f);
                glm::vec3 color = {1.0f, 1.0f, 0.0f};
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                clusterShader.setInt("lighting", 1);
                clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(),
                                              color, CLUSTER_DEPTH,
                                              c);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                clusterShader.setInt("lighting", 0);
                clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(),
                                              color, CLUSTER_DEPTH,
                                              c);
            } else if (std::ranges::find(passedClusters, c) != passedClusters.end())
            {
                glLineWidth(1.0f);
                glm::vec3 color = {0.0f, 0.6f, 0.0f};
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                clusterShader.setInt("lighting", 0);
                clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(),
                                              color, CLUSTER_DEPTH,
                                              c);
            } else
            {
                glLineWidth(2.0f);
                glm::vec3 color = {0.6f, 0.0f, 0.0f};
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                clusterShader.setInt("lighting", 1);
                clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(),
                                              color, CLUSTER_DEPTH,
                                              c);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                clusterShader.setInt("lighting", 0);
                clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(),
                                              color, CLUSTER_DEPTH,
                                              c);
            }
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // ---- debug info and post-processing ---- //

        fontManager.updateProjection(static_cast<float>(app.getWidth()), static_cast<float>(app.getHeight()));

        if (DEBUG_INFO_ENABLED)
        {
            std::stringstream text;
            std::vector<std::string> info;
            // average frame time
            float avgTime {static_cast<int>(app.getAvgFrameTime() * 1000) / 1000.0f};
            text << "Avg. frame time: " << avgTime * 1000.0f<< " ms";
            info.emplace_back(text.str());
            text.str(""); // clear string stream
            // framebuffer size
            text << "Framebuffer size: " << app.getWidth() << " * " << static_cast<float>(app.getHeight());
            info.emplace_back(text.str());
            text.str(""); // clear string stream
            // progress
            int prog {std::min(static_cast<int>(paperLoader.getNumPapers()), static_cast<int>(glfwGetTime() * ANIMATION_SPEED))};
            float percentage {static_cast<float>(glfwGetTime()) * ANIMATION_SPEED / static_cast<float>(paperLoader.getNumPapers())}; // progress as percentage
            percentage = std::min(100.0f, static_cast<float>(static_cast<int>(percentage * 1000.f)) / 10.f); // (n / 10.f = n / 1000.f * 100.f)
            text << "Progress: " << prog << "/" << paperLoader.getNumPapers() << " (" << percentage << "%)";
            info.emplace_back(text.str());
            text.str("");
            // animation speed
            text << "Animation speed: " << ANIMATION_SPEED << " papers/sec";
            info.emplace_back(text.str());
            text.str("");
            // papers size & vertices size
            text << "Paper data size (MB): " << static_cast<int>(paperLoader.getPapersSize()) / 1000000;
            info.emplace_back(text.str());
            text.str("");
            text << "Vertex data size (KB): " << static_cast<int>(paperLoader.getVerticesSize() + sizeof(Shapes3D::cubeVerticesNormals)) / 1000;
            info.emplace_back(text.str());
            text.str("");

            text << "Num. papers explored: " << paperLoader.getLastIndex();
            info.emplace_back(text.str());
            text.str("");
            text << "Num. papers unexplored: " << paperLoader.getNumPapers() - paperLoader.getLastIndex();
            info.emplace_back(text.str());
            text.str("");

            text << "Current cluster depth: " << CLUSTER_DEPTH;
            info.emplace_back(text.str());
            text.str("");

            std::wstring clusterLabel {paperLoader.getClusterLabel(currentPaper, CLUSTER_DEPTH)};
            std::string label;
            wstring2string(clusterLabel, label);
            text << "Current cluster label: " << label;
            info.emplace_back(text.str());
            text.str("");


            for (int i {0}; i < info.size(); ++i)
            {
                fontManager.renderText(fontShader, info[i], 10.0f, static_cast<float>(app.getHeight() - 25 - 15 * i), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            }

            std::string paperTitle;
            wstring2string(currentPaper.title, paperTitle);
            text << "Current paper title: " << paperTitle;
            fontManager.renderText(fontShader, text.str(), 5.0f, 5.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
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

    return EXIT_SUCCESS;
}

void wstring2string(std::wstring ws, std::string& s)
{
    std::size_t len {std::wcstombs(nullptr, ws.c_str(), 0) + 1};
    // buffer to hold multibyte string
    char* buffer {new char[len]};
    wcstombs(buffer, ws.c_str(), len);
    s = buffer;
    delete[] buffer;
}
