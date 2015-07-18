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
    std::string heightView;
    std::string heightAttr;
    bool heightinmeter;
    int heightperstory;
    int numbernearest;
    int percentile;

    DM::View city;
    DM::View sb;
    DM::View sb_cent;
    DM::View hview;
    DM::View hview_cent;
};

#endif // urbandevelSetHeight_H
