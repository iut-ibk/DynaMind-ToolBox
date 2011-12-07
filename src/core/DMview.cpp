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
    this->ReadAttributes.push_back(name);
}
void View::addComponent(int Type) {
    this->writeType = Type;
}
void View::addAvalibleAttribute(std::string name) {
    this->AvailableAttributes.push_back(name);
}

void View::getComponent(int Type) {
    this->readType = Type;
}

bool View::reads() {
    if (readType != -1 || ReadAttributes.size() > 0)
        return true;
    return false;
}

bool View::writes() {
    if (writeType != -1 || WriteAttributes.size() > 0)
        return true;
    return false;
}

}
