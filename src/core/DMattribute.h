#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <iostream>
class QVariant;

namespace DM {
    class Attribute
    {
    private:
        QVariant *value;
        std::string id;
        std::string name;

    public:
        Attribute(std::string name, std::string id);
        ~Attribute();
        void setValue(QVariant* value);
        QVariant* getValue();
        void setName(std::string name);
        void setID(std::string ID);
        std::string getName();
        std::string getID();
    };
}
#endif // ATTRIBUTE_H
