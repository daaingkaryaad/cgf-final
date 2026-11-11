#include "ParticleSystem.h"
#include <algorithm>
#include <cmath>
#include <cstddef>

namespace {
constexpr std::size_t maxParticles = 4096;
constexpr double padSurface = 0.61;
}

ParticleSystem::ParticleSystem(const std::string& directory)
    : shader_(directory+"/particles.vert",directory+"/particles.frag") {
    particles_.reserve(maxParticles);
    instances_.reserve(maxParticles);
    glGenVertexArrays(1,&vao_);
    glGenBuffers(1,&vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    const GLint widths[] = {3,1,4,2};
    const std::size_t offsets[] = {offsetof(Instance,x),offsetof(Instance,radius),
                                   offsetof(Instance,r),offsetof(Instance,angle)};
    for (GLuint i=0; i<4; ++i) {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,widths[i],GL_FLOAT,GL_FALSE,sizeof(Instance),
                              reinterpret_cast<const void*>(offsets[i]));
        glVertexAttribDivisor(i,1);
    }
    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem() {
    glDeleteBuffers(1,&vbo_);
    glDeleteVertexArrays(1,&vao_);
}

void ParticleSystem::reset() {
    particles_.clear();
    fireCarry_ = smokeCarry_ = 0.0;
    emitting_ = false;
    random_.seed(451);
}

void ParticleSystem::emit(double dt, const Outlets& outlets, bool smoke) {
    double& carry = smoke ? smokeCarry_ : fireCarry_;
    carry += dt * (smoke ? 30.0 : 76.0);
    const int count = static_cast<int>(carry);
    carry -= count;

    for (std::size_t engine=0; engine<outlets.size(); ++engine) {
        const auto movement = outlets[engine]-previousOutlets_[engine];
        for (int i=0; i<count && particles_.size()<maxParticles; ++i) {
            // distribute births along the engine's path during this frame.
            const double along = (i+unit_(random_))/count;
            const auto origin = previousOutlets_[engine]+movement*along;
            const double angle = 2.0*math3d::pi*unit_(random_);
            const double spread = std::sqrt(unit_(random_));
            const double x = std::cos(angle)*spread, z = std::sin(angle)*spread;
            const double speed = unit_(random_);
            const double scale = engine==0 ? 1.2 : 1.0;
            Particle p{};
            p.position = origin+math3d::vec3{0.22*x,-0.06,0.22*z};
            p.velocity = smoke ? math3d::vec3{2.5*x+0.85,-2.2-1.4*speed,2.5*z+0.3}
                               : math3d::vec3{0.9*x,-7.0-5.0*speed,0.9*z};
            p.lifetime = smoke ? 2.5+1.2*unit_(random_) : 0.20+0.15*unit_(random_);
            p.radius = (smoke ? 0.48+0.34*unit_(random_) : 0.23+0.19*unit_(random_))*scale;
            p.angle = angle;
            p.seed = unit_(random_);
            p.smoke = smoke;
            particles_.push_back(p);
        }
    }
}

void ParticleSystem::update(double dt, const Outlets& outlets, bool firing) {
    if (dt<=0.0 || !std::isfinite(dt)) return;
    dt = std::min(dt,0.05);
    for (auto& p : particles_) {
        p.age += dt;
        if (p.smoke) p.velocity.y += 0.95*dt;
        p.position = p.position+p.velocity*dt;
        if (p.position.y < padSurface) {
            p.position.y = padSurface;
            p.velocity.y = p.smoke ? 0.24 : 0.0;
        }
    }
    particles_.erase(std::remove_if(particles_.begin(),particles_.end(),[](const Particle& p) {
        return p.age>=p.lifetime;
    }),particles_.end());

    if (!firing) {
        fireCarry_ = smokeCarry_ = 0.0;
        emitting_ = false;
        return;
    }
    if (!emitting_) previousOutlets_ = outlets;
    emit(dt,outlets,false);
    emit(dt,outlets,true);
    previousOutlets_ = outlets;
    emitting_ = true;
}

void ParticleSystem::drawPass(const math3d::mat4& view, bool smoke) {
    instances_.clear();
    for (const auto& p : particles_) {
        if (p.smoke!=smoke) continue;
        const double progress = p.age/p.lifetime;
        const double remaining = 1.0-progress;
        const float radius = float(p.radius+(smoke ? 2.0 : 0.34)*progress);
        const float alpha = float(smoke ? 0.31*std::min(1.0,p.age/0.15)*remaining
                                        : 0.56*remaining*remaining);
        const auto& center = p.position;
        const float depth = float(view[2]*center.x+view[6]*center.y+view[10]*center.z+view[14]);
        instances_.push_back({float(center.x),float(center.y),float(center.z),radius,
                              smoke ? float(0.68-0.09*progress) : 1.0f,
                              smoke ? 0.63f : float(0.19+0.64*remaining),
                              smoke ? float(0.57+0.08*progress) : float(0.04+0.21*remaining),
                              alpha,float(smoke ? p.angle+0.35*p.age : 0.0),float(p.seed),depth});
    }
    if (instances_.empty()) return;
    if (smoke) {
        std::sort(instances_.begin(),instances_.end(),[](const Instance& a,const Instance& b) {
            return a.depth<b.depth;
        });
    }
    shader_.integer("smokePass",smoke ? 1 : 0);
    glBlendFunc(GL_SRC_ALPHA,smoke ? GL_ONE_MINUS_SRC_ALPHA : GL_ONE);
    glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(instances_.size()*sizeof(Instance)),
                 instances_.data(),GL_STREAM_DRAW);
    glDrawArraysInstanced(GL_TRIANGLES,0,6,GLsizei(instances_.size()));
}

void ParticleSystem::render(const math3d::mat4& view, const math3d::mat4& projection) {
    if (particles_.empty()) return;
    shader_.use();
    shader_.matrix("view",view);
    shader_.matrix("projection",projection);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER,vbo_);
    // keep opaque-scene depth testing and leave the depth buffer intact.
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    drawPass(view,true);
    drawPass(view,false);
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
}
