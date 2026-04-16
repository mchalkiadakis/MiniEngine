#pragma once
#include <memory>
#include <vector>
#include <string>
#include "CubemapTexture.h"
#include "Shader.h"
#include "Light.h"

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    ~Skybox();

    void Render(const RenderContext& ctx) const;

private:
    unsigned int              m_VAO = 0;
    unsigned int              m_VBO = 0;
    std::unique_ptr<CubemapTexture> m_Cubemap;
    std::unique_ptr<Shader>         m_Shader;

    void SetupMesh();
};