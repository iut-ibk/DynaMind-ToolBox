#include <functional>
#include <algorithm>
#include <utility>
#include <cmath>

#include "roadcalc.h"

void RoadCalc::walk()
{
  x_ = y_ = 0.0f;
  angle_ = config_.initial_angle();

  auto current = system_.evolve();
  std::for_each(current.begin(), current.end(),
		[&](char c)
		{
		  switch(c)
		    {
		    case 'F': 
		      forward();
		      break;
		    case '+':
		      rotate(config_.right_angle());
		      break;
		    case '-':
		      rotate(config_.left_angle());
		      break;
		    case '[':
		      push_state();
		      break;
		    case ']':
		      pop_state();
		      break;
		    }
		});
}

void RoadCalc::forward()
{
  float x1 = x_ + config_.segment_length() * cos(angle_ * (M_PI / 180.0f));
  float y1 = y_ + config_.segment_length() * sin(angle_ * (M_PI / 180.0f));

  glBegin(GL_LINES);
  glVertex2f(x_, y_);
  glVertex2f(x1, y1);
  glEnd();

  x_ = x1;
  y_ = y1;
}

void RoadCalc:: rotate(float angle)
{
  angle_ += angle;

  if(angle_ >= 360.0f)
    angle_ -= 360.0f;

  if(angle_ < 0.0f)
    angle_ += 360.0f;
}

void RoadCalc::push_state()
{
  auto state = std::make_tuple(x_, y_, angle_);
  state_.push(state);
}

void RoadCalc::pop_state()
{
  std::tie(x_, y_, angle_) = state_.top();
  state_.pop();
}
