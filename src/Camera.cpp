#include "Camera.h"
#include <algorithm>
#include <cmath>

namespace {
constexpr math3d::vec3 chaseOffset{14.0, 6.0, 21.0};
constexpr double chaseResponse = 5.0;
bool finiteTarget(const math3d::vec3& target) {
    return std::isfinite(target.x) && std::isfinite(target.y) && std::isfinite(target.z);
}
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
    if (chasing_ || !std::isfinite(dx) || !std::isfinite(dy)) return;
    const double turn = 2.0 * math3d::pi;
    yaw_ = std::remainder(yaw_ + std::remainder(dx*0.0025, turn), turn);
    pitch_ = std::max(-1.48, std::min(1.48, pitch_ - dy*0.0025));
}

void Camera::move(double forward, double right, double vertical, double dt, bool fast) {
    if (chasing_ || !(dt > 0.0) || !std::isfinite(dt) ||
        !std::isfinite(forward) || !std::isfinite(right) || !std::isfinite(vertical)) return;
    const auto axis = [](double value) { return std::max(-1.0, std::min(1.0, value)); };
    forward = axis(forward);
    right = axis(right);
    vertical = axis(vertical);
    const auto f = direction();
    const auto r = math3d::normalized(math3d::cross(f, {0,1,0}));
    auto velocity = f*forward + r*right + math3d::vec3{0,vertical,0};
    const double magnitude = math3d::length(velocity);
    if (magnitude > 1.0) velocity = velocity / magnitude;
    const auto next = position_ + velocity*(dt*(fast ? 24.0 : 9.0));
    if (!std::isfinite(next.x) || !std::isfinite(next.y) || !std::isfinite(next.z)) return;
    position_ = next;
    position_.y = std::max(0.8, position_.y);
}

void Camera::toggleChase(const math3d::vec3& target) {
    if (!chasing_ && !finiteTarget(target)) return;
    chasing_ = !chasing_;
    if (chasing_) chaseTarget_ = target;
}

void Camera::follow(const math3d::vec3& target, double dt) {
    if (!chasing_ || dt <= 0.0 || !std::isfinite(dt) || !finiteTarget(target)) return;
    const double blend = -std::expm1(-chaseResponse * dt);
    const auto next = chaseTarget_ * (1.0 - blend) + target * blend;
    if (finiteTarget(next)) chaseTarget_ = next;
}

void Camera::updateIgnitionShake(bool igniting, double dt) {
    if (!igniting) {
        shakeTime_ = 0.0;
        shakeOffset_ = {0,0,0};
        return;
    }
    if (dt <= 0.0 || !std::isfinite(dt)) return;
    constexpr double shakeDuration = 5.0;
    shakeTime_ = std::min(shakeDuration, shakeTime_ + std::min(dt, shakeDuration));
    if (shakeTime_ >= shakeDuration) {
        shakeOffset_ = {0,0,0};
        return;
    }
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
    const auto target = chasing_ ? chaseTarget_ : eye + direction();
    return math3d::look_at(eye, target, {0,1,0});
}
