#pragma once
#include <glad/glad.h>
#include "Shader.h"
#include "Mesh.h"

class OffscreenBuffer {
public:
    OffscreenBuffer() = default;
    ~OffscreenBuffer();

    // renderWidth/Height — internal resolution to render at
    // windowWidth/Height — output resolution to upscale to
    void Init(int renderWidth, int renderHeight,
        int windowWidth, int windowHeight);

    void BindForWriting() const;
    void Present(Shader& shader, Mesh& quadMesh) const;
    void BlitDepthToScreen() const;

    GLuint GetColorTexture() const { return m_ColorTexture; }
    int    GetRenderWidth()  const { return m_RenderWidth; }
    int    GetRenderHeight() const { return m_RenderHeight; }

private:
    GLuint m_FBO = 0;
    GLuint m_ColorTexture = 0;
    GLuint m_DepthRBO = 0;

    int m_RenderWidth = 320;
    int m_RenderHeight = 240;
    int m_WindowWidth = 1280;
    int m_WindowHeight = 720;
};