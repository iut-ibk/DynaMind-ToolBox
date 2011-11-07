#ifndef COMPONENT_H
#define COMPONENT_H

class QString;

namespace DM {

    class Attribute;

    class Component
    {
    public:
        bool setAttribute(Attribute *newattribute);
        Attribute* getAttribute(QString ID);
    };
}
#endif // COMPONENT_H
