#ifndef COMPONENT_H
#define COMPONENT_H

#include <iostream>
#include <map>
#include <vector>
#include <list>

namespace DM {

    class Attribute;

    class Component
    {
    protected:
        std::string name;
        std::string id;
        std::map<std::string,Component*> childsview;
        std::map<std::string,Attribute*> attributesview;
        std::map<std::string,Component*> ownedchilds;
        std::map<std::string,Attribute*> ownedattributes;

    public:
        Component(std::string name, std::string id);
        Component(const Component& s);
        virtual ~Component();
        void setName(std::string name);
        std::string getName();
        void setID(std::string id);
        std::string getID();
        bool addAttribute(Attribute *newattribute);
        bool changeAttribute(Attribute *newattribute);
        bool removeAttribute(std::string name);
        Attribute* getAttribute(std::string name);
        std::map<std::string, Attribute*> getAllAttributes();
        bool addChild(Component *newcomponent);
        bool changeChild(Component *newcomponent);
        bool removeChild(std::string name);
        Component* getChild(std::string name);
        std::map<std::string, Component*> getAllChilds();
        virtual Component* clone()=0;
    };
}
#endif // COMPONENT_H
