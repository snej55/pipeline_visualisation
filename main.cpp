#include "src/opengl/app.h"

#include "src/paper_loader.h"

int main()
{
    // load papers
    PaperLoader paperLoader{};
    paperLoader.loadFromFile("data/papers_with_labels.csv");

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
