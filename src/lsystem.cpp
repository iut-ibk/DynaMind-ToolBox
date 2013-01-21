#include <sstream>
#include <algorithm>

#include "lsystem.h"

const std::string& 
LSystem::evolve()
{
  std::ostringstream out;
  
  std::for_each(current_.begin(), current_.end(),
		[&](char c) 
		{
		  auto res = rules_.find(c);
		  if(res != rules_.end()) 
		    out << res->second;
		  else
		    out << c;
		});

  current_ = out.str();

  return current_;
}
