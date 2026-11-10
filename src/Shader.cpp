#include "Shader.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {
std::string readText(const std::string& path) {
    std::ifstream file(path);
    if (!file) throw std::runtime_error("cannot open shader: " + path);
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compile(GLenum type, const std::string& path) {
    const std::string source = readText(path);
    const char* text = source.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);
    GLint ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(std::max(1, length));
        glGetShaderInfoLog(shader, GLsizei(log.size()), nullptr, log.data());
        glDeleteShader(shader);
        throw std::runtime_error(path + ": " + log.data());
    }
    return shader;
}
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    const GLuint vertex = compile(GL_VERTEX_SHADER, vertexPath);
    GLuint fragment = 0;
    try { fragment = compile(GL_FRAGMENT_SHADER, fragmentPath); }
    catch (...) { glDeleteShader(vertex); throw; }
    program_ = glCreateProgram();
    glAttachShader(program_, vertex);
    glAttachShader(program_, fragment);
    glLinkProgram(program_);
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    GLint ok = GL_FALSE;
    glGetProgramiv(program_, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint length = 0;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(std::max(1,length));
        glGetProgramInfoLog(program_, GLsizei(log.size()), nullptr, log.data());
        glDeleteProgram(program_);
        throw std::runtime_error("shader link failed: " + std::string(log.data()));
    }
}

Shader::~Shader() { glDeleteProgram(program_); }
void Shader::use() const { glUseProgram(program_); }
GLint Shader::location(const std::string& name) {
    const auto found = uniforms_.find(name);
    if (found != uniforms_.end()) return found->second;
    const GLint value = glGetUniformLocation(program_, name.c_str());
    if (value < 0) throw std::runtime_error("missing shader uniform: " + name);
    uniforms_[name] = value;
    return value;
}
void Shader::matrix(const std::string& name, const math3d::mat4& v) {
    glUniformMatrix4fv(location(name), 1, GL_FALSE, v.data());
}
void Shader::vector(const std::string& name, const math3d::vec3& v) {
    glUniform3f(location(name), float(v.x), float(v.y), float(v.z));
}
void Shader::scalar(const std::string& name, float v) { glUniform1f(location(name), v); }
void Shader::integer(const std::string& name, int v) { glUniform1i(location(name), v); }
