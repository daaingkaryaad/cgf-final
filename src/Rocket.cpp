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
constexpr double terrainHeight = -0.025; // Ground plane in Scene::buildSite.
constexpr double boosterGravity = 14.0;
constexpr double releaseRamp = 0.12;
constexpr double driftSpeed = 4.5;
constexpr double tumbleSpeed = 1.25;
constexpr double spinSpeed = 0.32;
const double separationTime = std::sqrt(2.0*separationHeight/ascentAcceleration);
const double separationAt = ignitionEnd+separationTime;
const double coreStopsAt = ignitionEnd+std::sqrt(2.0*maximumAltitude/ascentAcceleration);
const double releaseSpeed = ascentAcceleration*separationTime;
constexpr double releaseCenterHeight = padHeight+separationHeight+boosterPivot;

math3d::mat4 rotationZ(double angle) {
    auto m = math3d::identity();
    const float c = float(std::cos(angle)), s = float(std::sin(angle));
    m[0] = c; m[1] = s;
    m[4] = -s; m[5] = c;
    return m;
}

double easedFlightAge(double age) {
    return age*age/(age+releaseRamp);
}

double boosterCenterHeight(double age) {
    return releaseCenterHeight+releaseSpeed*age-0.5*boosterGravity*age*age;
}

math3d::mat4 boosterTumble(double easedAge) {
    return math3d::multiply(rotationZ(-tumbleSpeed*easedAge),math3d::rotation_y(spinSpeed*easedAge));
}

double boosterGroundClearance(double age) {
    // Facing yaw leaves height unchanged, so one local envelope fits all four boosters.
    const auto tumble = boosterTumble(easedFlightAge(age));
    const double x = tumble[1], y = tumble[5], z = tumble[9];
    const double radial = std::hypot(x,z);
    const auto cylinderBottom = [=](double base, double top, double radius) {
        return std::min((base-boosterPivot)*y,(top-boosterPivot)*y)-radius*radial;
    };
    // The envelopes cover the nozzle, skirt, shell with trim, nose tip, and outer fin.
    const double finBottom = 0.49*x+(1.2-boosterPivot)*y
                           -0.325*std::abs(x)-0.625*std::abs(y)-0.05*std::abs(z);
    const double lowest = std::min({cylinderBottom(0.09,0.51,0.32),
                                   cylinderBottom(0.45,1.15,0.68),
                                   cylinderBottom(1.15,5.65,0.58),
                                   (6.65-boosterPivot)*y,finBottom});
    return boosterCenterHeight(age)+lowest-terrainHeight;
}

double impactAge() {
    // On the way down, bracket contact between the apex and the pivot reaching ground.
    double above = releaseSpeed/boosterGravity;
    double below = (releaseSpeed+std::sqrt(releaseSpeed*releaseSpeed+
                   2.0*boosterGravity*(releaseCenterHeight-terrainHeight)))/boosterGravity;
    for (int i=0; i<32; ++i) {
        const double middle = (above+below)*0.5;
        if (boosterGroundClearance(middle)>0.0) above = middle;
        else below = middle;
    }
    return above;
}

const double boosterImpactAge = impactAge();
const double boostersImpactAt = separationAt+boosterImpactAge;
const double launchEndsAt = std::max(coreStopsAt,boostersImpactAt);
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
    const double remaining = launchEndsAt-launchElapsed_;
    launchElapsed_ = dt >= remaining ? launchEndsAt : launchElapsed_+dt;
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
    return launchElapsed_ < boostersImpactAt;
}

math3d::mat4 Rocket::boosterModel(std::size_t index) const {
    using namespace math3d;
    const double angle = index*(2.0*pi/boosterCount);
    const vec3 outward{std::cos(angle),0,std::sin(angle)};
    const auto facing = rotation_y(-angle);
    if (!boostersSeparated()) {
        return multiply(model(),multiply(translation(outward*boosterOffset),facing));
    }

    // Radial and angular speeds start at zero and ease toward their steady rates.
    const double age = std::min(launchElapsed_-separationAt,boosterImpactAge);
    const double easedAge = easedFlightAge(age);
    const vec3 center = outward*(boosterOffset+driftSpeed*easedAge)+vec3{0,boosterCenterHeight(age),0};
    const auto tumble = boosterTumble(easedAge);
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
