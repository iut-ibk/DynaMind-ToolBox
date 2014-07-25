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
    double numbernearest;
    bool from_SB;
    bool from_nearest;

    DM::View city;
    DM::View sb;
    DM::View sb_cent;

    std::vector<std::string> typevec;
};

#endif // urbandevelSetType_H
