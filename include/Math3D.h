#pragma once

#include <array>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace math3d {

constexpr double pi = 3.14159265358979323846;

struct vec3 {
    double x, y, z;
};

inline vec3 operator+(const vec3& a, const vec3& b) {
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
inline vec3 operator-(const vec3& a, const vec3& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
inline vec3 operator*(const vec3& a, double s) {
    return {a.x * s, a.y * s, a.z * s};
}
inline vec3 operator/(const vec3& a, double s) {
    return {a.x / s, a.y / s, a.z / s};
}
inline double dot(const vec3& a, const vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline vec3 cross(const vec3& a, const vec3& b) {
    return {a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x};
}
inline double length(const vec3& a) {
    return std::hypot(std::hypot(a.x, a.y), a.z);
}
inline vec3 normalized(const vec3& a) {
    const double n = length(a);
    if (!(n > 0.0) || !std::isfinite(n)) {
        throw std::runtime_error("cannot normalize a zero or non-finite vector");
    }
    return a / n;
}

using mat4 = std::array<float, 16>;

inline mat4 identity() {
    return {{1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1}};
}
inline mat4 multiply(const mat4& a, const mat4& b) {
    mat4 result{};
    for (int col = 0; col < 4; ++col)
        for (int row = 0; row < 4; ++row)
            for (int k = 0; k < 4; ++k)
                result[col * 4 + row] += a[k * 4 + row] * b[col * 4 + k];
    return result;
}

inline mat4 look_at(const vec3& eye, const vec3& target, const vec3& up) {
    const vec3 forward = normalized(target - eye);
    const vec3 right = normalized(cross(forward, up));
    const vec3 camera_up = cross(right, forward);
    mat4 result = identity();
    result[0] = static_cast<float>(right.x);
    result[4] = static_cast<float>(right.y);
    result[8] = static_cast<float>(right.z);
    result[1] = static_cast<float>(camera_up.x);
    result[5] = static_cast<float>(camera_up.y);
    result[9] = static_cast<float>(camera_up.z);
    result[2] = static_cast<float>(-forward.x);
    result[6] = static_cast<float>(-forward.y);
    result[10] = static_cast<float>(-forward.z);
    result[12] = static_cast<float>(-dot(right, eye));
    result[13] = static_cast<float>(-dot(camera_up, eye));
    result[14] = static_cast<float>(dot(forward, eye));
    return result;
}

inline float projection_value(double value) {
    if (!std::isfinite(value) || std::abs(value) > std::numeric_limits<float>::max())
        throw std::runtime_error("projection coefficient exceeds float range");
    return static_cast<float>(value);
}

inline mat4 perspective(double fovy, double aspect, double near_z, double far_z) {
    if (!std::isfinite(fovy) || !std::isfinite(aspect) ||
        !std::isfinite(near_z) || !std::isfinite(far_z) ||
        !(fovy > 0 && fovy < pi && aspect > 0 && near_z > 0 && far_z > near_z))
        throw std::runtime_error("invalid perspective parameters");
    const double f = 1.0 / std::tan(fovy * 0.5);
    mat4 result{};
    result[0] = projection_value(f / aspect);
    result[5] = projection_value(f);
    result[10] = projection_value(-1.0 - 2.0 * (near_z / (far_z - near_z)));
    result[11] = -1.0f;
    result[14] = projection_value(-2.0 * (near_z / (far_z - near_z)) * far_z);
    return result;
}

inline mat4 orthographic(double left, double right, double bottom, double top,
                         double near_z, double far_z) {
    if (!std::isfinite(left) || !std::isfinite(right) ||
        !std::isfinite(bottom) || !std::isfinite(top) ||
        !std::isfinite(near_z) || !std::isfinite(far_z) ||
        !(right > left && top > bottom && near_z > 0 && far_z > near_z))
        throw std::runtime_error("invalid orthographic parameters");
    if (!std::isfinite(right-left) || !std::isfinite(top-bottom))
        throw std::runtime_error("orthographic extent exceeds double range");
    mat4 result = identity();
    result[0] = projection_value(2.0 / (right - left));
    result[5] = projection_value(2.0 / (top - bottom));
    result[10] = projection_value(-2.0 / (far_z - near_z));
    result[12] = projection_value(-(right / (right - left) + left / (right - left)));
    result[13] = projection_value(-(top / (top - bottom) + bottom / (top - bottom)));
    result[14] = projection_value(-1.0 - 2.0 * (near_z / (far_z - near_z)));
    return result;
}

inline mat4 translation(const vec3& p) {
    mat4 m = identity();
    m[12] = float(p.x); m[13] = float(p.y); m[14] = float(p.z);
    return m;
}

inline mat4 scaling(const vec3& s) {
    mat4 m = identity();
    m[0] = float(s.x); m[5] = float(s.y); m[10] = float(s.z);
    return m;
}

inline mat4 rotation_y(double angle) {
    mat4 m = identity();
    const float c = float(std::cos(angle)), s = float(std::sin(angle));
    m[0] = c; m[2] = -s; m[8] = s; m[10] = c;
    return m;
}

inline mat4 transform(const vec3& p, const vec3& s, double yaw = 0.0) {
    return multiply(translation(p), multiply(rotation_y(yaw), scaling(s)));
}

inline vec3 transform_point(const mat4& m, const vec3& p) {
    return {m[0]*p.x + m[4]*p.y + m[8]*p.z + m[12],
            m[1]*p.x + m[5]*p.y + m[9]*p.z + m[13],
            m[2]*p.x + m[6]*p.y + m[10]*p.z + m[14]};
}

// align a unit-height box's local y axis with the segment from a to b.
inline mat4 beam(const vec3& a, const vec3& b, double width) {
    const vec3 y = normalized(b - a);
    const vec3 helper = std::abs(y.y) > 0.95 ? vec3{1,0,0} : vec3{0,1,0};
    const vec3 x = normalized(cross(y, helper));
    const vec3 z = cross(x, y);
    const double height = length(b - a);
    const vec3 mid = (a+b)*0.5;
    return {{float(x.x*width), float(x.y*width), float(x.z*width), 0,
             float(y.x*height), float(y.y*height), float(y.z*height), 0,
             float(z.x*width), float(z.y*width), float(z.z*width), 0,
             float(mid.x), float(mid.y), float(mid.z), 1}};
}

} 
