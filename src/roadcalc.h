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
                float initial_angle,
                std::pair<float, float> translation = std::make_pair(0.0f, 0.0f),
                std::pair<float, float> scale = std::make_pair(1.0f, 1.0f))
    : segment_length_(segment_length),
      left_angle_(left_angle),
      right_angle_(right_angle),
      initial_angle_(initial_angle),
      translation_(translation),
      scale_(scale)
  {}

  float segment_length() const { return segment_length_; }
  float left_angle()     const { return left_angle_; }
  float right_angle()    const { return right_angle_; }
  float initial_angle()  const { return initial_angle_; }

  std::pair<float, float> translation() const { return translation_; }
  std::pair<float, float> scale()       const { return scale_; }

private:
  float segment_length_;
  float left_angle_;
  float right_angle_;
  float initial_angle_;
  std::pair<float, float> translation_;
  std::pair<float, float> scale_;
};

class RoadCalc
{
    public:
        RoadCalc(LSystem& system, RoadCalc_Config& config)
        : system_(system), config_(config) {}

        void init(int argc, char* argv[]);
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
