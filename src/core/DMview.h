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
    std::vector<std::string> WriteAttributes;
    std::vector<std::string> ReadAttributes;
    int readType;
    int writeType;


public:
    View(std::string name);
    View(){}
    void addAttributes(std::string name);
    void getAttributes(std::string name);
    void addComponent(int Type);
    void getComponent(int Type);
    std::string getName(){return this->name;}
    std::vector<std::string> const & getWriteAttributes  () const {return WriteAttributes;}
    std::vector<std::string> const & getReadAttributes  () const {return ReadAttributes;}

    int const  & getReadType  () const {return readType;}
    int const  & getWriteType  () const {return writeType;}

    bool reads();
    bool writes();

};
}

#endif // DMVIEW_H
