#pragma once

#include "Shader.h"
#include "Texture.h"
#include <memory>

class Material {
public:
    Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture,
        std::shared_ptr<Texture> normalMap = nullptr);

    void Bind() const;

    std::shared_ptr<Shader>  GetShader()    const { return m_Shader; }
    bool                     HasNormalMap() const { return m_NormalMap != nullptr; }

private:
    std::shared_ptr<Shader>  m_Shader;
    std::shared_ptr<Texture> m_Texture;
    std::shared_ptr<Texture> m_NormalMap;
};