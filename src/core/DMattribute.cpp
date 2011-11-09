#include <DMcomponent.h>
#include <DMattribute.h>
#include <QVariant>

using namespace DM;

Attribute::Attribute(std::string name, std::string id)
{
    this->value=0;
    this->name=name;
    this->id=id;
}

Attribute::~Attribute()
{
    if(!value)
        delete value;
}

void Attribute::setValue(QVariant* value)
{
    if(!value)
        delete value;

    this->value=value;
}

QVariant* Attribute::getValue()
{
    return value;
}

void Attribute::setName(std::string name)
{
    this->name=name;
}

void Attribute::setID(std::string ID)
{
    this->id=ID;
}
std::string Attribute::getName()
{
    return name;
}
std::string Attribute::getID()
{
    return id;
}
