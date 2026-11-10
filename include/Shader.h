#pragma once
#include <GL/glew.h>
#include "Math3D.h"
#include <string>
#include <unordered_map>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    void use() const;
    void matrix(const std::string& name, const math3d::mat4& value);
    void vector(const std::string& name, const math3d::vec3& value);
    void scalar(const std::string& name, float value);
    void integer(const std::string& name, int value);
private:
    GLuint program_ = 0;
    std::unordered_map<std::string, GLint> uniforms_;
    GLint location(const std::string& name);
};
