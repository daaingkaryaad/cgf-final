#pragma once
#include "Shader.h"
#include <array>
#include <random>
#include <vector>

class ParticleSystem {
public:
    using Outlets = std::array<math3d::vec3,5>;

    explicit ParticleSystem(const std::string& shaderDirectory);
    ~ParticleSystem();
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;

    void update(double dt, const Outlets& outlets, bool firing);
    void render(const math3d::mat4& view, const math3d::mat4& projection);
    void reset();

private:
    struct Particle {
        math3d::vec3 position, velocity;
        double age, lifetime, radius, angle, seed;
        bool smoke;
    };

    struct Instance {
        float x, y, z, radius;
        float r, g, b, alpha;
        float angle, seed, depth;
    };

    void emit(double dt, const Outlets& outlets, bool smoke);
    void drawPass(const math3d::mat4& view, bool smoke);

    Shader shader_;
    GLuint vao_ = 0, vbo_ = 0;
    std::vector<Particle> particles_;
    std::vector<Instance> instances_;
    std::mt19937 random_{451};
    std::uniform_real_distribution<double> unit_{0.0,1.0};
    Outlets previousOutlets_{};
    double fireCarry_ = 0.0, smokeCarry_ = 0.0;
    bool emitting_ = false;
};
