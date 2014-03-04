#ifndef urbandevelSetHeight_H
#define urbandevelSetHeight_H

#include <dmmodule.h>

class urbandevelSetHeight: public DM::Module
{
    DM_DECLARE_NODE(urbandevelSetHeight)
public:
    urbandevelSetHeight();
    ~urbandevelSetHeight();

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

#endif // urbandevelSetHeight_H
