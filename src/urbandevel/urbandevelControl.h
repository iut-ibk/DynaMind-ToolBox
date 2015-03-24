#ifndef urbandevelControl_H
#define urbandevelControl_H

#include <dmmodule.h>

class urbandevelControl: public DM::Module
{
    DM_DECLARE_NODE(urbandevelControl)

public:
    urbandevelControl();
    ~urbandevelControl();
    void run();
    void init();
    std::string getHelpUrl();

private:
    int startyear;
    int endyear;
    bool wpfromcity;
    int workplaces;
    int shareindwp;

    DM::View cityview;
};

#endif // urbandevelControl_H
