#include <QtCore>
#include <DMcomponent.h>
#include <DMattribute.h>

using namespace DM;

bool Component::setAttribute(Attribute *newattribute)
{
    return true;
}

Attribute* Component::getAttribute(QString ID)
{
    return 0;
}
