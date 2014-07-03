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
    bool setdev();
    bool setdec();

    int yearcycle;
    DM::View city;
    DM::View superblock;
    DM::View cityblock;
};

#endif // urbandevelTrigger_H
