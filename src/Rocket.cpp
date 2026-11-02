#include "Rocket.h"
#include <algorithm>
#include <cmath>

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
        if (stage_ == Stage::ready || stage_ == Stage::complete) return;
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
            elapsed_ += dt;
            dt = 0.0;
            altitude_ = std::min(90.0, 0.5*1.1*elapsed_*elapsed_);
            velocity_ = 1.1*elapsed_;
            if (altitude_ >= 90.0) { stage_ = Stage::complete; velocity_ = 0.0; }
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
    return math3d::translation({0,0.65+altitude_,0});
}

const char* Rocket::status() const {
    switch (stage_) {
        case Stage::ready: return "ready on pad";
        case Stage::countdown: return "countdown";
        case Stage::ignition: return "engine ignition";
        case Stage::ascent: return "ascending";
        case Stage::complete: return "demo complete - r to reset";
    }
    return "ready";
}
