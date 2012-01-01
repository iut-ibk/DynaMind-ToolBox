#ifndef DMVIEW_H
#define DMVIEW_H

#include <map>
#include <vector>
#include <string>



namespace DM {


enum {
    READ,
    MODIFY,
    WRITE
};
class View
{
private:

    int type;
    std::string name;
    std::string IdofDummyComponent;
    int accesstypeGeometry;

    std::map<std::string, int> ownedAttributes;

public:
    View(std::string name, int type, int accesstypeGeometry = READ);
    View(){}

    void addAttributes(std::string name, int accesstype);


    void setIdOfDummyComponent(std::string UUID);
    std::string getIdOfDummyComponent();
    std::string getName(){return this->name;}
    std::vector<std::string>  getWriteAttributes  () const;
    std::vector<std::string>  getReadAttributes  () const;

    int const & getType() const {return type;}
    int const & getAccessType() const{return accesstypeGeometry;}
    bool reads();
    bool writes();

};
}

#endif // DMVIEW_H
