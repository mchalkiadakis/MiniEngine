#include "Material.h"

Material::Material(std::shared_ptr<Shader> shader,
    std::shared_ptr<Texture> texture,
    std::shared_ptr<Texture> normalMap)
    : m_Shader(shader)
    , m_Texture(texture)
    , m_NormalMap(normalMap)
{
}

void Material::Bind() const {
    m_Shader->Use();
    m_Texture->Bind(0);          // albedo on unit 0
    m_Shader->SetUniform1i("u_Texture", 0);

    if (m_NormalMap) {
        m_NormalMap->Bind(1);    // normal map on unit 1
        m_Shader->SetUniform1i("u_NormalMap", 1);
        m_Shader->SetUniform1i("u_UseNormalMap", 1);
    }
    else {
        m_Shader->SetUniform1i("u_UseNormalMap", 0);
    }
}