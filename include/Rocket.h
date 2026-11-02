#pragma once
#include "Math3D.h"

class Rocket {
public:
    enum class Stage { ready, countdown, ignition, ascent, complete };
    void start();
    void reset();
    void update(double dt);
    Stage stage() const { return stage_; }
    const char* status() const;
    int countdown() const;
    double altitude() const { return altitude_; }
    double velocity() const { return velocity_; }
    bool enginesOn() const;
    math3d::mat4 model() const;
private:
    Stage stage_ = Stage::ready;
    double elapsed_ = 0.0, altitude_ = 0.0, velocity_ = 0.0;
};
