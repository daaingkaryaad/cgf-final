#pragma once
#include "Math3D.h"
#include <cstddef>

class Rocket {
public:
    enum class Stage { ready, countdown, ignition, ascent, complete };
    static constexpr std::size_t boosterCount = 4;
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
    bool boostersSeparated() const;
    bool boostersVisible() const;
    // Returns the world pose, including after impact. Throws std::out_of_range
    // if index is outside [0, boosterCount).
    math3d::mat4 boosterModel(std::size_t index) const;
private:
    Stage stage_ = Stage::ready;
    double launchElapsed_ = 0.0, altitude_ = 0.0, velocity_ = 0.0;
};
