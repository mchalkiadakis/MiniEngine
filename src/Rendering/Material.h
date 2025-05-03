#pragma once

#include "Shader.h"
#include "Texture.h"
#include <memory>


class Material {
public:
    Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture);
    void Bind() const;

    std::shared_ptr<Shader> GetShader() const { return m_Shader; } 

private:
    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<Texture> m_Texture;
};
