#include "DMview.h"
namespace DM {
View::View(std::string name)
{
    this->name = name;

    this->readType = -1;
    this->writeType = -1;

}


void View::addAttributes(std::string name) {
    this->WriteAttributes.push_back(name);
}
void View::getAttributes(std::string name) {
    this->WriteAttributes.push_back(name);
}
void View::addComponent(int Type) {
    this->writeType = Type;
}
void View::getComponent(int Type) {
    this->readType = Type;
}
}
