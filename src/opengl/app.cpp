//
// Created by Jens Kromdijk on 2/6/25.
//

#include "app.h"

#include <iostream>

App::App(const int width, const int height, const char *title)
// vertex & fragment paths don't matter for default shader
{
    // should only be called once
    if (!init(width, height, title))
    {
        std::cout << "Failed to initialize!" << std::endl;
    } else
    {
        std::cout << "Initialized OpenGL context!" << std::endl;
    }
}

App::~App()
{
    if (!_closed)
    {
        close();
    }
}

bool App::init(const int width, const int height, const char *title)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // for antialiasing
    glfwWindowHint(GLFW_SAMPLES, 4);

    _window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (_window == nullptr)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(_window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cout << "Failed to initialize GLAD!" << std::endl;
        return false;
    }

    _width = width;
    _height = height;
    glViewport(0, 0, width, height);

    glfwSetWindowUserPointer(_window, this);

    glfwSetFramebufferSizeCallback(_window, win_framebuffer_size_callback);
    glfwSetCursorPosCallback(_window, win_mouse_callback);
    glfwSetScrollCallback(_window, win_scroll_callback);

    _defaultShader = new Shader{"default", "default", true};

    ShapeMan.init();
    TexHandlerMan.init();
    ObjHandlerMan.init();

    // camera stuff
    _camLastX = static_cast<float>(_width) / 2.0f;
    _camLastY = static_cast<float>(_height) / 2.0f;

    glEnable(GL_MULTISAMPLE); // for antialiasing

    return true;
}

void App::handleInput()
{
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(_window, true);
    }
    if (_cameraEnabled)
    {
        if (glfwGetKey(_window, GLFW_KEY_W) == GLFW_PRESS)
            CameraMan.processInput(CameraN::CameraMotion::FORWARD, _deltaTime);
        if (glfwGetKey(_window, GLFW_KEY_S) == GLFW_PRESS)
            CameraMan.processInput(CameraN::CameraMotion::BACKWARD, _deltaTime);
        if (glfwGetKey(_window, GLFW_KEY_A) == GLFW_PRESS)
            CameraMan.processInput(CameraN::CameraMotion::LEFT, _deltaTime);
        if (glfwGetKey(_window, GLFW_KEY_D) == GLFW_PRESS)
            CameraMan.processInput(CameraN::CameraMotion::RIGHT, _deltaTime);
    }
}


void App::close()
{
    if (!_closed)
    {
        // cleanup
        _defaultShader->close();
        delete _defaultShader;
        delete _postProcessor;

        ShapeMan.close();

        glfwDestroyWindow(_window);
        glfwTerminate();
        _closed = true;
    }
}

void App::clear() const
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    _defaultShader->use();
}

void App::tick()
{
    glfwSwapBuffers(_window);
    glfwPollEvents();

    const float currentFrame{static_cast<float>(glfwGetTime())};
    _deltaTime = currentFrame - _lastFrame;
    _lastFrame = currentFrame;
}

bool App::shouldClose() const
{
    return glfwWindowShouldClose(_window);
}

GLFWwindow *App::getWindow() const
{
    return _window;
}

int App::getWidth() const
{
    return _width;
}

int App::getHeight() const
{
    return _height;
}

void App::setTitle(const char *title) const
{
    glfwSetWindowTitle(_window, title);
}

float App::getDeltaTime() const
{
    return _deltaTime;
}

void App::setCameraEnabled(const bool val)
{
    _cameraEnabled = val;
    if (_cameraEnabled)
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    else
        glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

bool App::getCameraEnabled() const
{
    return _cameraEnabled;
}

// --------------- Shapes ---------------- //
void App::drawRect(const FRect rect, const Color color) const
{
    ShapeMan.drawRect(rect, color);
}

void App::drawRect(const float x, const float y, const float w, const float h, const Color color) const
{
    ShapeMan.drawRect({x, y, w, h}, color);
}

void App::drawRect(const float x, const float y, const float w, const float h, const int r, const int g,
                   const int b) const
{
    ShapeMan.drawRect({x, y, w, h}, {r, g, b});
}

void App::drawRect(const FRect rect, const int r, const int g, const int b) const
{
    ShapeMan.drawRect(rect, {r, g, b});
}

void App::drawCube(const Objects::Cube &cube, const Shader &shader, const CubeVertexDatOption type, const float angle,
                   const glm::vec3 rotateAxis) const
{
    ObjHandlerMan.drawCube(shader, cube, getPerspectiveMatrix(), getViewMatrix(), type, angle, rotateAxis);
}

// void App::drawCubeNormals(const Objects::Cube& cube, const Shader &shader, const float angle, const glm::vec3 rotateAxis) const {
//     ObjHandlerMan.drawCubeNormals(shader, cube, getPerspectiveMatrix(), getViewMatrix(), angle, rotateAxis);
// }

Texture* App::loadTexture(const char *path) const
{
    Texture *texture{new Texture};
    texture->loadFromFile(path);
    return texture;
}

void App::freeTexture(const Texture *texture) const
{
    delete texture;
}

void App::drawTexture(const Texture *texture, const FRect destination) const
{
    TexHandlerMan.drawTexture(texture, destination);
}

void App::mouse_callback(GLFWwindow *window, const double xPosIn, const double yPosIn)
{
    const float xPos{static_cast<float>(xPosIn)};
    const float yPos{static_cast<float>(yPosIn)};

    if (_camFirstMouse)
    {
        _camLastX = xPos;
        _camLastY = yPos;
        _camFirstMouse = false;
    }

    const float xOffset{xPos - _camLastX};
    const float yOffset{_camLastY - yPos}; // remember to reverse because of reversed y coordinates

    _camLastX = xPos;
    _camLastY = yPos;

    CameraMan.processMouseMovement(xOffset, yOffset);
}

void App::scroll_callback(GLFWwindow *window, double xOffset, double yOffset)
{
    CameraMan.processMouseScroll(static_cast<float>(yOffset));
}

void App::framebuffer_size_callback(GLFWwindow *window, const int width, const int height)
{
    _width = width;
    _height = height;
    glViewport(0, 0, width, height);

    // update postprocessor
    if (_postProcessor != nullptr)
    {
        _postProcessor->generate(width, height);
    }
}

void App::win_mouse_callback(GLFWwindow *window, const double xPosIn, const double yPosIn)
{
    if (App *handler{static_cast<App *>(glfwGetWindowUserPointer(window))})
        handler->mouse_callback(window, xPosIn, yPosIn);
}

void App::win_scroll_callback(GLFWwindow *window, const double xOffset, const double yOffset)
{
    if (App *handler{static_cast<App *>(glfwGetWindowUserPointer(window))})
        handler->scroll_callback(window, xOffset, yOffset);
}

void App::win_framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    if (App *handler{static_cast<App *>(glfwGetWindowUserPointer(window))})
        handler->framebuffer_size_callback(window, width, height);
}


// perspective & view matrices getters
glm::mat4 App::getPerspectiveMatrix() const
{
    return glm::perspective(glm::radians(CameraMan.getZoom()), static_cast<float>(_width) / static_cast<float>(_height),
                            0.1f, 100000.0f);
}

glm::mat4 App::getViewMatrix() const
{
    return CameraMan.getViewMatrix();
}

glm::vec3 App::getCameraPosition() const
{
    return CameraMan.getPosition();
}

glm::mat4 App::getNormalMatrix(glm::mat4 modelMat) const
{
    return glm::transpose(glm::inverse(modelMat));
}


// flags

void App::enableDepthTesting()
{
    _depthTestingEnabled = true;
    glEnable(GL_DEPTH_TEST);
}

void App::disableDepthTesting()
{
    _depthTestingEnabled = false;
    glDisable(GL_DEPTH_TEST);
}

void App::enableDebugHotKeys()
{
    _debugHotKeysEnabled = true;
}

void App::disableDebugHotKeys()
{
    _debugHotKeysEnabled = false;
}

void App::enableStencilTesting()
{
    _stencilTestingEnabled = true;
    glEnable(GL_STENCIL_TEST);
}

void App::disableStencilTesting()
{
    _stencilTestingEnabled = false;
    glDisable(GL_STENCIL_TEST);
}

void App::enableFaceCulling()
{
    _faceCullingEnabled = true;
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void App::disableFaceCulling()
{
    _faceCullingEnabled = false;
    glDisable(GL_CULL_FACE);
}

void App::initPostProcessing()
{
    delete _postProcessor;
    _postProcessor = new PostProcessor{};
    _postProcessor->init(_width, _height);
    _postProcessingEnabled = false;
}

void App::deletePostProcessor()
{
    delete _postProcessor;
    _postProcessor = nullptr;
    _postProcessingEnabled = false;
}

void App::enablePostProcessing()
{
    if (_postProcessor != nullptr)
    {
        _postProcessor->enable();
        _postProcessingEnabled = true;
    }
}

void App::disablePostProcessing()
{
    if (_postProcessor != nullptr)
    {
        _postProcessor->disable();
        _postProcessingEnabled = false;
    }
}

bool App::getDebugHotKeysEnabled() const
{
    return _debugHotKeysEnabled;
}

bool App::getDepthTestingEnabled() const
{
    return _depthTestingEnabled;
}

bool App::getStencilTestingEnabled() const
{
    return _stencilTestingEnabled;
}

bool App::getFaceCullingEnabled() const
{
    return _faceCullingEnabled;
}

bool App::getPostProcessingEnabled() const
{
    return _postProcessingEnabled;
}

PostProcessor *App::getPostProcessor() const
{
    return _postProcessor;
}


// Models
Model *App::loadModel(const char *path) const
{
    Model *model{new Model{path}};
    return model;
}

void App::freeModel(const Model *model) const
{
    std::cout << "Freed model " << model << std::endl;
    delete model;
}

void App::drawModel(const Model *model, const Shader &shader, glm::vec3 pos, glm::vec3 scale) const
{
    glm::mat4 position{1.0f};
    position = glm::scale(position, scale);
    position = glm::translate(position, pos);
    shader.use();
    shader.setMat4("model", position);
    shader.setMat4("view", CameraMan.getViewMatrix());
    shader.setMat4("projection", getPerspectiveMatrix());
    shader.setMat4("normalMat", getNormalMatrix(position));
    model->draw(shader);
}

void App::drawModelM(const Model* model, const Shader& shader, glm::mat4 position) const
{
    shader.use();
    shader.setMat4("model", position);
    shader.setMat4("view", CameraMan.getViewMatrix());
    shader.setMat4("projection", getPerspectiveMatrix());
    shader.setMat4("normalMat", getNormalMatrix(position));
    model->draw(shader);
}