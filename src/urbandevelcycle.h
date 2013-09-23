#ifndef URBANDEVELCYCLE_H
#define URBANDEVELCYCLE_H

#include <dmmodule.h>

class UrbanDevelCycle: public DM::Module
{
    DM_DECLARE_NODE(UrbanDevelCycle)
public:
    UrbanDevelCycle();
    ~UrbanDevelCycle();

    void run();
    void init();
private:
    int yearcycle;
    int wp_com;
    int wp_ind;

    DM::View city;
    DM::View population;
};

#endif // MYMODULE_H
