#pragma once
#include <GL/glew.h>

class Texture {
public:
    Texture();
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    void bind() const;
private:
    GLuint id_ = 0;
};
