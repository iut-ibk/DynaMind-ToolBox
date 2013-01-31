#ifndef _LSYSTEM_H
#define _LSYSTEM_H

#include <string>
#include <map>

typedef std::map<char, std::string> rule_map;

class LSystem
{
public:
  LSystem(const char* initial, rule_map& rules)
    : current(initial), rules(rules)
  {}

  const std::string& evolve();

private:
  std::string current;
  rule_map rules;
};

#endif // LSYSTEM_H
