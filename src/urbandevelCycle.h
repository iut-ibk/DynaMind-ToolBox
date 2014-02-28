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
private:
    int yearcycle;
    DM::View city;
};

#endif // urbandevelCycle_H
