#ifndef urbandevelTrigger_H
#define urbandevelTrigger_H

#include <dmmodule.h>

class urbandevelTrigger: public DM::Module
{
    DM_DECLARE_NODE(urbandevelTrigger)
public:
    urbandevelTrigger();
    ~urbandevelTrigger();

    void run();
    void init();
private:
    int yearcycle;
    DM::View city;
};

#endif // urbandevelTrigger_H
