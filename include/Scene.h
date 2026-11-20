#pragma once
#include "Mesh.h"
#include "ParticleSystem.h"
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
    void updateExhaust(const Rocket& rocket, double dt);
    void resetExhaust() { exhaust_.reset(); }
    bool exhaustActive() const { return exhaust_.hasParticles(); }
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
    Shader shader_, sky_, shadowShader_;
    Texture concrete_;
    ParticleSystem exhaust_;
    GLuint skyVao_ = 0, shadowFbo_ = 0, shadowTexture_ = 0;
    int shadowSize_ = 2048;
    bool depthPass_ = false;
    std::vector<Part> site_, rocket_, boosterParts_, workers_;
    void buildSite();
    void initShadowMap();
    math3d::mat4 renderShadowMap(const Rocket& rocket, bool padLight);
    void drawOpaque(const Rocket& rocket, bool padLight);
    void buildEquipment();
    void buildWorkers();
    void addBeam(std::vector<Part>& list, Shape shape, const math3d::vec3& a,
                 const math3d::vec3& b, double radius, const math3d::vec3& colour,
                 float specular=0.15f);
    void appendParts(std::vector<Part>& target, const std::vector<Part>& parts,
                     const math3d::mat4& parent);
    void buildRocket();
    void buildBoosters();
    void add(std::vector<Part>& list, Shape shape, const math3d::vec3& pos,
             const math3d::vec3& scale, const math3d::vec3& colour,
             float specular=0.15f, float repeat=0.0f, double yaw=0.0, bool emissive=false);
    void draw(const Part& part, const math3d::mat4& parent);
    void drawBoosters(const Rocket& rocket);
};
