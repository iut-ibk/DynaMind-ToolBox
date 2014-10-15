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
    void setdev(std::string type, bool dev);

    int yearcycle;
    DM::View city;
    DM::View superblock;
    DM::View cityblock;
    DM::View parcel;
};

#endif // urbandevelTrigger_H
