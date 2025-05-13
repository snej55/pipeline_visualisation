//
// Created by Jens Kromdijk on 2/6/25.
//

#ifndef APP_H
#define APP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "./shader.h"
#include "./shapes.h"
#include "./texture.h"
#include "./objects.h"
#include "./camera.h"
#include "./model.h"
#include "./postprocessing.h"

class App
{
public:
    App(int width, int height, const char *title);

    ~App();

    void clear() const;

    void tick();

    [[nodiscard]] bool shouldClose() const;

    [[nodiscard]] GLFWwindow *getWindow() const;

    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    void setTitle(const char *title) const;

    [[nodiscard]] float getDeltaTime() const;

    void setCameraEnabled(bool val);

    [[nodiscard]] bool getCameraEnabled() const;

    void handleInput();

    void close();

    // ---------- Shapes ------------ //
    void drawRect(FRect rect, Color color) const;

    void drawRect(float x, float y, float w, float h, Color color) const;

    void drawRect(float x, float y, float w, float h, int r, int g, int b) const;

    void drawRect(FRect rect, int r, int g, int b) const;

    // ---------- Objects ----------- //
    void drawCube(const Objects::Cube &cube, const Shader &shader, CubeVertexDatOption type = CUBE_VERTICES,
                  float angle = 0.0f, glm::vec3 rotateAxis = {1.0f, 1.0f, 1.0f}) const;

    // void drawCubeNormals(const Objects::Cube& cube, const Shader& shader, float angle = 0.0f, glm::vec3 rotateAxis = {1.0f, 1.0f, 1.0f}) const;

    // ---------- Textures ---------- //
    Texture *loadTexture(const char *path) const;

    void freeTexture(const Texture *texture) const;

    void drawTexture(const Texture *texture, FRect destination) const;

    // ----------- Models ----------- //
    Model *loadModel(const char *path) const;

    void freeModel(const Model *model) const;

    void drawModel(const Model *model, const Shader &shader, glm::vec3 pos, glm::vec3 scale) const;
    void drawModelM(const Model* model, const Shader& shader, glm::mat4 position) const;

    // window callbacks
    void mouse_callback(GLFWwindow *window, double xPosIn, double yPosIn);

    void scroll_callback(GLFWwindow *window, double xOffset, double yOffset);

    void framebuffer_size_callback(GLFWwindow *window, int width, int height);

    // view & perspective matrices getters
    [[nodiscard]] glm::mat4 getPerspectiveMatrix() const;

    [[nodiscard]] glm::mat4 getViewMatrix() const;

    [[nodiscard]] glm::vec3 getCameraPosition() const;

    [[nodiscard]] glm::mat4 getNormalMatrix(glm::mat4 modelMat) const;

    // flags
    void enableDepthTesting();

    void disableDepthTesting();

    void enableDebugHotKeys();

    void disableDebugHotKeys();

    void enableStencilTesting();

    void disableStencilTesting();

    void enableFaceCulling();

    void disableFaceCulling();

    void initPostProcessing();

    void deletePostProcessor();

    void enablePostProcessing();

    void disablePostProcessing();

    [[nodiscard]] bool getDebugHotKeysEnabled() const;

    [[nodiscard]] bool getDepthTestingEnabled() const;

    [[nodiscard]] bool getStencilTestingEnabled() const;

    [[nodiscard]] bool getFaceCullingEnabled() const;

    [[nodiscard]] bool getPostProcessingEnabled() const;

    // post-processing
    [[nodiscard]] PostProcessor* getPostProcessor() const;

private:
    GLFWwindow *_window{nullptr};
    int _width{0};
    int _height{0};

    float _deltaTime{0.0f};
    float _lastFrame{0.0f};

    bool _closed{false};

    Shader *_defaultShader{nullptr};
    Shapes ShapeMan{};
    TexHandler TexHandlerMan{};
    ObjectHandler ObjHandlerMan{};

    PostProcessor* _postProcessor{nullptr};

    // camera stuff
    Camera CameraMan{};
    float _camLastX{};
    float _camLastY{};
    bool _camFirstMouse{true};

    // flags
    bool _cameraEnabled{false};
    bool _debugHotKeysEnabled{false};
    bool _depthTestingEnabled{false};
    bool _stencilTestingEnabled{false};
    bool _faceCullingEnabled{false};
    bool _postProcessingEnabled{false};

    // ----------------------------------------------------------- //

    bool init(int width, int height, const char *title);

    static void win_framebuffer_size_callback(GLFWwindow *window, int width, int height);

    static void win_mouse_callback(GLFWwindow *window, double xPosIn, double yPosIn);

    static void win_scroll_callback(GLFWwindow *window, double xOffset, double yOffset);
};


#endif //APP_H
