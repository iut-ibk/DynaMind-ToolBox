#ifndef urbandevelReduction_H
#define urbandevelReduction_H

#include <dmmodule.h>

class urbandevelReduction: public DM::Module
{
    DM_DECLARE_NODE(urbandevelReduction)
public:
    urbandevelReduction();
    ~urbandevelReduction();

    void run();
    void init();
private:
    void setdev(std::string type, bool dev);

    int yearcycle;
    DM::View city;
    DM::View superblock;
    DM::View cityblock;
    DM::View parcel;
};

#endif // urbandevelReduction_H
