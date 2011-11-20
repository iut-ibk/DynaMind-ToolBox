#include "DMview.h"
namespace DM {
View::View(std::string name,int type)
{
    this->type = type;
    this->name = name;
}
void View::setAttributes(std::string name) {
    this->Attributes.push_back(name);
}
}
