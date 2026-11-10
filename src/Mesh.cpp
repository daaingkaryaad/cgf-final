#include "Mesh.h"
#include <cstddef>
#include <limits>
#include <stdexcept>

Mesh::Mesh(const MeshData& data) {
    if (data.vertices.empty() || data.indices.empty() ||
        data.indices.size()>std::size_t(std::numeric_limits<GLsizei>::max()))
        throw std::runtime_error("invalid mesh size");
    for (const auto index : data.indices)
        if (index>=data.vertices.size()) throw std::runtime_error("mesh index out of range");
    count_=GLsizei(data.indices.size());
    glGenVertexArrays(1,&vao_);
    glGenBuffers(1,&vbo_);
    glGenBuffers(1,&ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(data.vertices.size()*sizeof(Vertex)),data.vertices.data(),GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(data.indices.size()*sizeof(unsigned int)),data.indices.data(),GL_STATIC_DRAW);
    const std::size_t offsets[]={offsetof(Vertex,x),offsetof(Vertex,nx),offsetof(Vertex,u)};
    const GLint sizes[]={3,3,2};
    for (GLuint i=0; i<3; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,sizes[i],GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<const void*>(offsets[i]));
    }
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteBuffers(1,&ebo_); glDeleteBuffers(1,&vbo_); glDeleteVertexArrays(1,&vao_);
}

void Mesh::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES,count_,GL_UNSIGNED_INT,nullptr);
}
