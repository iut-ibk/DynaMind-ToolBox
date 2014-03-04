#ifndef urbandevelSetType_H
#define urbandevelSetType_H

#include <dmmodule.h>

class urbandevelSetType: public DM::Module
{
    DM_DECLARE_NODE(urbandevelSetType)
public:
    urbandevelSetType();
    ~urbandevelSetType();

    void run();
    void init();
private:
    bool setdev();
    bool setdec();

    int yearcycle;
    DM::View city;
    DM::View sb;
    DM::View cb;
    DM::View parcel;
    DM::View building;
};

#endif // urbandevelSetType_H
