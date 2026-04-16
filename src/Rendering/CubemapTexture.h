#pragma once
#include <string>
#include <vector>

class CubemapTexture {
public:
    CubemapTexture(const std::vector<std::string>& faces);
    ~CubemapTexture();

    void Bind(unsigned int slot = 0) const;

private:
    unsigned int m_ID = 0;
};