#include "Material.h"

Material::Material(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> texture)
    : m_Shader(shader), m_Texture(texture)
{
}

void Material::Bind() const {
    m_Shader->Use();
    m_Texture->Bind(0); // Default slot 0
    //could also call m_Shader->SetUniform("u_Texture", 0) if needed
}

