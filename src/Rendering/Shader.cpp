#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>



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
    return glGetUniformLocation(m_ID, name.c_str());
}

void Shader::SetUniformMat4(const std::string& name, const float* matrix) {
    glUseProgram(m_ID);  // REQUIRED!
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, matrix);
}



void Shader::Use() const {
    glUseProgram(m_ID);
}
// Load shader source from file
std::string Shader::LoadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "Failed to open shader file: " << path << std::endl;
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();  // Read full file into stream
    return ss.str();
}
// Compile individual shader
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check for errors
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }
    std::cout << "Compiling shader source:\n" << source << "\n";
    return shader;
}
// Link shaders into a program
unsigned int Shader::CreateProgram(const std::string& vertexSource, const std::string& fragmentSource) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // Check for linking errors
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
