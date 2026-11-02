#pragma once
#include <GL/glew.h>
#include "Geometry.h"

class Mesh {
public:
    explicit Mesh(const MeshData& data);
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    void draw() const;
private:
    GLuint vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLsizei count_ = 0;
};
