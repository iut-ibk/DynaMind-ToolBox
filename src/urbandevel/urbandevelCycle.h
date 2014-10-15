#ifndef urbandevelCycle_H
#define urbandevelCycle_H

#include <dmmodule.h>

class urbandevelCycle: public DM::Module
{
    DM_DECLARE_NODE(urbandevelCycle)

public:
    urbandevelCycle();
    ~urbandevelCycle();
    void run();
    void init();
    std::string getHelpUrl();

private:    
    DM::View cityview;
    int yearcycle;
};

#endif // urbandevelCycle_H
