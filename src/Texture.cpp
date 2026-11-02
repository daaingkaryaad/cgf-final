#include "Texture.h"
#include <algorithm>
#include <cstdint>
#include <vector>

Texture::Texture() {
    const int size=256;
    std::vector<unsigned char> pixels(size*size*3);
    for (int y=0; y<size; ++y) for (int x=0; x<size; ++x) {
        std::uint32_t h=std::uint32_t(x)*374761393u + std::uint32_t(y)*668265263u;
        h=(h^(h>>13u))*1274126177u;
        const int noise=int((h^(h>>16u))&31u)-16;
        const int seam=(x<2 || y<2) ? -30 : 0;
        const int value=std::max(0,std::min(255,193+noise+seam));
        const std::size_t offset=std::size_t(y*size+x)*3;
        pixels[offset]=static_cast<unsigned char>(value);
        pixels[offset+1]=static_cast<unsigned char>(value-3);
        pixels[offset+2]=static_cast<unsigned char>(value-9);
    }
    glGenTextures(1,&id_);
    bind();
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB8,size,size,0,GL_RGB,GL_UNSIGNED_BYTE,pixels.data());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::~Texture() { glDeleteTextures(1,&id_); }
void Texture::bind() const { glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,id_); }
