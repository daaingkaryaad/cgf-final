#include "Rocket.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr double padHeight = 0.65;
constexpr double ascentAcceleration = 1.1;
constexpr double maximumAltitude = 90.0;
constexpr double separationHeight = 54.0;
constexpr double boosterOffset = 1.38;
constexpr double boosterPivot = 3.2;
constexpr double boosterFallTime = 4.1;
const double separationTime = std::sqrt(2.0*separationHeight/ascentAcceleration);

math3d::mat4 rotationZ(double angle) {
    auto m = math3d::identity();
    const float c = float(std::cos(angle)), s = float(std::sin(angle));
    m[0] = c; m[1] = s;
    m[4] = -s; m[5] = c;
    return m;
}
}

void Rocket::start() {
    if (stage_ == Stage::ready) { stage_ = Stage::countdown; elapsed_ = 0.0; }
}

void Rocket::reset() {
    stage_ = Stage::ready;
    elapsed_ = altitude_ = velocity_ = 0.0;
}

void Rocket::update(double dt) {
    if (dt <= 0.0 || !std::isfinite(dt)) return;
    while (dt > 0.0) {
        if (stage_ == Stage::ready) return;
        if (stage_ == Stage::countdown || stage_ == Stage::ignition) {
            const double duration = stage_ == Stage::countdown ? 3.0 : 1.2;
            const double step = std::min(dt, duration - elapsed_);
            elapsed_ += step;
            dt -= step;
            if (elapsed_ >= duration) {
                stage_ = stage_ == Stage::countdown ? Stage::ignition : Stage::ascent;
                elapsed_ = 0.0;
            }
        } else {
            // the boosters can still be falling after the core stops ascending.
            elapsed_ = std::min(elapsed_+dt,separationTime+boosterFallTime);
            altitude_ = std::min(maximumAltitude,0.5*ascentAcceleration*elapsed_*elapsed_);
            velocity_ = ascentAcceleration*elapsed_;
            if (altitude_ >= maximumAltitude) { stage_ = Stage::complete; velocity_ = 0.0; }
            return;
        }
    }
}

int Rocket::countdown() const {
    return stage_ == Stage::countdown ? std::max(1, int(std::ceil(3.0-elapsed_))) : 0;
}

bool Rocket::enginesOn() const {
    return stage_ == Stage::ignition || stage_ == Stage::ascent;
}

math3d::mat4 Rocket::model() const {
    return math3d::translation({0,padHeight+altitude_,0});
}

bool Rocket::boostersSeparated() const {
    return altitude_ >= separationHeight;
}

bool Rocket::boostersVisible() const {
    return !boostersSeparated() || elapsed_ < separationTime+boosterFallTime;
}

math3d::mat4 Rocket::boosterModel(std::size_t index) const {
    using namespace math3d;
    const double angle = index*(2.0*pi/boosterCount);
    const vec3 outward{std::cos(angle),0,std::sin(angle)};
    const auto facing = rotation_y(-angle);
    if (!boostersSeparated()) {
        return multiply(model(),multiply(translation(outward*boosterOffset),facing));
    }

    // start at the attached pose, then drift and tumble around the booster center.
    const double age = std::max(0.0,elapsed_-separationTime);
    const double height = padHeight+separationHeight+boosterPivot
                        + ascentAcceleration*separationTime*age-7.0*age*age;
    const vec3 center = outward*(boosterOffset+4.5*age)+vec3{0,height,0};
    const auto tumble = rotationZ(-age*(1.15+0.14*index));
    return multiply(translation(center),
                    multiply(facing,multiply(tumble,translation({0,-boosterPivot,0}))));
}

const char* Rocket::status() const {
    switch (stage_) {
        case Stage::ready: return "ready on pad";
        case Stage::countdown: return "countdown";
        case Stage::ignition: return "engine ignition";
        case Stage::ascent: return boostersSeparated() ? "boosters separated" : "ascending";
        case Stage::complete: return "demo complete - r to reset";
    }
    return "ready";
}
