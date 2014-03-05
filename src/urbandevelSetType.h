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
    DM::View city;
    DM::View sb;
    DM::View sb_cent;
};

#endif // urbandevelSetType_H
