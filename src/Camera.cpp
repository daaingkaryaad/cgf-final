#include "Camera.h"
#include <algorithm>

Camera::Camera() { home(); }

void Camera::home() {
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
    yaw_ += dx*0.0025;
    pitch_ = std::max(-1.48, std::min(1.48, pitch_ - dy*0.0025));
}

void Camera::move(double forward, double right, double vertical, double dt, bool fast) {
    const auto f = direction();
    const auto r = math3d::normalized(math3d::cross(f, {0,1,0}));
    auto velocity = f*forward + r*right + math3d::vec3{0,vertical,0};
    const double magnitude = math3d::length(velocity);
    if (magnitude > 1.0) velocity = velocity / magnitude;
    position_ = position_ + velocity*(dt*(fast ? 24.0 : 9.0));
    position_.y = std::max(0.8, position_.y);
}

math3d::mat4 Camera::view() const {
    return math3d::look_at(position_, position_ + direction(), {0,1,0});
}
