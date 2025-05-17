#include "src/opengl/app.h"

int main()
{
    // ---- OpenGL ---- //
    // initialization
    App app{640, 640, "OpenGL window"};
    app.enableDepthTesting();
    app.setCameraEnabled(true);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // main loop
    while (!app.shouldClose()) {
        app.handleInput();
        app.clear();

        app.tick();
    }

    // clean up
    app.close();

    return 0;
}
