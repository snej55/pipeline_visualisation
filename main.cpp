// absolve some linking errors with glad
#include "src/extern/glad.c"

// for rendering
#include "src/opengl/app.h" // window management, events, etc
#include "src/opengl/fonts.h" // text rendering

// cluster and paper management
#include "src/paper_loader.h" // loading papers & clusters
#include "src/clusters.h" // rendering clusters
// small struct for bar charts
#include "src/bar_chart.h"

// standard library stuff
#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdlib>

// constants
constexpr unsigned int FONT_SIZE {8}; // font size of text on screen
constexpr bool DEBUG_INFO_ENABLED {true}; // flag to toggle whether to show text on screen or not
constexpr float ANIMATION_SENSITIVITY{1.f}; // amount to change animation speed by on key press
constexpr float SCALE {5.0}; // scalar value to scale raw coordinates from csv by
constexpr unsigned int MAX_BARS{32}; // maximum amount of bars to display

// cluster depth for rendering
int CLUSTER_DEPTH {6}; // amount of clusters is 2^CLUSTER_DEPTH, so 2:4, 3:8, 4:16, 5:32, 6:64
// animation tweaks
float ANIMATION_SPEED {10.f};

// view mode: default is all shown, unseen hidden is unexplored clusters hidden, and hidden is no clusters
enum VIEW_MODE
{
    CLUSTERS_DEFAULT,
    CLUSTERS_UNSEEN_HIDDEN,
    CLUSTERS_HIDDEN,
};
// view mode (global for callbacks)
int viewMode{CLUSTERS_DEFAULT};

// convert from wstring (wide-string) to regular standard string
void wstring2string(const std::wstring& ws, std::string& s);
// glfw keycallback to handle interactivity
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
// gets string format for VIEW_MODe enum
std::string getViewMode();

int main()
{
    // load papers
    PaperLoader paperLoader{};
    paperLoader.loadFromFile("data/papers_with_labels.csv", SCALE);
    paperLoader.generateClusters(); // group papers into clusters

    std::cout << "Loaded papers!\n";

    // ---- OpenGL ---- //
    // initialize opengl wrapper
    App app{640, 640, "OpenGL window"};
    // for keyboard interactivity
    glfwSetKeyCallback(app.getWindow(), key_callback);
    app.enableDepthTesting(); // IMPORTANT
    // first person camera
    app.setCameraEnabled(true);
    // configure global opengl state
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // for text & cluster rendering
    // glLineWidth(5.0f);
    // glEnable(GL_CULL_FACE);

    // load coordinates from papers
    std::vector<float> paperData;
    paperLoader.getVertices(paperData);

    // generate convex hull models from clusters (saved at data/cluster_models/)
    Clusters::ClusterRenderer clusterRenderer{};
    // // generates .obj file of convex hull for each cluster
    // clusterRenderer.generateClusters(paperLoader.getClustersFull());
    clusterRenderer.loadClusters(paperLoader.getClustersFull()); // load convex hulls

    // generate vbo for paper instances (offset xyz, included flag, counter)
    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(paperData.size() * sizeof(paperData[0])), paperData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vertex array and vertex buffer for paper cubes
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

    // post-processing shader
    const Shader screenShader{"shaders/builtin/screenShader.vert", "shaders/builtin/screenShader.frag"};
    app.initPostProcessing();

    // initialize font manager
    FontManager fontManager{};
    fontManager.init("data/fonts/Acme 9 Regular Bold Xtnd.ttf", FONT_SIZE);
    // load fonts shader
    const Shader fontShader{"shaders/builtin/fonts.vert", "shaders/builtin/fonts.frag"};

    // cluster shader
    const Shader clusterShader{"shaders/cluster.vert", "shaders/cluster.frag"};

    std::vector<int> passedClusters{};
    int lastPaperIndex{0};

    std::cout << "Successfully initialized!\n";

    // data for bar chart
    std::map<int, Bar> bars{};
    std::map paperClusters{paperLoader.getClusters(CLUSTER_DEPTH)};
    // create a bar for each cluster
    for (int b {0}; b < std::pow(2, CLUSTER_DEPTH); ++b)
    {
        std::string name;
        wstring2string(paperClusters[b].label, name);
        bars[b] = Bar{
            0.0f,
            0,
            b,
            std::move(name)
        };
    }

    // counter to keep track of num. papers
    int numPapers{0};
    float animationProgress{0.f};

    // main loop
    while (!app.shouldClose())
    {
        // refresh keyboard events
        app.handleInput();
        app.enablePostProcessing(); // write to framebuffer
        // ---- do rendering ---- //
        app.clear(); // clear depth and color buffers (+stencil but that's not used)

        // ------------------------ //

        // Render the points (cubes)
        // The cubes are rendered instanced to improve performance
        pointShader.use();
        pointShader.setMat4("projection", app.getPerspectiveMatrix());
        pointShader.setMat4("view", app.getViewMatrix());
        pointShader.setMat4("model", glm::mat4(1.0f));
        pointShader.setVec3("camerapos", app.getCameraPosition());
        pointShader.setFloat("time", animationProgress);
        pointShader.setInt("lastIndex", static_cast<int>(paperLoader.getLastIndex()));
        glBindVertexArray(VAO);
        // there are five pieces of data per instance (5 * sizeof(float)), so number of instances = paperData.size() / 5
        // not paperData.size()
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<int>(paperData.size() / 5));

        // ------------------------ //

        // update progress, lastPaperIndex, currentCluster & currentPaper
        // progress of animation
        const float progress {std::min(animationProgress, static_cast<float>(paperLoader.getLastIndex()))};
        // current paper animation is at
        const Paper& currentPaper {paperLoader.getPaper(progress)};
        // current cluster the current paper is located in
        const int currentCluster {paperLoader.getClusterID(currentPaper, CLUSTER_DEPTH)};
        // update passed clusters
        if (std::ranges::find(passedClusters, currentCluster) == passedClusters.end())
        {
            passedClusters.push_back(currentCluster);
        }
        // update all the clusters animation skipped (animation speed > 1 paper/sec)
        for (int i{lastPaperIndex}; i < static_cast<int>(progress); ++i)
        {
            const Paper& paperTemp {paperLoader.getPaper(static_cast<float>(i))};
            const int paperCluster {paperLoader.getClusterID(paperTemp, CLUSTER_DEPTH)};
            if (std::ranges::find(passedClusters, paperCluster) == passedClusters.end())
            {
                passedClusters.push_back(paperCluster);
            }
            // add clusters to bar chart
            ++bars[paperCluster].numPapers;
            ++numPapers;
        }
        lastPaperIndex = static_cast<int>(progress);

        // ---- Render clusters ---- //


        /* Clusters are transparent, so order is:
         * 1. Render opaque objects (points/papers/cubes)
         * 2. Sort transparent objects (clusters)
         * 3. Render transparent objects in order (blending works now)
         */

        clusterShader.use();
        clusterShader.setVec3("CameraPos", app.getCameraPosition());

        // iterate through clusters (n = 2^CLUSTER_DEPTH)
        std::map<float, std::pair<int, glm::vec3>> sortedClusters{};
        for (int c {0}; c < std::pow(2, CLUSTER_DEPTH); ++c)
        {
            // color & idx is info needed for rendering
            glm::vec3 color;
            float distance; // info needed for sorting
            // get cluster data for position
            const Clusters::ClusterData* clusterData {clusterRenderer.getClusterData(CLUSTER_DEPTH, c)};
            distance = glm::length(app.getCameraPosition() - clusterData->position);
            if (currentCluster == c)
            {
                color = {0.9f, 1.0f, 0.0f};
            } else if (std::ranges::find(passedClusters, c) != passedClusters.end())
            {
                color = {0.0f, 0.9f, 1.0f};
            } else
            {
                if (viewMode == CLUSTERS_DEFAULT)
                {
                    color = {0.4f, 0.3f, 0.4f};
                } else
                {
                    color = {0.0f, 0.0f, 0.0f};
                }
            }
            if (viewMode == CLUSTERS_HIDDEN)
            {
                color = {0.0f, 0.0f, 0.0f};
            }
            // add to map to be sorted
            sortedClusters[distance] = std::make_pair(c, color);
        }

        // sort & render the clusters
        clusterShader.use();
        clusterShader.setVec3("CameraPos", app.getCameraPosition());
        clusterShader.setInt("lighting", 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        for (std::map<float, std::pair<int, glm::vec3>>::reverse_iterator it {sortedClusters.rbegin()}; it != sortedClusters.rend(); ++it)
        {
            clusterRenderer.renderCluster(clusterShader, app.getPerspectiveMatrix(), app.getViewMatrix(),
                it->second.second, CLUSTER_DEPTH, it->second.first);
        }

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Calculate bar chart of percentages to render //
        // update font manager first
        fontManager.updateProjection(static_cast<float>(app.getWidth()), static_cast<float>(app.getHeight()));

        std::vector<std::pair<int, Bar>> sortedBars{};
        for (const std::pair<const int, Bar>& bar : bars)
        {
            sortedBars.emplace_back(bar);
        }

        std::ranges::sort(sortedBars, [](const std::pair<int, Bar>& bar1, const std::pair<int, Bar>& bar2)
        {
            return bar1.second.numPapers > bar2.second.numPapers;
        });

        int numBars{0};
        std::stringstream ss; // for percentages & cluster labesl
        for (const std::pair<int, Bar>& bar : sortedBars)
        {
            // render bar
            const float percentage {static_cast<float>(bar.second.numPapers) / progress};
            FRect rect {40.f, static_cast<float>(app.getHeight() - 205 - numBars * 17), 1.f + 200.f * percentage, 14.f};
            app.drawRect({
                             rect.x * 2.f / static_cast<float>(app.getWidth()) - 1.f, rect.y * 2.f / static_cast<float>(app.getHeight()) - 1.f,
                             rect.w * 2.f / static_cast<float>(app.getWidth()), rect.h * 2.f / static_cast<float>(app.getHeight())
                         }, {255, 255, 255});

            // render text
            ss << static_cast<int>(percentage * 100.f) << "%";
            fontManager.renderText(fontShader, ss.str(), 5.f, static_cast<float>(app.getHeight() - 217 - numBars * 17), 1.0f, glm::vec3{1.0f});
            ss.str("");

            ss << bar.second.name;
            fontManager.renderText(fontShader, ss.str(), 45.f + 200.f * percentage, static_cast<float>(app.getHeight() - 217 - numBars * 17), 1.0f, glm::vec3{1.0f});
            ss.str("");

            // cap number of bars
            ++numBars;
            if (numBars > MAX_BARS)
            {
                break;
            }
        }

        // ------------------------ //

        // ---- debug info and post-processing ---- //

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
            int prog {std::min(static_cast<int>(paperLoader.getNumPapers()), static_cast<int>(animationProgress))};
            float percentage {animationProgress / static_cast<float>(paperLoader.getNumPapers())}; // progress as percentage
            percentage = std::min(100.0f, static_cast<float>(static_cast<int>(percentage * 1000.f)) / 10.f); // (n / 10.f = n / 1000.f * 100.f)
            text << "Raw Progress: " << prog << "/" << paperLoader.getNumPapers() << " (" << percentage << "%)";
            info.emplace_back(text.str());
            text.str("");
            prog = std::min(static_cast<int>(paperLoader.getLastIndex()), prog);
            percentage = animationProgress / static_cast<float>(paperLoader.getLastIndex()); // progress as percentage
            percentage = std::min(100.0f, static_cast<float>(static_cast<int>(percentage * 1000.f)) / 10.f); // (n / 10.f = n / 1000.f * 100.f)
            text << "Progress: " << prog << "/" << paperLoader.getLastIndex() << " (" << percentage << "%)";// n." << animationProgress;
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

            text << "Current cluster ID: " << paperLoader.getClusterID(currentPaper, CLUSTER_DEPTH);
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

            text << "View mode: " << getViewMode();
            fontManager.renderText(fontShader, text.str(), 5.0f, 20.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str("");

            glm::vec3 cameraPos {app.getCameraPosition()};
            text << "Camera Position: " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z;
            fontManager.renderText(fontShader, text.str(), 5.0f, 35.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
            text.str("");
        }

        // ------------------------ //

        app.disablePostProcessing();

        app.getPostProcessor()->render(screenShader);

        // update buffers and stuff
        app.tick();
        // animation is updated at constant speed
        animationProgress += ANIMATION_SPEED * app.getDeltaTime();
        // clamp animation progress to avoid user messing it up
        animationProgress = std::max(0.0f, std::min(static_cast<float>(paperLoader.getNumPapers() - 1), animationProgress));
    }

    // clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
    app.close();

    return EXIT_SUCCESS;
}

void wstring2string(const std::wstring& ws, std::string& s)
{
    const std::size_t len {std::wcstombs(nullptr, ws.c_str(), 0) + 1};
    // buffer to hold multibyte string
    char* buffer {new char[len]};
    wcstombs(buffer, ws.c_str(), len);
    s = buffer;
    delete[] buffer;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        viewMode = (viewMode + 1) % 3;
    }
    // increase animation speed
    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        ANIMATION_SPEED += ANIMATION_SENSITIVITY;
    }

    // decrease animation speed
    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        ANIMATION_SPEED -= ANIMATION_SENSITIVITY;
        // limit to positive values for now
        ANIMATION_SPEED = std::max(0.f, ANIMATION_SPEED); // TODO: Fix this
    }
}

std::string getViewMode()
{
    switch (viewMode)
    {
        case CLUSTERS_DEFAULT:
            return "CLUSTERS_DEFAULT";
        case CLUSTERS_UNSEEN_HIDDEN:
            return "CLUSTERS_UNSEEN_HIDDEN";
        case CLUSTERS_HIDDEN:
            return "CLUSTERS_HIDDEN";
        default:
            return "CLUSTERS_DEFAULT";
    };
}