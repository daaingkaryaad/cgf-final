#pragma once
#include "Shader.h"
#include "Rocket.h"
#include <vector>

class Hud {
public:
    explicit Hud(const std::string& directory);
    ~Hud();
    void render(int width, int height, const Rocket& rocket, double fps,
                bool padLight, bool mouseCaptured);
private:
    struct Pixel { float x,y,r,g,b,a; };
    Shader shader_;
    GLuint vao_ = 0, vbo_ = 0;
    std::vector<Pixel> pixels_;
    void rect(float x,float y,float w,float h,const math3d::vec3& colour,float alpha=1);
    void text(float x,float y,const std::string& value,float size,const math3d::vec3& colour);
};
