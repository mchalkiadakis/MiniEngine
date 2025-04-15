#include "Renderer.h"
#include <glad/glad.h>

void Renderer::Clear() {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw() {
    // You'll hook this up to actual VAOs/VBOs later
}