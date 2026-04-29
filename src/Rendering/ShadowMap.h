#pragma once
#include <glad/glad.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class ShadowMap {
public:
    static constexpr int SHADOW_WIDTH = 2048;
    static constexpr int SHADOW_HEIGHT = 2048;

    ShadowMap() = default;
    ~ShadowMap();

    void Init();
    void BindForWriting() const;
    void BindForReading(unsigned int textureUnit) const;
    void RestoreViewport(int width, int height) const;

    GLuint GetTexture() const { return m_DepthTexture; }

    // Calculate light space matrix for directional light
    glm::mat4 GetLightSpaceMatrix(const glm::vec3& lightDirection,
                                   const glm::vec3& sceneCenter,
                                   float sceneRadius) const;

private:
    GLuint m_FBO = 0;
    GLuint m_DepthTexture = 0;
};