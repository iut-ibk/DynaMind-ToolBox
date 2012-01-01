#include "DMview.h"
namespace DM {
View::View(std::string name, int type, int accesstypeGeometry)
{
    this->name = name;
    this->type = type;
    this->accesstypeGeometry = accesstypeGeometry;

}

void View::setIdOfDummyComponent(std::string UUID) {
    this->IdofDummyComponent = UUID;
}

std::string View::getIdOfDummyComponent() {
    return this->IdofDummyComponent;
}

void View::addAttributes(std::string name, int access) {
    this->ownedAttributes[name] = access;

}


std::vector<std::string> View::getWriteAttributes() const {
    std::vector<std::string> attrs;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second > READ)
            attrs.push_back(it->first);
    }

    return attrs;
}

std::vector<std::string> View::getReadAttributes() const {
    std::vector<std::string> attrs;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second < WRITE)
            attrs.push_back(it->first);
    }

    return attrs;



}

bool View::reads() {
    if (this->accesstypeGeometry < WRITE)
        return true;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second < WRITE)
            return true;
    }

    return false;
}

bool View::writes() {
    if (this->accesstypeGeometry > READ)
        return true;
    for (std::map<std::string, int>::const_iterator it = this->ownedAttributes.begin(); it != this->ownedAttributes.end(); ++it) {
        if (it->second > READ)
            return true;
    }

    return false;
}

}


