#include "OffscreenBuffer.h"
#include <stdexcept>

OffscreenBuffer::~OffscreenBuffer() {
    if (m_FBO)          glDeleteFramebuffers(1, &m_FBO);
    if (m_ColorTexture) glDeleteTextures(1, &m_ColorTexture);
    if (m_DepthRBO)     glDeleteRenderbuffers(1, &m_DepthRBO);
}

void OffscreenBuffer::Init(int renderWidth, int renderHeight,
    int windowWidth, int windowHeight) {
    m_RenderWidth = renderWidth;
    m_RenderHeight = renderHeight;
    m_WindowWidth = windowWidth;
    m_WindowHeight = windowHeight;

    // use nearest-neighbor for pixelated look, linear for smooth
    GLint filter = (renderWidth < windowWidth) ? GL_NEAREST : GL_LINEAR;

    glGenTextures(1, &m_ColorTexture);
    glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        m_RenderWidth, m_RenderHeight, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenRenderbuffers(1, &m_DepthRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24,
        m_RenderWidth, m_RenderHeight);

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, m_ColorTexture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, m_DepthRBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("OffscreenBuffer: framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OffscreenBuffer::BindForWriting() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, m_RenderWidth, m_RenderHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
}

void OffscreenBuffer::Present(Shader& shader, Mesh& quadMesh) const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND); // add this — reset any leftover blend state

    shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ColorTexture);
    shader.SetUniform1i("u_Screen", 0);

    quadMesh.Draw();

    glEnable(GL_DEPTH_TEST);
}
void OffscreenBuffer::BlitDepthToScreen() const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(
        0, 0, m_RenderWidth, m_RenderHeight,
        0, 0, m_WindowWidth, m_WindowHeight,
        GL_DEPTH_BUFFER_BIT, GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}