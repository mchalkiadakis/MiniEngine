#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <gtc/type_ptr.hpp>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vSrc = LoadFile(vertexPath);
    std::string fSrc = LoadFile(fragmentPath);
    m_ID = CreateProgram(vSrc, fSrc);
}

Shader::Shader(FromSourceTag, std::string_view vertexSource, std::string_view fragmentSource) {
    m_ID = CreateProgram(std::string(vertexSource), std::string(fragmentSource));
}

Shader::~Shader() {
    glDeleteProgram(m_ID);
}

int Shader::GetUniformLocation(const std::string& name) {
    auto it = m_UniformLocationCache.find(name);
    if (it != m_UniformLocationCache.end())
        return it->second;

    int location = glGetUniformLocation(m_ID, name.c_str());
    m_UniformLocationCache[name] = location;
    return location;
}

void Shader::Use() const {
    glUseProgram(m_ID);
}

void Shader::SetUniform1i(const std::string& name, int value) {
    glUseProgram(m_ID);
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string& name, float value) {
    glUseProgram(m_ID);
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& value) {
    glUseProgram(m_ID);
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetUniformMat4(const std::string& name, const float* matrix) {
    glUseProgram(m_ID);
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix);
}

std::string Shader::LoadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }

    return shader;
}

unsigned int Shader::CreateProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}
void Shader::SetUniformMat3(const std::string& name, const float* matrix) {
    glUseProgram(m_ID);
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, matrix);
}