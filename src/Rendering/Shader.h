#pragma once

#include <string>
#include <unordered_map>
struct FromSourceTag {};
inline constexpr FromSourceTag FromSource{};
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    Shader(FromSourceTag, std::string_view vertexSource, std::string_view fragmentSource);
    ~Shader();

    void Use() const;
    
    // Uniform setters (we’ll expand as needed)
    void SetUniform1i(const std::string& name, int value);
    void SetUniform1f(const std::string& name, float value);
    void SetUniformMat4(const std::string& name, const float* matrix); // expects glm::value_ptr(mat)

private:
    unsigned int m_ID;
    std::unordered_map<std::string, int> m_UniformLocationCache;

    std::string LoadFile(const std::string& path);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateProgram(const std::string& vertexSource, const std::string& fragmentSource);
    int GetUniformLocation(const std::string& name);
};
