#include "Renderer.h"
#include <glad/glad.h>

void Renderer::Clear() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(unsigned int vaoID) {
    glBindVertexArray(vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 3); 
    glBindVertexArray(0); 
}

void Renderer::Draw(unsigned int vaoID, size_t vertexCount, Shader& shader) {
    shader.Use();                      // Use the shader
    glBindVertexArray(vaoID);          // Bind mesh data
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

unsigned int Renderer::CreateMesh(const float* vertices, size_t vertexCount) {
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(float), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}
