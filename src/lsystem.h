#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <string>
#include <map>

typedef std::map<char, std::string> rule_map;

class LSystem
{
public:
  LSystem(const char* initial, rule_map& rules)
    : current_(initial), rules_(rules)
  {}

  const std::string& evolve();

private:
  std::string current_;
  rule_map rules_;
};

#endif // LSYSTEM_H
