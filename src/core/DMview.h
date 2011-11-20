#ifndef DMVIEW_H
#define DMVIEW_H

#include <map>
#include <vector>
#include <string>
namespace DM {
class View
    {
        int type;
        std::string name;
        std::vector<std::string> Attributes;
    public:
        View(std::string name, int type);
        View(){}
        void setAttributes(std::string name);
        std::string getName(){return this->name;}

    };
}

#endif // DMVIEW_H
