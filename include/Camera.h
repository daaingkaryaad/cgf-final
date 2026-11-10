#pragma once
#include "Math3D.h"

class Camera {
public:
    Camera();
    void home();
    void look(double dx, double dy);
    void move(double forward, double right, double vertical, double dt, bool fast);
    math3d::mat4 view() const;
    const math3d::vec3& position() const { return position_; }
private:
    math3d::vec3 direction() const;
    math3d::vec3 position_{0,0,0};
    double yaw_ = 0.0, pitch_ = 0.0;
};
