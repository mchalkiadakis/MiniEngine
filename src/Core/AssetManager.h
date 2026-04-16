// src/Core/AssetManager.h
#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include "Rendering/Texture.h"
#include "Rendering/Shader.h"

class AssetManager {
public:
    static AssetManager& Get() {
        static AssetManager instance;
        return instance;
    }

    std::shared_ptr<Texture> LoadTexture(const std::string& path) {
        auto it = m_Textures.find(path);
        if (it != m_Textures.end()) return it->second;
        auto texture = std::make_shared<Texture>(path);
        m_Textures[path] = texture;
        return texture;
    }

    std::shared_ptr<Shader> LoadShader(const std::string& vertPath, const std::string& fragPath) {
        std::string key = vertPath + "|" + fragPath;
        auto it = m_Shaders.find(key);
        if (it != m_Shaders.end()) return it->second;
        auto shader = std::make_shared<Shader>(vertPath, fragPath);
        m_Shaders[key] = shader;
        return shader;
    }

    void Clear() {
        m_Textures.clear();
        m_Shaders.clear();
    }

private:
    AssetManager() = default;
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_Textures;
    std::unordered_map<std::string, std::shared_ptr<Shader>>  m_Shaders;
};