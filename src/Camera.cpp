#include "Camera.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr math3d::vec3 chaseOffset{14.0, 6.0, 21.0};
constexpr double chaseResponse = 5.0;
}

Camera::Camera() { home(); }

void Camera::home() {
    chasing_ = false;
    updateIgnitionShake(false,0.0);
    position_ = {26,17,32};
    const auto d = math3d::normalized(math3d::vec3{-1,6,0} - position_);
    yaw_ = std::atan2(d.z, d.x);
    pitch_ = std::asin(d.y);
}

math3d::vec3 Camera::direction() const {
    return {std::cos(yaw_)*std::cos(pitch_), std::sin(pitch_),
            std::sin(yaw_)*std::cos(pitch_)};
}

void Camera::look(double dx, double dy) {
    if (chasing_) return;
    yaw_ += dx*0.0025;
    pitch_ = std::max(-1.48, std::min(1.48, pitch_ - dy*0.0025));
}

void Camera::move(double forward, double right, double vertical, double dt, bool fast) {
    if (chasing_) return;
    const auto f = direction();
    const auto r = math3d::normalized(math3d::cross(f, {0,1,0}));
    auto velocity = f*forward + r*right + math3d::vec3{0,vertical,0};
    const double magnitude = math3d::length(velocity);
    if (magnitude > 1.0) velocity = velocity / magnitude;
    position_ = position_ + velocity*(dt*(fast ? 24.0 : 9.0));
    position_.y = std::max(0.8, position_.y);
}

void Camera::toggleChase(const math3d::vec3& target) {
    chasing_ = !chasing_;
    if (chasing_) chaseTarget_ = target;
}

void Camera::follow(const math3d::vec3& target, double dt) {
    if (!chasing_ || dt <= 0.0 || !std::isfinite(dt)) return;
    // exponential smoothing gives the same response across frame intervals.
    const double blend = -std::expm1(-chaseResponse * dt);
    chaseTarget_ = chaseTarget_ + (target - chaseTarget_) * blend;
}

void Camera::updateIgnitionShake(bool igniting, double dt) {
    if (!igniting) {
        shakeTime_ = 0.0;
        shakeOffset_ = {0,0,0};
        return;
    }
    if (dt <= 0.0 || !std::isfinite(dt)) return;
    shakeTime_ += dt;
    // ramp in briefly, then let the vibration decay during ignition.
    const double ramp = std::min(1.0,shakeTime_ / 0.08);
    const double strength = 0.26 * ramp * std::exp(-3.5 * shakeTime_);
    shakeOffset_ = math3d::vec3{std::sin(45.0 * shakeTime_),
                              0.7 * std::sin(61.0 * shakeTime_),
                              0.45 * std::sin(39.0 * shakeTime_)} * strength;
}

math3d::vec3 Camera::position() const {
    const auto base = chasing_ ? chaseTarget_ + chaseOffset : position_;
    return base + shakeOffset_;
}

math3d::mat4 Camera::view() const {
    const auto eye = position();
    const auto target = chasing_ ? chaseTarget_ + shakeOffset_ : eye + direction();
    return math3d::look_at(eye, target, {0,1,0});
}
