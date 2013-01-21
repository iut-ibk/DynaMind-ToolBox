#ifndef ROADCALC_H
#define ROADCALC_H

#include <stack>
#include <tuple>
#include <utility>

#include "lsystem.h"

class RoadCalc_Config
{
public:
  RoadCalc_Config(float segment_length,
                float left_angle, float right_angle,
                float initial_angle)
    : segment_length_(segment_length),
      left_angle_(left_angle),
      right_angle_(right_angle),
      initial_angle_(initial_angle)
  {}

  float segment_length() const { return segment_length_; }
  float left_angle()     const { return left_angle_; }
  float right_angle()    const { return right_angle_; }
  float initial_angle()  const { return initial_angle_; }

private:
  float segment_length_;
  float left_angle_;
  float right_angle_;
  float initial_angle_;
};

class RoadCalc
{
    public:
        RoadCalc(LSystem& system, RoadCalc_Config& config)
        : system_(system), config_(config) {}

        void init();
        void walk();

    private:
        void run();
        void forward();
        void rotate(float angle);
        void push_state();
        void pop_state();

        void render();

        LSystem system_;
        RoadCalc_Config config_;
        std::stack<std::tuple<float, float, float>> state_;
        float x_, y_, angle_;
};

#endif // ROADCALC_H
