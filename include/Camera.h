#pragma once
#include "Math3D.h"

class Camera {
public:
    Camera();
    void home();
    void look(double dx, double dy);
    void move(double forward, double right, double vertical, double dt, bool fast);
    void toggleChase(const math3d::vec3& target);
    void follow(const math3d::vec3& target, double dt);
    void updateIgnitionShake(bool igniting, double dt);
    bool chasing() const { return chasing_; }
    math3d::mat4 view() const;
    math3d::vec3 position() const;
private:
    math3d::vec3 direction() const;
    math3d::vec3 position_{0,0,0};
    double yaw_ = 0.0, pitch_ = 0.0;
    bool chasing_ = false;
    math3d::vec3 chaseTarget_{0,0,0};
    double shakeTime_ = 0.0;
    math3d::vec3 shakeOffset_{0,0,0};
};
