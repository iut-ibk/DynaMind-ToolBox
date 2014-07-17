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
    int yearcycle;
    int wp_com;
    int wp_ind;

    DM::View cityview;
};

#endif // urbandevelControl_H
