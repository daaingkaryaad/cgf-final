#include "Rocket.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr double padHeight = 0.65;
constexpr double countdownDuration = 3.0;
constexpr double ignitionEnd = countdownDuration+1.2;
constexpr double ascentAcceleration = 1.1;
constexpr double maximumAltitude = 90.0;
constexpr double separationHeight = 54.0;
constexpr double boosterOffset = 1.38;
constexpr double boosterPivot = 3.2;
constexpr double boosterFallTime = 4.1;
const double separationTime = std::sqrt(2.0*separationHeight/ascentAcceleration);
const double separationAt = ignitionEnd+separationTime;
const double coreStopsAt = ignitionEnd+std::sqrt(2.0*maximumAltitude/ascentAcceleration);
const double boostersHideAt = separationAt+boosterFallTime;

math3d::mat4 rotationZ(double angle) {
    auto m = math3d::identity();
    const float c = float(std::cos(angle)), s = float(std::sin(angle));
    m[0] = c; m[1] = s;
    m[4] = -s; m[5] = c;
    return m;
}
}

void Rocket::start() {
    if (stage_ == Stage::ready) stage_ = Stage::countdown;
}

void Rocket::reset() {
    stage_ = Stage::ready;
    launchElapsed_ = altitude_ = velocity_ = 0.0;
}

void Rocket::update(double dt) {
    if (stage_ == Stage::ready || dt <= 0.0 || !std::isfinite(dt)) return;
    // Use one clock for event boundaries; keep it running after the core stops.
    const double remaining = boostersHideAt-launchElapsed_;
    launchElapsed_ = dt >= remaining ? boostersHideAt : launchElapsed_+dt;
    if (launchElapsed_ < countdownDuration) {
        stage_ = Stage::countdown;
    } else if (launchElapsed_ < ignitionEnd) {
        stage_ = Stage::ignition;
    } else if (launchElapsed_ < coreStopsAt) {
        stage_ = Stage::ascent;
        const double ascentTime = launchElapsed_-ignitionEnd;
        altitude_ = 0.5*ascentAcceleration*ascentTime*ascentTime;
        velocity_ = ascentAcceleration*ascentTime;
    } else {
        stage_ = Stage::complete;
        altitude_ = maximumAltitude;
        velocity_ = 0.0;
    }
}

int Rocket::countdown() const {
    return stage_ == Stage::countdown ? int(std::ceil(countdownDuration-launchElapsed_)) : 0;
}

bool Rocket::enginesOn() const {
    return stage_ == Stage::ignition || stage_ == Stage::ascent;
}

math3d::mat4 Rocket::model() const {
    return math3d::translation({0,padHeight+altitude_,0});
}

bool Rocket::boostersSeparated() const {
    return launchElapsed_ >= separationAt;
}

bool Rocket::boostersVisible() const {
    return launchElapsed_ < boostersHideAt;
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
    const double age = launchElapsed_-separationAt;
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
