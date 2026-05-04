#include "ShadowMap.h"
#include <stdexcept>

ShadowMap::~ShadowMap() {
    if (m_FBO)          glDeleteFramebuffers(1, &m_FBO);
    if (m_DepthTexture) glDeleteTextures(1, &m_DepthTexture);
}

void ShadowMap::Init() {
    glGenTextures(1, &m_DepthTexture);
    glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0,
        GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glGenFramebuffers(1, &m_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D, m_DepthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("ShadowMap: framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadowMap::BindForWriting() const {
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
}

void ShadowMap::BindForReading(unsigned int textureUnit) const {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, m_DepthTexture);
}

void ShadowMap::RestoreViewport(int width, int height) const {
    glViewport(0, 0, width, height);
}

glm::mat4 ShadowMap::GetLightSpaceMatrix(const glm::vec3& lightDirection,
                                          const glm::vec3& sceneCenter,
                                          float sceneRadius) const {
    // Position light far enough to encompass the scene
    // Light position is along the negated direction to face the scene
    glm::vec3 lightPos = sceneCenter - lightDirection * sceneRadius * 2.0f;

    // Orthographic projection sized to the scene
    float halfSize = sceneRadius * 1.5f; // 1.5x margin for safety
    glm::mat4 lightProjection = glm::ortho(
        -halfSize, halfSize,
        -halfSize, halfSize,
        0.1f, sceneRadius * 4.0f
    );

    // Look from light position towards scene center
    glm::mat4 lightView = glm::lookAt(
        lightPos,
        sceneCenter,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    return lightProjection * lightView;
}
void ShadowMap::FinishWriting() const {
    glDisable(GL_CULL_FACE);
}