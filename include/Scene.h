#pragma once
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Rocket.h"
#include <array>
#include <memory>
#include <string>
#include <vector>

class Scene {
public:
    explicit Scene(const std::string& shaderDirectory);
    ~Scene();
    void render(const math3d::mat4& view, const math3d::mat4& projection,
                const math3d::vec3& eye, const Rocket& rocket, bool padLight);
private:
    enum Shape { box, plane, cylinder, cone, taper, shapeCount };
    struct Part {
        Shape shape;
        math3d::mat4 local;
        math3d::vec3 colour;
        float specular, textureRepeat;
        bool emissive;
    };
    std::array<std::unique_ptr<Mesh>,shapeCount> meshes_;
    Shader shader_, sky_;
    Texture concrete_;
    GLuint skyVao_ = 0;
    std::vector<Part> site_, rocket_, booster_;
    void buildSite();
    void buildRocket();
    void add(std::vector<Part>& list, Shape shape, const math3d::vec3& pos,
             const math3d::vec3& scale, const math3d::vec3& colour,
             float specular=0.15f, float repeat=0.0f, double yaw=0.0, bool emissive=false);
    void draw(const Part& part, const math3d::mat4& parent);
};
