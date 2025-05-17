#include "src/opengl/app.h"

int main()
{
    // ---- OpenGL ---- //
    // initialization
    App app{640, 640, "OpenGL window"};
    app.enableDepthTesting();
    // app.setCameraEnabled(true);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::vec2 translations[100];
    int index {0};
    constexpr float offset {0.1f};
    for(int y = -10; y < 10; y += 2)
    {
        for(int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = static_cast<float>(x) / 10.0f + offset;
            translation.y = static_cast<float>(y) / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // quad vertex data to index
    constexpr float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
        0.05f, -0.05f,  0.0f, 1.0f, 0.0f,   
        0.05f,  0.05f,  0.0f, 1.0f, 1.0f		    		
    };


    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // set instance data
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);


    const Shader shader{"shaders/quad.vert", "shaders/quad.frag"};
    shader.use();
    
    // main loop
    while (!app.shouldClose()) {
        app.handleInput();
        app.clear();

        shader.use();
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);  

        app.tick();
    }

    // clean up
    app.close();

    return 0;
}
