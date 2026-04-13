#include "Texture.h"
#include <glad/glad.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <filesystem>

Texture::Texture(const std::string& filepath) {
    stbi_set_flip_vertically_on_load(1); // Flip image vertically for OpenGL
    unsigned char* data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    std::cout << "Trying to load: " << std::filesystem::absolute(filepath) << "\n";
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        std::cerr << "STB reason: " << stbi_failure_reason() << std::endl;
        return;
    }

    std::cout << "Loaded texture: " << filepath << "\n";
    std::cout << "Dimensions: " << m_Width << "x" << m_Height << ", Channels: " << m_Channels << "\n";
    std::cout << "First pixel RGBA: ";
    for (int i = 0; i < m_Channels; ++i) {
        std::cout << static_cast<int>(data[i]) << " ";
    }
    std::cout << "\n";

    glGenTextures(1, &m_ID);
    glBindTexture(GL_TEXTURE_2D, m_ID);

    // Choose format based on channel count
    GLenum format = m_Channels == 4 ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Set filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &m_ID);
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_ID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
