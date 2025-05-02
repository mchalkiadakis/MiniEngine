#pragma once

#include <string>

class Texture {
public:
    Texture(const std::string& filepath);
    ~Texture();

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

private:
    unsigned int m_ID = 0;
    int m_Width = 0, m_Height = 0, m_Channels = 0;
};