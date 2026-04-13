#pragma once
#pragma once
#include <string>
#include <memory>
#include "Model.h"
#include "Shader.h"
#include <unordered_map>



class ModelLoader {
public:
    static std::unique_ptr<Model> Load(const std::string& filepath,
        std::shared_ptr<Shader> shader);
private:
    static std::string s_Directory;
    static std::unordered_map<std::string, std::shared_ptr<Texture>> s_TextureCache;
};